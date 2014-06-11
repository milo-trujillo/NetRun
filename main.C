#include "main.h"
#include "io.h"
#include "map.h"
#include "rand.h"
#include "save.h"
#include "entity.h"
#include "player.h"
#include "status.h"
#include "monster.h"

#include <math.h> // For exponent work

//
// ================
// GLOBAL VARIABLES
// ================
//

int turn = 0;
int level = 1;
entity* headlife;

//
// =========
// FUNCTIONS
// =========
//

int main()
{
	// Some global vars
	const int initial_monster_count = 30;
	float multiply_rate = 0;

	// Game initialization
	init_display();
	seed_random();
	if( load_game() == false )
	{
		gen_map();
		headlife = new player;
	}
	display_message("Select a growth rate: ");
	multiply_rate = get_float();
	make_monsters( initial_monster_count, headlife );
	print_map();
	display_message("Welcome to NetRun!\n");

	// Now we set up some stuff for population growth
	int num_open_spaces = count_open_spaces();
	int max_monsters = num_open_spaces - 1;
	//float A = (max_monsters / initial_monster_count) - 1;
	int ideal_monster_count = initial_monster_count;

	// Main event loop
	for( turn = get_turn(); true; turn++ )
	{
		print_map();
		draw_entities( headlife );
		print_turn();

		// Here comes our big block for growing monsters
		if( turn % 30 == 0 && turn > 0 && get_entity_count() < num_open_spaces )
		{
			// Population growth equation:	P = M/(1 + Ae^(-Mkt))
			// We assume a 't' of 1, because the equation is reset each turn
			float A = (max_monsters / (get_entity_count() - 1)) - 1;
			ideal_monster_count = float(max_monsters) / (1.0 + A * exp(-1.0 * multiply_rate));
			int grow = ideal_monster_count - (get_entity_count() - 1);
			multiply_monsters( get_first_monster(), grow );
			print_calculus(get_entity_count() - 1, num_open_spaces - 1, multiply_rate, A);
		}
		else
		{
			float A = (max_monsters / (get_entity_count() - 1)) - 1;
			print_calculus(get_entity_count() - 1, num_open_spaces - 1, multiply_rate, A);
		}

		// Aaaand back to the regular game
		select( headlife->get_x(), headlife->get_y() );
		refresh_screen();
		run_entities( headlife );
	}
	end_display();
	return 0;
}

// Utility functions, mostly for the save file code

int get_turn()
{
	return turn;
}

int get_level()
{
	return level;
}

void set_turn( int newturn )
{
	turn = newturn;
}

void set_level( int newlevel )
{
	level = newlevel;
}

player export_player()
{
	// We need to use a fancy-ass dynamic case, because headlife is a
	// pointer to an entity, not a player, so I can't just return a
	// dereferenced *headlife.
	player* user = dynamic_cast< player* >( headlife );
	return *user;
	//return *headlife;
}

// This one is for the recursive functions in entity code
entity* get_head_entity()
{
	return headlife;
}

void import_player( player* user )
{
	headlife = user;
}
