#include "curses.h"
#include "config.h"
#include "io.h"

//
// init_display() - Readies IO to display things on screen
//
// We prime ncurses, and turn on settings like noecho to make displays pretty
//
void init_display()
{
	initscr();
	cbreak();
	noecho();
	refresh();
}

//
// end_display() - Turn off display, restore old terminal settings
//
// At the moment, ncurses handles the shutdown for us.
//
void end_display()
{
	endwin();
}

//
// display() - Print a character somewhere on the screen
//
// We print a symbol to an X/Y coordinate, after offsetting so it ends up in the
// right area of the screen.
//
void display( int x, int y, char symbol )
{
	mvaddch( y + BOARD_Y, x + BOARD_X, symbol );
}

//
// display_message() - Prints a message string to the top of the screen
//
// We use a 'printf' like function to dump a string on the message bar at the
// top of the screen.
//
void display_message( const char* message )
{
	clear_messages(); // Don't leave ghost text up there
	mvprintw( MESSAGE_Y, MESSAGE_X, message );
}

//
// clear_messages() - Wipe the message bar at screen top
//
// We wipe the messages using nasty ncurses black magic and fiddling with the
// cursor. Seems to work though, and no nasty buffer overflows like we were 
// getting by writing an array of ' ' to the message bar...
//
void clear_messages()
{
	int y, x;
	getyx(stdscr, y, x); // Black magic curses
	move( MESSAGE_Y, MESSAGE_X );
	clrtoeol();
	move( y, x ); // Move the cursor back

	/*
	char message[BOARD_WIDTH];
	for( int x = 0; x < BOARD_WIDTH - 1; x++ )
		message[x] = ' ';
	mvprintw( MESSAGE_Y, MESSAGE_X, message );
	*/
}

//
// display_status() - Prints a line to the status bar at bottom screen
//
// We print a line to either line 1 or 2 of the status bar on the bottom screen.
//
void display_status( int line, const char* message )
{
	mvprintw( STATUS_Y + line - 1, STATUS_X, message );
}

//
// refresh_screen() - Updates any animation and drawing from buffer to screen
//
// At the moment, ncurses covers this in a simple way for us.
//
void refresh_screen()
{
	refresh();
}

//
// get_command() - Takes input from user, converts it to commands
//
// This reads keypresses from the user via ncurses, then converts it to
// commands, which are passed off to game logic.
//
command get_command()
{
	bool done;
	command choice;
	do
	{
		done = true;
		char input = getch();
		switch(input)
		{
			case 'h':
				choice = WEST;
				break;
			case 'j':
				choice = SOUTH;
				break;
			case 'k':
				choice = NORTH;
				break;
			case 'l':
				choice = EAST;
				break;
			case 'y':
				choice = NW;
				break;
			case 'u':
				choice = NE;
				break;
			case 'b':
				choice = SW;
				break;
			case 'n':
				choice = SE;
				break;
			case 'Q':
				choice = QUIT;
				break;
			case 'S':
				choice = SAVE;
				break;
			default:
				done = false;
				break;
		}
	}
	while( done == false );
	return choice;
}

//
// get_integer() - Reads an integer from keyboard
//
// We have to use sketch-ass curses to read a C-string, then convert to an int
// with atoi. So it goes.
//
int get_integer()
{
	char input[10];
	getstr(input);
	return atoi(input);
}

//
// get_float() - Reads an float from keyboard
//
// We have to use sketch-ass curses to read a C-string, then convert to an float
// with atof. So it goes.
//
float get_float()
{
	char input[10];
	getstr(input);
	return atof(input);
}


//
// select() - Moves the cursor over a given square
//
// We use an ncurses call, with offsets, to select any square on the map
//
void select( int x, int y )
{
	move( y + BOARD_Y, x + BOARD_X );
}

//
// clear_screen() - Wipes the screen, for debugging or changing levels
//
// Ncurses call clears the screen
//
void clear_screen()
{
	clear();
}
