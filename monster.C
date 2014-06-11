#include <stdio.h> // For sprintf

#include "rand.h"
#include "entity.h"
#include "monster.h"
#include "map.h"
#include "io.h"

#include "main.h" // This is exclusively for get_head_entity!

#ifndef NULL
#define NULL 0
#endif

//
// ================
// GLOBAL VARIABLES
// ================
//

// This keeps track of how many monsters have ever been created, even if dead
int monsters_created = 0;

// Need this up here so methods can use it
void make_monster( entity* last );
void multiply_monsters( monster* pMonster );
void insert_monster( entity* previous, entity* next );

//
// =====================
// MONSTER CLASS METHODS
// =====================
//

// We set a constructor so I don't have to repeat this code for new inherited
// classes (individual monster types)
monster::monster(entity* last, int health) : entity()
{
	hp = health;
	max_hp = health;
	type = MONSTER;
	is_visible = true;
	get_open_space( &x, &y );
	pNext = NULL;
	pLast = last;
	monsters_created++;
}

void monster::hurt( int damage )
{
	hp -= damage;
	if( hp <= 0 )
		kill();
	else
	{
		char death[50];
		for( int i = 0; i < 50; i++ )
			death[i] = ' ';
		sprintf(death, "You hit the %s!", name);
		display_message(death);
	}
}

void monster::kill()
{
	char death[50];
	for( int i = 0; i < 50; i++ )
		death[i] = ' ';
	sprintf(death, "You kill the %s!", name);
	display_message(death);
	this->~monster(); // When a monster dies, free the memory
}

void monster::save(string* save_data)
{
	if( pNext != NULL )
	{
		monster* pNext_Monster = dynamic_cast< monster* >( pNext );
		pNext_Monster->save(save_data);
	}
	save_data->append(name);
	save_data->append("\n");
	save_data->append(int_to_string(x));
	save_data->append("\n");
	save_data->append(int_to_string(y));
	save_data->append("\n");
	save_data->append(int_to_string(hp));
	save_data->append("\n");
	if( is_visible == true )
		save_data->append("VISIBLE\n");
	else
		save_data->append("INVISIBLE\n");
	save_data->append("!\n");
	return;
}

//
// ==========================
// INDIVIDUAL MONSTER CLASSES
// ==========================
//

class bug : public monster
{
	public:
		bug(entity* last) : monster( last, 10 )
		{
			symbol = 'x';
			name = const_cast<char*>("bug");
		}
		virtual void multiply()
		{
			//make_monster( this );
			insert_monster( this, pNext );
		}
		// A quick hack to make things wiggle.
		virtual void run()
		{
			int newx, newy;
			int foo = random( 1, 8 );
			switch( foo )
			{
				case 1:
					newx = x - 1;
					newy = y - 1;
					break;
				case 2:
					newx = x;
					newy = y - 1;
					break;
				case 3:
					newx = x + 1;
					newy = y - 1;
					break;
				case 4:
					newx = x - 1;
					newy = y;
					break;
				case 5:
					newx = x + 1;
					newy = y;
					break;
				case 6:
					newx = x - 1;
					newy = y + 1;
					break;
				case 7:
					newx = x;
					newy = y + 1;
					break;
				case 8:
					newx = x + 1;
					newy = y + 1;
					break;
			}
					
			if( get_entity_at( newx, newy ) == NULL )
				interact( newx, newy, this );
			return;
		}
};

//
// =======================
// FUNCTIONS (NOT METHODS)
// =======================
//

//
// make_monster() - Adds a new monster to the end of the list
//
// Later this code will determine what types of monsters should be created,
// based on dungeon level and probability. For now it just hard codes in a bug.
//
void make_monster( entity* last )
{
	//last->set_next( new bug( last ) );
	insert_monster( last, NULL );
}

//
// insert_monster() - Adds a new monster to the middle of the list
//
// We shuffle about the pointers to make room for a new entity, then sticks one in.
//
void insert_monster( entity* previous, entity* next )
{
	entity* pMonster = new bug( previous );
	if( next != NULL )
		next->set_last( pMonster );
	previous->set_next( pMonster );
	pMonster->set_next( next );
	pMonster->set_last( previous );
}

//
// make_monsters() - Recursively add monsters to the end of the list
//
// We use a count provided to us to know how many monsters to add. Not much
// to say.
//
void make_monsters( int count, entity* creature )
{
	if( count == 0 )
		return;
	else
	{
		make_monster( creature );
		make_monsters( count - 1, creature->get_next() );
	}
}

//
// multiply_monsters()
//
// We jump to the end of the list, and in reverse add more nodes.
// 'count' specifies how many to add. If we are asked to create more monsters
// than the length of the list we'll wrap around and add more from the beginning
//
void multiply_monsters( monster* pMonster, int count )
{
	if( pMonster->get_next() != NULL && count > 0 )
		multiply_monsters( dynamic_cast< monster* >(pMonster->get_next() ), count - 1 );
	else if( count > 0 )
		multiply_monsters( get_first_monster(), count - 1 );
	pMonster->multiply();
}

//
// save_monsters() - Recursively have monsters save themselves to disk
//
// We pass around a file handle through the monsters, and they sort themselves out.
//
string save_monsters()
{
	string save_data = "MONSTERS\n";
	monster* pMonster = get_first_monster();
	pMonster->save(&save_data);
	return save_data;
}

//
// get_first_monster()
//
// We use this function to get the first monster (as apposed to head of list)
// This allows the rest of the code to be oblivious to list order
//
monster* get_first_monster()
{
	entity* pEntity = get_head_entity();
	while( pEntity->get_type() != MONSTER )
	{
		pEntity = pEntity->get_next();
	}
	return dynamic_cast<monster*>( pEntity );
}

//
// get_monsters_created()
//
// Tells how many monsters have been created.
//
int get_monsters_created()
{
	return monsters_created;
}
