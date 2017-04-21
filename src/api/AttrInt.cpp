/* This file contains function definitions for the AttrInt class
*
* Author: Josh McIntyre
*
*/

#include <DB.h>

/* This constructor sets information such as the size of the AttrInt data
*
*/
DB::AttrInt::AttrInt()
{
	this -> size = sizeof(int);
}

/* This function writes Attr information to disk using a stream object
*
* Argument: stream
*
*/
void DB::AttrInt::write(std::fstream& stream)
{	
	/* Write out the Attr properties
	*
	*/
	stream.write(name.get().c_str(), name.get_size());
	stream.write(reinterpret_cast<const char*>(&data), size);
}

/* This function reads Attr information from disk using a stream object
*
* Argument: stream
*
*/
void DB::AttrInt::read(std::fstream& stream)
{	
	/* Read the Attr properties from disk
	*
	*/
	int data;
	stream.read((char*)&data, sizeof(int));
	this -> data = data;
}

/* This setter sets the AttrInt data
*
* Argument: data
*
*/
void DB::AttrInt::set_data(const int data)
{
	this -> data = data;
}

/* This getter returns the AttrInt data
*
* Argument: data
*
*/
int DB::AttrInt::get_data()
{
	return data;
}

/* This getter returns the AttrInt data size
*
* Argument: size
*
*/
unsigned int DB::AttrInt::get_size()
{
	return size;
}
