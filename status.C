#include <stdio.h> // For sprintf

#include "status.h"
#include "io.h"
#include "main.h"
#include "config.h"

void print_turn()
{
	int turn = get_turn();
	char string[BOARD_WIDTH];
	sprintf(string, "Turn: %d", turn);
	display_status(2, string);
}

void print_calculus(int population, int max, float rate, float A)
{
	int turn = get_turn();
	char string[BOARD_WIDTH];
	//sprintf(string, "Pf=Pi(1 + R)^t  =>  %d=%d(1 + %d)^%d", final, initial, rate, turn / 30);

	sprintf(string, "P = M/(1 + Ae^(-kt))  =>  %d = %d/(1 + %de^(-1*%f*%d))", population, max, int(A), rate, turn / 30);
	display_status(1, string);
}
