#ifndef MAIN_H
#define MAIN_H

// These functions are for getting and setting gamewide variables
int get_turn();
int get_level();
void set_turn( int newturn );
void set_level( int newlevel );

#include "player.h"

// These are to be used for save functionality only
player export_player();
void import_player( player* user );

// This is for entity linked list recursive code
entity* get_head_entity();

#endif
