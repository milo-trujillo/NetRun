#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

/*
 * Note: Serious game logic takes place in player::run()
 * All user input is collected and sorted there to determine
 * what actions the player (and game) will take
*/

class player : public entity
{
	public:
		player(); // Constructor, defined in C file
		virtual void run(); // User / Player interaction
		friend bool save_player();
		friend bool load_player();
	private:
		virtual void kill();
		bool move( command ); // Tells the player to interact with a tile
};

#endif
