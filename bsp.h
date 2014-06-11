#ifndef BSP_H
#define BSP_H

#include "config.h"

// Note: This file ONLY needs to be included by the map

// Ugly hack, but I need to return a 2D bool array
//typedef bool bool_grid [BOARD_WIDTH][BOARD_HEIGHT];

bool** gen_bsp(); // Handles everything about BSP, passes off results to map code

#endif
