#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <iostream>
#include "util.h" // Utility functions like int_to_string

// This file is for configuring constants the majority of the codebase may need
// access to. These values generally don't have to do with a specific module, 
// but broader config.

// These define the size of the game board that pieces can be on
// For example, if we want a 2 line status bar on an 80x24 terminal,
// mark this as 80x22.
const int BOARD_WIDTH = 80;
const int BOARD_HEIGHT = 21;
// Where is the upper left corner of the board?
const int BOARD_X = 0;
const int BOARD_Y = 1;

// Not used for anything yet, but might as well stick the name in
const int STATUS_HEIGHT = 2;
const int STATUS_X = 0;
const int STATUS_Y = 22;

// Used for messages at top of screen
const int MESSAGE_HEIGHT = 1;
const int MESSAGE_X = 0;
const int MESSAGE_Y = 0;

const std::string SAVEDIR = "save/";

#endif
