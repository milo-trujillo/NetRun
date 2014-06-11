#include "io.h"
#include "player.h"
#include "map.h"
#include "save.h"

#ifndef NULL
#define NULL 0
#endif

//
// player() - Constructor for a player
//
// We set the coordinates to any open space on the map
//
player::player() : entity()
{
	is_visible = true;
	symbol = '@';
	hp = 10;
	max_hp = 10;
	get_open_space( &x, &y );
	pNext = NULL;
	pLast = NULL;
	damage = 5; // For now, just set a constant damage
}

//
// run() - The control layer between the user and the player is here
//
// This method handles major game logic surrounding the player's actions.
// Everything dictated by the user takes place here
//
// Note: The coordinates of the map are still based on curses
// ie, 0,0 is top left.
//
void player::run()
{
	bool done = false;
	while( done == false )
	{
		command input = get_command();
		switch( input )
		{
			case WEST:
			case EAST:
			case NORTH:
			case SOUTH:
			case NW:
			case NE:
			case SW:
			case SE:
				done = move( input );
				break;
			case QUIT:
				done = true;
				gen_map();
				clear_screen();
				break;
			case SAVE:
				done = true;
				save_game();
				break;
		}
	}
}

//
// move() - Hit a monster, or move to a square
//
// We calculate the coordinates we are trying to move to, then check if there is
// a monster there. If there is, we hit it. Otherwise, we interact with the tile
//
bool player::move( command d )
{
	int newx, newy;
	switch( d )
	{
		case NORTH:
			newx = x;
			newy = y - 1;
			break;
		case SOUTH:
			newx = x;
			newy = y + 1;
			break;
		case EAST:	
			newx = x + 1;
			newy = y;
			break;
		case WEST:
			newx = x - 1;
			newy = y;
			break;
		case NW:
			newx = x - 1;
			newy = y - 1;
			break;
		case NE:
			newx = x + 1;
			newy = y - 1;
			break;
		case SW:
			newx = x - 1;
			newy = y + 1;
			break;
		case SE:
			newx = x + 1;
			newy = y + 1;
			break;
		// This isn't a direction!
		default:
			return false;
	}
	entity* monster = get_entity_at( newx, newy );
	if( monster != NULL )
	{
		monster->hurt( damage );
		return true;
	}
	else
	{
		bool done;
		done = interact( newx, newy, this );
		return done;
	}
}

//
// kill() - Kills the player, triggering endgame
//
// Kill! Kill! Kill! Throws an exception.
//
void player::kill()
{
	throw;
}
