#ifndef MAP_H
#define MAP_H

// Pity, I don't like #includes in my headers...
#include "entity.h"

// Prints map to the screen
void print_map();

// Interacts with a tile at xy
// Returns true upon successful action, false upon failure (like walking 
// into a wall)
bool interact( int x, int y, entity* creature );

// This function is to be called once per level, if there is no save file
void gen_map();

// This function returns coordinates to an available open room
void get_open_space( int* endx, int* endy );
// Tells us how many squares there are
int count_open_spaces();

// This function dumps a char[BOARD_WIDTH][BOARD_HEIGHT] array of the map
char** dump_map_image();

bool load_map_image( char** map );

#endif
