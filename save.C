#include <unistd.h> // For getuid()
#include <fstream> // For file streams
//#include <iostream> // For strings
//#include <sstream> // For combining ints and strings in a pretty way LINUX ONLY

// We need to link in a lot of things since we'll be saving data across the
// board.
#include "config.h"
#include "save.h"
#include "map.h"
#include "monster.h"
#include "main.h"
#include "player.h"

using namespace std;

//
// ================
// GLOBAL VARIABLES
// ================
//

// Files to use
ofstream savefile;
ifstream loadfile;
ofstream playersavefile;
ifstream playerloadfile;

//
// =====================
// FUNCTION DECLARATIONS
// =====================
//
bool save_level();
bool save_player();
string get_level_filename( int level = get_level() );
string get_player_filename();
bool load_game();
bool load_player();
bool load_level( int level_number );

//
// ====================
// FUNCTION DEFINITIONS
// ====================
//

//
// save_game() - Saves the current level state and the player state to disk
//
// We return false if either save function fails, but otherwise return true
//
bool save_game()
{
	//  Save player appears to be returning false...
	if( save_player() && save_level() )
		return true;
	else
		return false;
}

//
// load_game() -  Load the player and level from disk
//
// We load the player from disk, and if it succeeds, that will load the level
// from disk.
//
bool load_game()
{
	// If we load the player okay _and_ the level works
	if( load_player() )
	{
		return true;
	}
	else // Set everything to default values
	{
		set_level( 1 );	// Set default level number
		return false;
	}
}

//
// save_level() - We dump the current level to disk, in UID-level#.save
//
// We save the level in the format of a block of OWS characters representing
// the two dimensional grid of the map. After that we'll dump extra data for
// all the 'special' tiles.
//
bool save_level()
{
	string filename = get_level_filename();
	savefile.open(filename.c_str());
	if( ! savefile.good() )
		return false;

	// Dump the char block of the map
	char** map = dump_map_image();
	for( int y = 0; y < BOARD_HEIGHT; y++ )
	{
		for( int x = 0; x < BOARD_WIDTH; x++ )
		{
			savefile << map[x][y];
		}
		savefile << endl;
	}
	savefile << save_monsters() << endl;
		
	savefile.close();
	return true;
}

//
// get_level_filename() - Returns a string for the filename of the current level
//
// We use the uid and level number to generate a filename for the save file.
//
string get_level_filename( int level )
{
	int uid = getuid();
	string filename = SAVEDIR;
	filename.append(int_to_string(uid));
	filename.append("-");
	filename.append(int_to_string(level));
	filename.append(".save");
	return filename;
}

//
// get_player_filename() - Returns a string for the filename of the player
//
// We use the uid to generate a filename for the save file.
//
string get_player_filename()
{
	int uid = getuid();
	string filename = SAVEDIR;
	filename.append(int_to_string(uid));
	filename.append("-");
	filename.append("player.save");
	return filename;
}

//
// load_level - Load a level from a save file, if the save is available
//
// We take a level number, open the appropriate file, and load it up
//
bool load_level( int level_number )
{
	string filename = get_level_filename();
	loadfile.open(filename.c_str());
	if( ! loadfile.good() )
		return false;

	// Allocate space
	char** map = new char*[BOARD_WIDTH];
	for( int x = 0; x < BOARD_WIDTH; x++ )
		map[x] = new char[BOARD_HEIGHT];

	// Load up that sucker
	for( int y = 0; y < BOARD_HEIGHT; y++ )
	{
		string line;
		getline( loadfile, line );
		for( int x = 0; x < BOARD_WIDTH; x++ )
		{
			map[x][y] = line.at(x);
			//cout << line.at(x); // For debugging
		}
		//cout << endl;
	}

	// Now set the map based off of this
	load_map_image( map );	

	//load_monsters();

	return true;
}

//
// load_monsters()
//
// Load the monsters from disk
//



//
// save_player() - We dump the player to disk in UID-player.save
//
// We save off the stats of the player, followed by everything in their
// inventory.
//
bool save_player()
{
	string filename = get_player_filename();
	playersavefile.open(filename.c_str());
	if( ! playersavefile.good() )
		return false;

	player user = export_player();
	playersavefile << user.x << endl;
	playersavefile << user.y << endl;
	playersavefile <<  user.hp << endl;
	playersavefile << user.max_hp << endl;
	playersavefile << get_level() << endl;
	playersavefile << get_turn() << endl;

	playersavefile.close();
	return true;
}

//
// load_player() - Read the player and level from disk
//
// We read in values from the save file according to spec, then return them all
// the way up to main.
//
bool load_player()
{
	string filename = get_player_filename();
	playerloadfile.open(filename.c_str());
	if( ! playerloadfile.good() )
		return false;
	
	int level, turn;
	int x, y, hp, max_hp;

	playerloadfile >> x;
	playerloadfile >> y;
	playerloadfile >> hp;
	playerloadfile >> max_hp;
	playerloadfile >> level;
	playerloadfile >> turn;

	load_level( get_level() );
	set_level( level );
	set_turn( turn );
	
	player* user = new player;
	user->x = x;
	user->y = y;
	user->hp = hp;
	user->max_hp = max_hp;

	import_player( user );

	return true;
}
