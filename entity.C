#include "entity.h"
#include "main.h" // For exporting the player to recursive functions

#ifndef NULL
#define NULL 0
#endif

int entity_count = 0;

//
// increase / decrease_entity_count() - maintain a counter of the entity length
//
//  We just keep a global int and control it from here.
//
void increase_entity_count()
{
	entity_count++;
}

void decrease_entity_count()
{
	entity_count--;
}

int get_entity_count()
{
	return entity_count;
}

//
// run_entities() - Recursively tell every entity to run
//
// We utilize the doubly linked list built into entities to run everything.
//
void run_entities( entity* pEntity )
{
	pEntity->run();
	if( pEntity->get_next() != NULL )
		run_entities( pEntity->get_next() );
}

//
// draw_entities() - Recursively tell every entity to draw
//
// We utilize the doubly linked list built into entities to draw everything.
//
void draw_entities( entity* pEntity )
{
	pEntity->draw();
	if( pEntity->get_next() != NULL )
		draw_entities( pEntity->get_next() );
}

//
// get_entity_at() - Recursively check for a creature at a coordinate
//
// We take the player (the head of the entity linked list), and recursively
// walk the list, checking coordinates until we reach the end, or we find
// something at the desired coordinate, which we return.
//
entity* get_entity_at( int x, int y, entity* creature )
{
	if( creature->get_x() == x && creature->get_y() == y )
		return creature;
	else if( creature->get_next() != NULL )
		return get_entity_at( x, y, creature->get_next() );
	else
		return NULL;
}

//
// get_entity_at() - A wrapper for the _real_ get_entity_at()
// 
// We use this function to hide implimentation and make sure 'main.h' doesn't
// start getting included everywhere.
//
entity* get_entity_at( int x, int y )
{
	return get_entity_at( x, y, get_head_entity() );
}


