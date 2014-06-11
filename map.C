#include "map.h"
#include "io.h"
#include "rand.h"
#include "entity.h"
#include "bsp.h"

//
// =================================
// CUSTOM VARIABLE TYPES AND CLASSES
// =================================
//

//
// Basic tile types
// -----------------
// WALL - Impassible, no line of sight
// OPEN - Passable, line of site
// SPECIAL - Passable, line of site
//
// Special tiles cover zones like doors, traps, stores, stairs, etcetera
//
enum tile_type { WALL, OPEN, SPECIAL };

//
// Tile design
// -----------
// There is no 'walking' on a tile
// Instead, to enter a tile you call the tile's 'interact' method, passing
// yourself in as an argument. Interact returns true if the action succeeded,
// in which case your turn is done, or false if action failed, allowing you to
// take another.
//
// Interact can do many things. On a wall, it simply returns false. On an open
// space it will move you to that space. On a special square it may or may not
// move you to the square, but will fire off an auxiliary action like triggering
// a trap or opening a door.
//
// Note - the class can _not_ be instantiated directly, you must use an
// inheriting class
class tile
{
	// Note: put only public methods, not variables
	public:
		tile_type get_type()
		{
			return type;
		}
		void draw()
		{
			if( is_visible == true )
				display( x, y, symbol );
		}
		void set_visible( bool state )
		{
			is_visible = state;
		}
		bool get_visible()
		{
			return is_visible;
		}
		friend bool save_tile( tile* ); // For saving tiles to file
		virtual bool interact( entity* ) = 0; // This MUST be overridden
	protected:
		//Variables
		tile_type type;
		char symbol;
		bool is_visible;
		int x;
		int y;
		//item* inventory; // A linked list of items on the tile
		//Methods
};

//
// Wall class - Interaction is impossible
//
class wall : public tile
{
	public:
		wall( int startx, int starty )
		{
			x = startx;
			y = starty;
			symbol = '#';
			is_visible = false;
			type = WALL;
		}
		virtual bool interact( entity* creature )
		{
			if( creature->get_type() == PLAYER )
				display_message("You bump into a wall.");
			return false; // Can't walk through walls
		}
};

//
// Open space class - Interaction moves you into the space
//
class open : public tile
{
	public:
		open( int startx, int starty )
		{
			x = startx;
			y = starty;
			symbol = '.';
			is_visible = true;
			type = OPEN;
		}
		virtual bool interact( entity* creature )
		{
			creature->set_position(x, y);
			if( creature->get_type() == PLAYER )
			{
				clear_messages();
				select(x, y);
			}
			return true;
		}
};

//
// ================
// GLOBAL VARIABLES
// ================
//

// Here's where we store ALL THE THINGS
tile* board [BOARD_WIDTH][BOARD_HEIGHT];
bool map_is_ready = false;

//
// =====================
// FUNCTION DECLARATIONS
// =====================
//

void prime_visibility();

//
// =======================
// FUNCTIONS (not methods)
// =======================
//

//
// gen_map() - Create a map of tiles, setting visibility and type correctly
//
// We utilize BSP to get a grid of bools, then make our own grid of tiles.
// We convert 'true' bools to open space, and 'false' bools to walls.
//
void gen_map()
{
	// bool_board has dimensions BOARD_WIDTH and BOARD_HEIGHT
	// But we couldn't return a two dimensional array,
	// so here we are...
	bool** bool_board = gen_bsp();
	// Now, convert from bools to a tile grid
	for( int x = 0; x < BOARD_WIDTH; x++ )
	{
		for( int y = 0; y < BOARD_HEIGHT; y++ )
		{
			if( bool_board[x][y] == true )
			{
				board[x][y] = new open(x, y);
			}
			else
			{
				board[x][y] = new wall(x, y);
			}
		}
	}
	// Now set visibility of walls correctly
	prime_visibility();
	map_is_ready = true;
}

//
// prime_visibility() - Make wall tiles adjacent to rooms visible
//
// We loop over all tiles, and loop over each of their neighbors.
// If a neighbor is open, and it is visible, then we set the square to visible.
// 
void prime_visibility()
{
	for( int x = 0; x < BOARD_WIDTH; x++ )
	{
		for( int y = 0; y < BOARD_HEIGHT; y++ )
		{
			for( int tilex = x - 1; tilex <= x + 1; tilex++ )
			{
				if( tilex < 0 || tilex >= BOARD_WIDTH )
					continue;
				for( int tiley = y - 1; tiley <= y + 1; tiley++ )
				{
					if( tiley < 0 || tiley >= BOARD_HEIGHT )
						continue;
					tile* cell = board[tilex][tiley];
					if( cell->get_visible() && cell->get_type() == OPEN )
					{
						board[x][y]->set_visible(true);
					}
				}
			}
		}
	}
}

//
// print_map() - Print out the map to the screen
//
// We loop over all tiles, and tell them to draw
//
void print_map()
{
	if( map_is_ready == false )
		return;
	for( int x = 0; x < BOARD_WIDTH; x++ )
		for( int y = 0; y < BOARD_HEIGHT; y++ )
			board[x][y]->draw();
}

//
// get_open_space() - Get coordinates of an open room
//
// This function is used if an entity needs an open square to spawn in.
// We basically just choose random squares until we get one that works.
//
void get_open_space( int* endx, int* endy )
{
	bool done = false;
	do
	{
		int x = random( 0, BOARD_WIDTH );
		int y = random( 0, BOARD_HEIGHT );
		if( board[x][y]->get_type() == OPEN )
		{
			*endx = x;
			*endy = y;
			done = true;
		}
	}
	while( done == false );
}

//
// interact() - Interact with a tile
//
// This serves as a separation layer between the tiles in the map and the rest
// of the code. Via this function, no one has to know our internal map design.
//
bool interact( int x, int y, entity* creature )
{
	// We can never interact with a square off screen
	if( x < 0 || y < 0 || x >= BOARD_WIDTH || y >= BOARD_HEIGHT )
		return false;
	bool result = board[x][y]->interact( creature );
	return result;
}

//
// dump_map_image() - Dumps a 2d char array representing the map
//
// We fill out a 2d array with 'O' 'W' 'S' for open, wall, and special spaces.
// Special spaces will require additional data in the save file later.
//
char** dump_map_image()
{
	// We'll have to allocate the array by hand since we intend to return it
	//char map[BOARD_WIDTH][BOARD_HEIGHT];
	char** map = new char*[BOARD_WIDTH];
	for( int x = 0; x < BOARD_WIDTH; x++ )
	{
		map[x] = new char[BOARD_HEIGHT];
		for( int y = 0; y < BOARD_HEIGHT; y++ )
		{
			switch( board[x][y]->get_type() )
			{
				case WALL:
					if( board[x][y]->get_visible() )
						map[x][y] = 'W';
					else
						map[x][y] = 'w';
					break;
				case OPEN:
					if( board[x][y]->get_visible() )
						map[x][y] = 'O';
					else
						map[x][y] = 'o';
					break;
				case SPECIAL:
					map[x][y] = 'S';
					break;
				default:
					map[x][y] = '!';
					break;
			}
		}
	}
	return map;
}

bool load_map_image(char** map)	
{
	for( int x = 0; x < BOARD_WIDTH; x++ )
	{
		for( int y = 0; y < BOARD_HEIGHT; y++ )
		{
			switch( map[x][y] )
			{
				case 'W':
				case 'w':
					board[x][y] = new wall(x, y);
					if( map[x][y] == 'W' )
						board[x][y]->set_visible(true);
					break;
				case 'O':
				case 'o':
					board[x][y] = new open(x, y);
					if( map[x][y] == 'O' )
						board[x][y]->set_visible(true);
					break;
			}
		}
	}
	map_is_ready = true;
	return true;
}
		
//
// count_open_spaces() - returns a count of how many open spaces there are
//
// We just loop over every tile and check its type.
// Function doesn't run unless board is primed.
//
int count_open_spaces()
{
	if( map_is_ready == false )
		return 0;
	int count = 0;
	for( int x = 0; x < BOARD_WIDTH; x++ )
	{
		for( int y = 0; y < BOARD_HEIGHT; y++ )
		{
			if( board[x][y]->get_type() == OPEN )
				count++;
		}
	}
	return count;
}
