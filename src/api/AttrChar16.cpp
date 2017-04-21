/* This file contains function definitions for the AttrChar16 class
*
* Author: Josh McIntyre
*
*/

#include <DB.h>

/* This constructor sets information such as the size of the AttrChar data
*
*/
DB::AttrChar16::AttrChar16()
{
	this -> size = CHAR_16_SIZE;
}


/* This function writes Attr information to disk using a stream object
*
* Argument: stream
*
*/
void DB::AttrChar16::write(std::fstream& stream)
{	
	/* Write out the Attr properties
	*
	*/
	stream.write(name.get().c_str(), name.get_size());
	stream.write(data.c_str(), size);
}

/* This function reads Attr information from disk using a stream object
*
* Argument: stream
*
*/
void DB::AttrChar16::read(std::fstream& stream)
{	
	/* Read the Attr properties from disk
	*
	*/
	std::string data;
	data.resize(CHAR_16_SIZE);
	stream.read(&data[0], CHAR_16_SIZE);
	this -> data = data;
}

/* This setter sets the AttrChar16 data
*
* Argument: data
*
*/
void DB::AttrChar16::set_data(std::string data)
{
	data = data.substr(0, CHAR_16_SIZE);
	
	if (data.size() < CHAR_16_SIZE)
	{
		int pad = CHAR_16_SIZE - data.size();
		data.append(pad, ' ');
	}
	
	this -> data = data;
}

/* This getter returns the AttrChar16 data
*
* Argument: data
*
*/
std::string DB::AttrChar16::get_data()
{
	return data;
}

/* This getter returns the AttrChar16 data size
*
* Argument: size
*
*/
unsigned int DB::AttrChar16::get_size()
{
	return size;
}
