#ifndef IO_H
#define IO_H

// These are the various commands that a user can enter
enum command { WEST, EAST, NORTH, SOUTH, NW, NE, SW, SE, QUIT, SAVE };

// This file is mostly just a wrapper for curses
// But it allows us to forget about offsets in the rest of the code
// And we don't have to think about the curses y,x coordinate system

void init_display();
void display( int x, int y, char symbol );
void display_message( const char* message );
void display_status( int line, const char* message );
void clear_messages();
void refresh_screen();
void end_display();
void select( int x, int y );
void clear_screen();
int get_integer();
float get_float();
command get_command();

// Later this will also hold code for drawing strings in the status bar at the
// bottom, and the message bar at the top

#endif
