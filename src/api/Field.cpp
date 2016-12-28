/* This file contains function definitions for a field class
*
* Author: Josh McIntyre
*
*/

#include <DB.h>

/* This constructor sets information such as the size of the Field
*
*/
DB::Field::Field()
{
	/* Set the default size for the field
	*
	*/
	this -> size = NAME_SIZE + sizeof(int);
}

/* This constructor sets information such as the size of the Field
*
*/
DB::Field::Field(unsigned int size)
{
	/* Set the default size for
	* This makes database files forward compatible with new features that require additional table information
	*
	*/
	this -> size = size;
}

/* This function writes field information to disk using a stream object
*
* Argument: stream
*
*/
void DB::Field::write(std::fstream& stream)
{	
	/* Write out the field properties
	*
	*/
	stream.write(name.c_str(), NAME_SIZE);
	stream.write(reinterpret_cast<const char*>(&type), sizeof(int));
}

/* This function reads field information from disk using a stream object
*
* Argument: stream
*
*/
void DB::Field::read(std::fstream& stream)
{	
	/* Write out the field properties
	*
	*/
	name.resize(NAME_SIZE);
	stream.read(&name[0], NAME_SIZE);
	stream.read((char*)&type, sizeof(int));
}

/* This setter sets the name
* If the passed in name is longer than the name size limit, truncate it
* If it is shorter than the name size limit, pad the string
*
* Argument: name
*
*/
void DB::Field::set_name(std::string name)
{
	name = name.substr(0, NAME_SIZE);
	
	if (name.size() < NAME_SIZE)
	{
		int pad = NAME_SIZE - name.size();
		name.append(pad, ' ');
	}
	
	this -> name = name;
}

/* This setter sets the type
* If the passed in type is not a valid type, set a default type
*
* Argument: type
*
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

/* This getter gets the field size
*
* Return: size
*
*/
unsigned int DB::Field::get_size()
{
	return size;
}

/* This getter gets the field name
*
* Return: name
*
*/
std::string DB::Field::get_name()
{
	return name;
}

/* This getter gets the field type
*
* Return: name
*
*/
int DB::Field::get_type()
{
	return type;
}
