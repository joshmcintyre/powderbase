/* This file contains function definitions for Attr class
*
* Author: Josh McIntyre
*/

#include <DB.h>

// This setter sets the name
void DB::Attr::set_name(FixedString8 name)
{
	this -> name = name;
}

// This getter returns the Attr name
DB::FixedString8 DB::Attr::get_name()
{
	return name.get();
}

