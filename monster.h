#ifndef MONSTER_H
#define MONSTER_H

#include "entity.h"
#include "config.h" // Need strings

class monster : public entity
{
	public:
		monster( entity*, int ); // Constructor, has a pointer to parent
		virtual void run() = 0; // AI for monster
		virtual void multiply() = 0; // Make a new monster after this
		virtual void hurt( int damage );
		void save(string* save_data);
	protected:
		virtual void kill();
		char* name;		// Type of monster
};

void make_monsters( int number, entity* last );
void multiply_monsters( monster* pMonster, int count );
monster* get_first_monster();
string save_monsters();
int get_monsters_created();

#endif
