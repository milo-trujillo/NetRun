#ifndef ENTITY_H
#define ENTITY_H

#include "io.h"

#ifndef NULL
	#define NULL 0
#endif

// This file is to describe living entities, a base class used by the player
// and monsters alike.

// These are for keeping track of the number of entities we're working with
void increase_entity_count();
void decrease_entity_count();
int get_entity_count();

// Maybe later I'll add more types like a familiar. Who knows.
enum entity_type { PLAYER, MONSTER };

// Note: This class must be inherited from, and cannot be instantiated
class entity
{
	public:
		entity()
		{
			increase_entity_count();
		}
		~entity()
		{
			decrease_entity_count();
			if( pLast != NULL ) // Hopefully that never happens
				pLast->set_next( pNext );
			if( pNext != NULL )
				pNext->set_last( pLast );
		}
		void draw()
		{
			display( x, y, symbol );
		}
		int get_x()
		{
			return x;
		}
		int get_y()
		{
			return y;
		}
		int get_hp()
		{
			return hp;
		}
		int get_max_hp()
		{
			return max_hp;
		}
		entity_type get_type()
		{
			return type;
		}
		virtual void hurt( int damage )
		{
			hp -= damage;
			if( hp <= 0 )
				kill();
		}
		virtual void heal( int ammount )
		{
			hp += ammount;
			if( hp > max_hp )
				hp = max_hp;
		}
		void set_position( int newx, int newy )
		{
			x = newx;
			y = newy;
		}
		// Linked list management
		entity* get_next()
		{
			return pNext;
		}
		void set_next( entity* next )
		{
			pNext = next;
		}
		entity* get_last()
		{
			return pLast;
		}
		void set_last( entity* last )
		{
			pLast = last;
		}
		// This is the function that manages AI in monsters
		// It also does input for players
		virtual void run() = 0;
		friend bool save_entity( entity* );
	protected:
		// Variables
		int x;
		int y;
		char symbol;
		int hp;
		int damage; // How much can you hit?
			// Later we'll probably want a base_damage, and offset
			// based on the equipped weapon and stats to get damage
		bool is_visible;
		// Variables only set by constructor
		int max_hp;
		entity_type type;
		entity* pNext; // Used for a linked list of entities
		entity* pLast; // Used for a linked list of entities
		// Methods
		virtual void kill() = 0;
};

void run_entities( entity* pEntity );
void draw_entities( entity* pEntity );
entity* get_entity_at( int x, int y );

#endif
