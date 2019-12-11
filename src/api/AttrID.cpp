/* This file contains function definitions for the AttrID class
*
* Author: Josh McIntyre
*/

#include "DB.h"

/* This constructor sets information such as the size of the AttrID data
* All id's will have the name "id"
* A default of 0 will be used by the update function to determine if a record has an id set
* before updating
*/
DB::AttrID::AttrID()
{
	set_name(FixedString8("id"));
	data = 0;
	size = sizeof(unsigned int);
}

// This function writes Attr information to disk using a stream object
void DB::AttrID::write(std::fstream& stream)
{	
	// Write out the Attr properties
	stream.write(name.get().c_str(), name.get_size());
	stream.write(reinterpret_cast<const char*>(&data), size);
}

// This function reads Attr information from disk using a stream object
void DB::AttrID::read(std::fstream& stream)
{	
	// Read the Attr properties from disk
	unsigned int data;
	stream.read((char*)&data, sizeof(unsigned int));
	this -> data = data;
}

// This setter sets the AttrID data
void DB::AttrID::set_data(const unsigned int data)
{
	this -> data = data;
}

// This getter returns the AttrID data
unsigned int DB::AttrID::get_data()
{
	return data;
}

// This getter returns the AttrID data size
unsigned int DB::AttrID::get_size()
{
	return size;
}
