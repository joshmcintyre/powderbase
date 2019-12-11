/* This file contains function definitions for a field class
*
* Author: Josh McIntyre
*/

#include "DB.h"

// This constructor sets information such as the size of the Field
DB::Field::Field()
{
	// Set the default size for the field
	this -> size = name.get_size() + sizeof(int);
}

// This constructor sets information such as the size of the Field
DB::Field::Field(unsigned int size)
{
	/* Set the default size for
	* This makes database files forward compatible with new features that require additional table information
	*/
	this -> size = size;
}

// This function writes field information to disk using a stream object
void DB::Field::write(std::fstream& stream)
{	
	// Write out the field properties
	stream.write(name.get().c_str(), name.get_size());
	stream.write(reinterpret_cast<const char*>(&type), sizeof(int));
}

// This function reads field information from disk using a stream object
void DB::Field::read(std::fstream& stream)
{	
	// Read the field properties
	name.read(stream);
	stream.read((char*)&type, sizeof(int));
}

// This setter sets the name
void DB::Field::set_name(FixedString8 name)
{
	this -> name = name;
}

/* This setter sets the type
* If the passed in type is not a valid type, set a default type
*/
void DB::Field::set_type(int type)
{
	if (type == ATTR_ID || type == ATTR_INT || type == ATTR_FLOAT || type == ATTR_CHAR16)
	{
		this -> type = type;
	}
	else
	{
		this -> type = ATTR_INT;
	}
}

// This getter gets the field size
unsigned int DB::Field::get_size()
{
	return size;
}

// This getter gets the field name
DB::FixedString8 DB::Field::get_name()
{
	return name;
}

// This getter gets the field type
int DB::Field::get_type()
{
	return type;
}
