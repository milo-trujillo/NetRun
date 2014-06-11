#ifndef SAVE_H
#define SAVE_H

// The save functionality is dirty and unappreciated. Unfortunately, we must
// link it into just about everything to save off the map and entities.
// Any significant changes in those architectures will need to be fixed here
// as well

// Speaking of which, need this to made load_game() work
#include "player.h"

// save_game will save both the level and the player to disk.
bool save_game();
bool load_game();
// save_level is for dumping the individual level to disk (for descending the
// dungeon). These must be exposed because there are times we would want to save
// an individual level without saving off the whole game and quitting.
bool save_level();
bool load_level( int level_number );

// Player save file format (UID-player.save)
// -----------------------------------------
// one value on each line for:
// - x
// - y
// - hp
// - max hp
// - dungeon level
//
// Dungeon save file format (UID-level#.save)
// ------------------------------------------
// We have a big grid of chars representing map status.
// The chars it can hold are:
// W - Visible Wall
// w - Invisible Wall
// O - Visible Open Space
// o - Invisible Open Space
// S - Special square (any type)

#endif
