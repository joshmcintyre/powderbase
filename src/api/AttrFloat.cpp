/* This file contains function definitions for the AttrFloat class
*
* Author: Josh McIntyre
*
*/

#include <DB.h>

/* This constructor sets information such as the size of the AttrFloat data
*
*/
DB::AttrFloat::AttrFloat()
{
	this -> size = sizeof(float);
}


/* This function writes Attr information to disk using a stream object
*
* Argument: stream
*
*/
void DB::AttrFloat::write(std::fstream& stream)
{	
	/* Write out the Attr properties
	*
	*/
	stream.write(name.c_str(), NAME_SIZE);
	stream.write(reinterpret_cast<const char*>(&data), size);
}

/* This function reads Attr information from disk using a stream object
*
* Argument: stream
*
*/
void DB::AttrFloat::read(std::fstream& stream)
{	
	/* Read the Attr properties from disk
	*
	*/
	float data;
	stream.read((char*)&data, sizeof(float));
	this -> data = data;
}

/* This setter sets the AttrFloat data
*
* Argument: data
*
*/
void DB::AttrFloat::set_data(const float data)
{
	this -> data = data;
}

/* This getter returns the AttrFloat data
*
* Argument: data
*
*/
float DB::AttrFloat::get_data()
{
	return data;
}

/* This getter returns the AttrFloat data size
*
* Argument: size
*
*/
unsigned int DB::AttrFloat::get_size()
{
	return size;
}
