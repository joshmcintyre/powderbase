/* This file contains function definitions for the Record class
*
* Author: Josh McIntyre
*
*/

#include <DB.h>

/* This function writes record information to disk using a stream object
*
* Argument: stream
*
*/
void DB::Record::write(std::fstream& stream)
{	
	attr_id.write(stream);

	std::map<std::string, AttrInt>::const_iterator iti;
	for (iti = attr_ints.begin(); iti != attr_ints.end(); iti++)
	{
		AttrInt attr_int = iti -> second;
		attr_int.write(stream);
	}

	std::map<std::string, AttrFloat>::const_iterator itf;
	for (itf = attr_floats.begin(); itf != attr_floats.end(); itf++)
	{
		AttrFloat attr_float = itf -> second;
		attr_float.write(stream);
	}

	std::map<std::string, AttrChar16>::const_iterator itc;
	for (itc = attr_char16s.begin(); itc != attr_char16s.end(); itc++)
	{
		AttrChar16 attr_char16 = itc -> second;
		attr_char16.write(stream);
	}
}

/* This function reads in record information from disk using a stream object
*
* Argument: stream
*
*/
void DB::Record::read(std::fstream& stream)
{
	std::map<std::string, Field> fields = table.get_fields();
	const int NAME_SIZE = 8;
	const int CHAR_16_SIZE = 16;

	std::map<std::string, Field>::const_iterator it;
	for (it = fields.begin(); it != fields.end(); it++)
	{
		std::string name;
		name.resize(NAME_SIZE);
		stream.read(&name[0], NAME_SIZE);
		
		/* Determine how much to read based on the field type in the table,
		* read the data, and add the attributes to the record
		*
		*/
		int type = fields[name].get_type();

		if (type == -1)
		{
			AttrID attr_id;
			attr_id.read(stream);
			this -> attr_id = attr_id;
		}
		else if (type == 0) //AttrInt
		{
			AttrInt attr;
			attr.read(stream);
			attr.set_name(name);
			attr_ints[attr.get_name()] = attr;
		}
		else if (type == 1) //AttrFloat
		{
			AttrFloat attr;
			attr.read(stream);
			attr.set_name(name);
			attr_floats[attr.get_name()] = attr;
		}
		else if (type == 2) //AttrChar16
		{
			AttrChar16 attr;
			attr.read(stream);
			attr.set_name(name);
			attr_char16s[attr.get_name()] = attr;
		}
	}
}

/* This function stores a copy of the database table to verify additions to the record
*
* Argument: Table
*
*/
void DB::Record::set_table(Table table)
{
	this -> table = table;
}

/* This function sets the record id
*
* Argument: id
*
*/
void DB::Record::set_id(unsigned int id)
{
	AttrID attr_id;
	attr_id.set_data(id);
	this -> attr_id = attr_id;
}

/* This function adds an integer Attr to a record
*
* Argument: name
* Argument: data
*
*/
void DB::Record::add_int(std::string name, int data)
{
	if (! table.is_field(name))
		return;

	AttrInt attr;
	attr.set_name(name);
	attr.set_data(data);
	attr_ints[attr.get_name()] = attr;
}

/* This function adds a floating point Attr to a record
*
* Argument: name
* Argument: data
*
*/
void DB::Record::add_float(std::string name, float data)
{
	if (! table.is_field(name))
		return;

	AttrFloat attr;
	attr.set_name(name);
	attr.set_data(data);
	attr_floats[attr.get_name()] = attr;
}

/* This function adds a 16 character string Attr to a record
*
* Argument: name
* Argument: data
*
*/
void DB::Record::add_char16(std::string name, std::string data)
{
	if (! table.is_field(name))
		return;

	AttrChar16 attr;
	attr.set_name(name);
	attr.set_data(data);
	attr_char16s[attr.get_name()] = attr;
}

/* This function returns the record id
*
* Return: id
*
*/
unsigned int DB::Record::get_id()
{
	return attr_id.get_data();
}

/* This function returns an integer attribute
*
* Argument: name
*
*/
int DB::Record::get_int(std::string name)
{
	/* Standardize the AttrName before retrieval
	*
	*/
	Attr standardize;
	standardize.set_name(name);
	name = standardize.get_name();

	if (attr_ints.count(name) == 0)
		return 0;

	AttrInt attr = attr_ints[name];
	return attr.get_data();
}

/* This function returns a floating point attribute
*
* Argument: name
*
*/
float DB::Record::get_float(std::string name)
{
	/* Standardize the AttrName before retrieval
	*
	*/
	Attr standardize;
	standardize.set_name(name);
	name = standardize.get_name();

	if (attr_floats.count(name) == 0)
		return 0.0;

	AttrFloat attr = attr_floats[name];
	return attr.get_data();
}

/* This function returns a 16 character string attribute
*
* Argument: name
*
*/
std::string DB::Record::get_char16(std::string name)
{
	/* Standardize the AttrName before retrieval
	*
	*/
	Attr standardize;
	standardize.set_name(name);
	name = standardize.get_name();

	if (attr_char16s.count(name) == 0)
		return "";

	AttrChar16 attr = attr_char16s[name];
	return attr.get_data();
}

/* This function calculates the size of a whole record
*
* Return: size
*
*/
int DB::Record::get_size()
{	
	int size = 0;

	size += attr_id.get_size();
	size += attr_id.get_name_size();

	std::map<std::string, AttrInt>::const_iterator iti;
	for (iti = attr_ints.begin(); iti != attr_ints.end(); iti++)
	{
		AttrInt attr_int = iti -> second;
		size += attr_int.get_size();
		size += attr_id.get_name_size();
	}

	std::map<std::string, AttrFloat>::const_iterator itf;
	for (itf = attr_floats.begin(); itf != attr_floats.end(); itf++)
	{
		AttrFloat attr_float = itf -> second;
		size += attr_float.get_size();
		size += attr_id.get_name_size();
	}

	std::map<std::string, AttrChar16>::const_iterator itc;
	for (itc = attr_char16s.begin(); itc != attr_char16s.end(); itc++)
	{
		AttrChar16 attr_char16 = itc -> second;
		size += attr_char16.get_size();
		size += attr_id.get_name_size();
	}

	return size;
}

/* This function ensures that any table fields not added to the record are set with
* sane defaults before write operations so that we don't end up with an incorrectly
* sized record written to disk
*
*/
void DB::Record::sanitize()
{
	std::map<std::string, Field> fields = table.get_fields();

	std::map<std::string, Field>::const_iterator it;
	for (it = fields.begin(); it != fields.end(); it++)
	{
		Field field = it -> second;

		if (field.get_type() == ATTR_INT)
		{
			if (attr_ints.count(field.get_name()) == 0)
			{
				AttrInt attr_int;
				attr_int.set_name(field.get_name());
				attr_int.set_data(0);
				attr_ints[field.get_name()] = attr_int;
			}
		}
		else if (field.get_type() == ATTR_FLOAT)
		{
			if (attr_floats.count(field.get_name()) == 0)
			{
				AttrFloat attr_float;
				attr_float.set_name(field.get_name());
				attr_float.set_data(0.0);
				attr_floats[field.get_name()] = attr_float;
			}
		}
		else if (field.get_type() == ATTR_CHAR16)
		{
			if (attr_char16s.count(field.get_name()) == 0)
			{
				AttrChar16 attr_char16;
				attr_char16.set_name(field.get_name());
				attr_char16.set_data("");
				attr_char16s[field.get_name()] = attr_char16;
			}
		}
	}
}
