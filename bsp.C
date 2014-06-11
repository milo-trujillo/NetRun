#include "bsp.h"
#include "rand.h"
#include "config.h"

//
// BSP (Binary Space Partitioning) is responsible for our dungeon generation
// It works in three phases:
//
// 1 - Subdivide the map into rectangles of the size we want
// 2 - Fill those rectangles with rooms, connect those together with tunnels
// 3 - Export the BSP grid to a map in map.C / map.h
//

//
// =========================
// DEBUGGING CONTROL SECTION
// =========================
//

// NOTICE: To control debugging, toggle the following
//#define DEBUG_BSP 	// For the bsp_cell cuts
//#define DEBUG_ROOMS	// For the rooms placed in bsp_cells, and tunnels
//#define DEBUG_TREE	// For printing a tree of pointers to a file

#ifdef DEBUG_BSP
	#include "curses.h"
	void debug_display_map( struct bsp_cell, char symbol );
#endif
#ifdef DEBUG_ROOMS
	#include "curses.h"
#endif
#ifdef DEBUG_TREE
	#include <iostream>
	#include <fstream>
	using namespace std;
	int tree_depth = 0;
	ofstream treefile;
#endif
#ifndef NULL
	#define NULL 0
#endif

//
// =====================
// CUSTOM VARIABLE TYPES
// =====================
//

//
// This is a BSP cell. No one outside this file needs to know about it.
//
struct bsp_cell // BSP style rectangles
{
	int height;
	int width;

	// Upper left co-ordinates
	int x;
	int y;

	// Mark if this is the deepest cell
	bool deepest;

	// To build a tree diagram of cells
	bsp_cell* pParent;
	bsp_cell* pChildA;
	bsp_cell* pChildB;
};

// Used to describe which direction we should cut things
enum direction { VERT, HORIZ, RAND };

//
// ================
// GLOBAL CONSTANTS
// ================
//

// Used to decide the size of the primary cell, and the bool array
// No longer defined here because we get them from config.h
//const int BOARD_WIDTH = 80;
//const int BOARD_HEIGHT = 22;

// Minimum dimensions a rectangle is allowed to have
const int MIN_HEIGHT = 6;
const int MIN_WIDTH = 8; 

// Maximum dimensions a deepest-level rectangle should have
// In other words, if a rectangle is smaller than these, make it deepest
// NOTE: These MUST be at least twice the min width and height
const int MAX_DEEP_HEIGHT = 18;
const int MAX_DEEP_WIDTH = 22;
// Old values were 12 and 16

// How many rows or collums deep of rooms do we go before placing a tunnel?
const int TUNNEL_PROXIMITY = 5;

// Minimum dimensions a is allowed to have
const int MIN_ROOM_HEIGHT = 4;
const int MIN_ROOM_WIDTH = 6;

// How close before we start choosing cuts manually
const int OVERRIDE_WIDTH = MIN_WIDTH + 6;
const int OVERRIDE_HEIGHT = MIN_HEIGHT + 6;


//
// =====================
// FUNCTION DECLARATIONS
// =====================
//
// Global functions and functions for phase 1
//

// Sets up the very first cell, no randomization
bsp_cell init_first_cell();

// This take a rectangle, choose how to split it, then do so
void split( bsp_cell* );
void split_horizontal( bsp_cell* );
void split_vertical( bsp_cell* );

// This for overriding our random-direction cuts to improve cell count
direction decide_chop( struct bsp_cell );

// This fills cells with rooms
void fill_cell( bsp_cell* );

// This clears the game board, and sets its dimensions correctly
void clear_rooms();

// Exports BSP grid to map code
bool export_bsp();

// Some debug functions we may or may not want

#ifdef DEBUG_TREE
void debug_tree( bsp_cell* );
#endif

#ifdef DEBUG_ROOMS
void debug_rooms();
#endif


//
// ================
// GLOBAL VARIABLES
// ================
//

// This keeps track of open space (rooms and tunnels) across the board
// True means open space, false means closed wall
bool** room;

//
// ====================
// FUNCTION DEFINITIONS
// ====================
//

//
// gen_bsp() - Only function exposed to outside
//
// Generates a dungeon via BSP and places rooms and tunnels in it
// Then exports to a map. Should only be called once per level.
//
bool** gen_bsp()
{
	bsp_cell init_cell = init_first_cell();
	clear_rooms();
	split( &init_cell );

	#ifdef DEBUG_TREE
	treefile.open ("tree-debug.txt");
	debug_tree( &init_cell );
	#endif

	fill_cell( &init_cell );

	#ifdef DEBUG_TREE
	treefile.close();
	#endif

	#ifdef DEBUG_ROOMS
	debug_rooms();
	#endif

	return room;
}

bsp_cell init_first_cell()
{
	bsp_cell init_cell;
	init_cell.height = BOARD_HEIGHT;
	init_cell.width = BOARD_WIDTH;
	init_cell.x = 0;
	init_cell.y = 0;
	init_cell.deepest = false;
	init_cell.pParent = NULL;
	return init_cell;
}

//
// split() - Takes a rectangle, decides how to split it
//
// The function chooses to split a bsp_cell horizontally or vertically.
// It chooses randomly unless overridden with direction parameter
//
void split( bsp_cell* rect )
{
	direction overload = decide_chop( *rect );
	if( overload == VERT )
		split_vertical( rect );
	else if( overload == HORIZ )
		split_horizontal( rect );
	else
	{
		int direction = random( 0, 20 ); // Choose direction to split
		if( direction % 2 ) // Split vertically
			split_vertical( rect );
		else
			split_horizontal( rect );
	}
}

//
// split_vertical() - Splits a bsp_cell vertically, making two cells
//
// 'Vertical' means we are left with a vertical line between the cells.
// Sets up dimensions, co-ordinates, and pointers, for child cells
//
void split_vertical( struct bsp_cell* rect )
{
	// First step - do we even _want_ to split the cell?
	if( rect->width < MAX_DEEP_WIDTH )
	{
		rect->deepest = true;
		rect->pChildA = NULL; // Should be unnecessary, but helps debug
		rect->pChildB = NULL; // Shouldn't use children if deepest cell
		return;
	}

	bsp_cell* left = new bsp_cell;
	bsp_cell* right = new bsp_cell;

	// Keep on cutting until we get good cells
	do
	{
		// Set up parent list
		left->pParent = rect;
		right->pParent = rect;

		// Set up sizes based on parent
		left->x = rect->x;
		left->y = rect->y;
		left->height = rect->height;
		right->height = rect->height;

		// Now decide how wide to make the cell
		left->width = random( MIN_WIDTH, rect->width / 2 );
		right->width = rect->width - left->width;
		right->x = rect->x + left->width;
		right->y = rect->y;
	} 
	while( left->width < MIN_WIDTH || right->width < MIN_WIDTH );

	// These cells are now approved, set up the pointers
	rect->pChildA = right;
	rect->pChildB = left;
	rect->deepest = false;
	#ifdef DEBUG_BSP
	debug_display_map( *left, 'L' );
	debug_display_map( *right, 'R' );
	#endif
	split( left );
	split( right );
}

//
// split_horizontal() - splits a cell horizontally creating two bsp_cells.
//
// "Horizontal" means making a horizontal line between the cells.
// Sets up co-ordinates, dimensions, and pointers of child cells.
//
void split_horizontal( struct bsp_cell* rect )
{
	// First step - do we even _want_ to split the cell?
	if( rect->height < MAX_DEEP_HEIGHT )
	{
		rect->deepest = true;
		rect->pChildA = NULL; // Should be unnecessary, but helps debug
		rect->pChildB = NULL; // Shouldn't use children if deepest cell
		return;
	}
	
	bsp_cell* top = new bsp_cell;
	bsp_cell* bottom = new bsp_cell;
	
	do
	{
		// Set up pointers
		top->pParent = rect;
		bottom->pParent = rect;
	
		// Set up coordinates based on parent
		top->x = rect->x;
		top->y = rect->y;
		top->width = rect->width;
		bottom->width = rect->width;
	
		// Now decide how tall to make the cell
		top->height = random( MIN_HEIGHT, rect->height / 2 );
		bottom->height = rect->height - top->height;
		bottom->x = rect->x;
		bottom->y = rect->y + top->height;
	}
	while( top->height < MIN_HEIGHT || bottom->height < MIN_HEIGHT );

	// These cells are now approved, set up the pointers
	rect->pChildA = top;
	rect->pChildB = bottom;
	rect->deepest = false;
	#ifdef DEBUG_BSP
	debug_display_map( *top, 'T' );
	debug_display_map( *bottom, 'B' );
	#endif
	split( top );
	split( bottom );
}


//
// decide_chop() - Decides if we need to cut a certain way to maximize bsp_cells
//
// If a cell is too skinny or short we make sure to cut the other way.
// We determine this using a buffer limit.
// This maximizes the chance that we get more sub-cells
//
direction decide_chop( struct bsp_cell cell )
{
	// If the thing is barely wide enough, cut it horizontally
	if( cell.width <= OVERRIDE_WIDTH )
		return HORIZ;
	// If the thing is barely tall enough, cut vertically
	if( cell.height <= OVERRIDE_HEIGHT )
		return VERT;
	else
		return RAND;
}


//
// debug_display_map() - visibly show where cuts for bsp_cells were made
//
// Iterates over co-ordinates in a cell, writes a character in them
// We wrap it in ifdefs so there can't be debug code in use on a non-debug build
//
#ifdef DEBUG_BSP
void debug_display_map( struct bsp_cell rect, char symbol )
{
	for( int x = 0; x < rect.width; x++ )
		for( int y = 0; y < rect.height; y++ )
			mvaddch(rect.y + y, rect.x + x, symbol);
	// Now make sure to write blanks over the "status" line
	for( int x = 0; x < BOARD_WIDTH; x++ )
		for( int y = 22; y < 24; y++ )
			mvaddch(y, x, ' ');
}
#endif


// =====================================================
//
// END OF BSP_CELL CODE - START OF ROOM PLACEMENT CODE
//
// =====================================================

//
// =====================
// FUNCTION DECLARATIONS
// =====================
//
// Functions here are not accessible to code above, helps separate code
//

void assimilate_children( bsp_cell* parent );
void free_children( bsp_cell* cell );
void gen_room( bsp_cell* cell );
void make_tunnel( int aX, int aY, int bX, int bY );
void clear_rooms();
int siblings( int, int );

//
// ==================
// START OF FUNCTIONS
// ==================
//

//
// fill_cell() - Recursively traverse tree of cells, end when we have one cell
//
// Descends tree of cells, fills deepest cells with rooms, consolidates children
// into their parents, then adds tunnels and frees the children.
// Continuing back up, we keep combining cells into parents until we hit top.
// Therefore in the end we have only one cell ready for export to the map.
//
void fill_cell( bsp_cell* cell )
{
	// Abort immediately if we have a bad pointer!
	if( cell == NULL )
		return;
	if( cell->deepest == true )
	{
		gen_room( cell );
	}
	else if( cell->deepest == false )
	{
		fill_cell( cell->pChildA );
		fill_cell( cell->pChildB );
		// If this line is reached, both children have been filled
		assimilate_children( cell );
	}
}

//
// clear_rooms() - Set the entire room[][] to false
//
// We prime the array to false to make sure no random data screws things up
//
void clear_rooms()
{
	room = new bool*[BOARD_WIDTH];
	for( int x = 0; x < BOARD_WIDTH; x++ )
	{
		room[x] = new bool[BOARD_HEIGHT];
		for( int y = 0; y < BOARD_HEIGHT; y++ )
			room[x][y] = false;
	}
}

//
// gen_room() - Places a room in a cell
//
// We make a room of semi-random size, place it randomly in the cell.
// This room is represented by bools in a 2-dimensional array
//
void gen_room( bsp_cell* cell )
{
	// Choose a room of random size and location to fit the cell
	int width = random( MIN_ROOM_WIDTH, cell->width - 2 );
	int height = random( MIN_ROOM_HEIGHT, cell->height - 2 );
	int roomx = random( cell->x + 1, cell->x + cell->width - 1 - width );
	int roomy = random( cell->y + 1, cell->y + cell->height - 1 - height );

	// Now place the room in the global grid
	for( int x = roomx; x < roomx + width; x++ )
		for( int y = roomy; y < roomy + height; y++ )
			room[x][y] = true;

	#ifdef DEBUG_ROOMS
	debug_rooms();
	#endif
}

//
// assimilate_children - Combine the children's rooms to make a parent room
//
// We take a random point in the rooms of both child cells. The point must
// be open, and have more or less than 3 neighbors (can't be a corner).
// Then we make a tunnel between these two points, and free the children.
//
void assimilate_children( bsp_cell* parent )
{
	// Let's forget the cells for a minute, deal only with coordinates
	
	int aStartX = parent->pChildA->x;
	int aStartY = parent->pChildA->y;
	int aEndX = parent->pChildA->width + aStartX;
	int aEndY = parent->pChildA->height + aStartY;

	int bStartX = parent->pChildB->x;
	int bStartY = parent->pChildB->y;
	int bEndX = parent->pChildB->width + bStartX;
	int bEndY = parent->pChildB->height + bStartY;

	// NOW we want to restrict tunnel positions to the rooms near eachother
	// This is so a tunnel doesn't burrow through multiple rooms if possible
	// We attempt this by selecting tunnel points from within the first 5
	// or so rows / collums with open space in them.

	/*
	if( aStartY == bStartY ) // We have LEFT / RIGHT cells
	{
		// Old method please ignore
		//aStartX = (aEndX - aStartX) / 2 + aStartX;
		//bEndX = (bEndX - bStartX) / 2 + bStartX;
		
		int count = 0;
		int x;
		for( x = aEndX; count < TUNNEL_PROXIMITY; x-- )
		{
			for( int y = aStartY; y < aEndY; y++ )
			{
				if( room[x][y] == true )
				{
					count++;
					break;
				}
			}
		}

		aStartX = x; // Here's the cutoff for A

		count = 0;
		for( x = bStartX; count < TUNNEL_PROXIMITY; x++ )
		{
			for( int y = aStartY; y < aEndY; y++ )
			{
				if( room[x][y] == true )
				{
					count++;
					break;
				}
			}
		}

		bEndX = x; // And here's the cutoff for B
	}
	else if( aStartX == bStartX ) // We have TOP / BOTTOM cells
	{
		// Old method please ignore
		//aStartY = (aEndY - aStartY) / 2 +  aStartY;
		//bEndY = (bEndY - bStartY) / 2 +  bStartY;

		int count = 0;
		int y;
		for( y = aEndY; count < TUNNEL_PROXIMITY; y-- )
		{
			for( int x = aStartY; x < aEndY; x++ )
			{
				if( room[x][y] == true )
				{
					count++;
					break;
				}
			}
		}

		aStartY = y; // Here's the cutoff for A

		count = 0;
		for( y = bStartY; count < TUNNEL_PROXIMITY; y++ )
		{
			for( int x = aStartX; x < aEndX; x++ )
			{
				if( room[x][y] == true )
				{
					count++;
					break;
				}
			}
		}

		bEndY = y; // And here's the cutoff for B
	}
	*/

	int aX;
	int aY;
	int bX;
	int bY;

	do
	{
		aX = random( aStartX, aEndX );
		aY = random( aStartY, aEndY );
	}
	while( room[aX][aY] == false && siblings(aX, aY) != 3 );

	do
	{
		bX = random( bStartX, bEndX );
		bY = random( bStartY, bEndY );
	}
	while( room[bX][bY] == false && siblings(bX, bY) != 3 );

	// Okay, now we have two random points in the children's rooms
	// And we know they aren't corners
	// Time to connect them with tunnels
	
	make_tunnel( aX, aY, bX, bY );

	// Now let's free the children
	free_children( parent );
}

//
// make_tunnel() - Draws a tunnel between two points
//
// We make either a straight line, or a line with one turn, between points
//
void make_tunnel( int aX, int aY, int bX, int bY )
{
	if( aY == bY ) // Can we make a straight line?
	{
		// Yes, make a horizontal line
		if( aX < bX )
			for( int x = aX; x < bX; x++ )
				room[x][aY] = true;
		else
			for( int x = aX; x > bX; x-- )
				room[x][aY] = true;
	}
	else if( aY > bY )
	{
		// No, make a vertical line
		for( int y = bY; y <= aY; y++ )
		{
			room[aX][y] = true;
		}
		make_tunnel( aX, aY, bX, aY );
	}
	else if( bY > aY )
	{
		// No, make a vertical line
		for( int  y = aY; y <= bY; y++ )
		{
			room[aX][y] = true;
		}
		make_tunnel( aX, bY, bX, bY );
	}
}


//
// siblings() - Returns the number of neighboring cells that are open
//
// We count all 'true' cells in a 3x3 grid, subtract one, and return it.
//
int siblings( int cellx, int celly )
{
	int count = 0;
	for( int x = cellx - 1; x <= cellx + 1; x++ )
	{
		if( x < 0 || x >= BOARD_WIDTH )
			continue;
		for( int y = celly - 1; y <= celly + 1; y++ )
		{
			if( y < 0 || y >= BOARD_HEIGHT )
				continue;
			if( room[x][y] == true )
				count++;
		}
	}
	return count - 1;
}

//
// free_children() - Free the memory of child cell resources
//
// We kill off the children, delete them, then set their pointers to null.
// While we're at it, we mark the cell as deepest for good measure.
void free_children( bsp_cell* cell )
{
	delete cell->pChildA;
	delete cell->pChildB;
	cell->pChildA = NULL;
	cell->pChildB = NULL;
	cell->deepest = true;
	return;
}

#ifdef DEBUG_ROOMS
//
// debug_rooms() - Prints the board to the screen for debugging
//
// We minimally loop over the board and print 'X' on all open spaces
//
void debug_rooms()
{
	for( int x = 0; x < BOARD_WIDTH; x++ )
		for( int y = 0; y < BOARD_HEIGHT; y++ )
			if( room[x][y] == true )
				mvaddch( y, x, 'X' );
	// Now make sure to write blanks over the "status" line
	for( int x = 0; x < 80; x++ )
		for( int y = 22; y < 24; y++ )
			mvaddch(y, x, ' ');
	refresh();
}
#endif

#ifdef DEBUG_TREE
//
// debug_tree() - Prints the whole BSP tree to disk
//
// We recursively follow the entire tree and dump it to file for browsing
//
void debug_tree( bsp_cell* cell )
{
	for( int i = 0; i < tree_depth; i++ )
		treefile << "\t";
	treefile << cell << endl;
	for( int i = 0; i < tree_depth; i++ )
		treefile << "\t";
	treefile << cell->x << endl;
	for( int i = 0; i < tree_depth; i++ )
		treefile << "\t";
	treefile << cell->y << endl;
	for( int i = 0; i < tree_depth; i++ )
		treefile << "\t";
	treefile << cell->width << endl;
	for( int i = 0; i < tree_depth; i++ )
		treefile << "\t";
	treefile << cell->pChildA << endl;
	for( int i = 0; i < tree_depth; i++ )
		treefile << "\t";
	treefile << cell->pChildB << endl;
	for( int i = 0; i < tree_depth; i++ )
		treefile << "\t";
	treefile << cell->height << endl << endl;
	if( cell->deepest == false )
	{
		tree_depth++;
		debug_tree( cell->pChildA );
		debug_tree( cell->pChildB );
	}
	tree_depth--;
}
#endif
