/* This file contains function definitions for the AttrChar16 class
*
* Author: Josh McIntyre
*/

#include "DB.h"

// This constructor sets information such as the size of the AttrChar data
DB::AttrChar16::AttrChar16()
{
	this -> size = data.get_size();
}


// This function writes Attr information to disk using a stream object
void DB::AttrChar16::write(std::fstream& stream)
{	
	/* Write out the Attr properties
	*
	*/
	stream.write(name.get().c_str(), name.get_size());
	stream.write(data.get().c_str(), size);
}

// This function reads Attr information from disk using a stream object
void DB::AttrChar16::read(std::fstream& stream)
{	
	// Read the Attr properties from disk
	data.read(stream);
}

// This setter sets the AttrChar16 data
void DB::AttrChar16::set_data(std::string data)
{
	FixedString16 fixed_data(data);	
	this -> data = fixed_data;
}

// This getter returns the AttrChar16 data
std::string DB::AttrChar16::get_data()
{
	return data.get();;
}

// This getter returns the AttrChar16 data size
unsigned int DB::AttrChar16::get_size()
{
	return size;
}
