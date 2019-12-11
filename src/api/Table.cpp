  /* This file contains function definitions for a Table class
* The Table allows the user to add fields to the table
* It creates and stores Field objects
*
* Author: Josh McIntyre
*/

#include "DB.h"

// This function writes table information to disk using a stream object
void DB::Table::write(std::fstream& stream)
{	
	// Make sure the table isn't empty before writing
	if (fields.empty())
		return;

	// Make sure to add the required id field to the table
	Field id;
	id.set_name(FixedString8("id"));
	id.set_type(ATTR_ID);
	
	fields[id.get_name().get()] = id;
	
	// Write out the size of each field and the number of table fields
	DB::Field size;
	int num_fields = fields.size();
	unsigned int field_size = size.get_size();
	stream.write(reinterpret_cast<const char*>(&num_fields), sizeof(int));
	stream.write(reinterpret_cast<const char*>(&field_size), sizeof(unsigned int));

	// Write out the individual tables
	std::map<std::string, Field>::const_iterator it;
	for (it = fields.begin(); it != fields.end(); it++)
	{
		Field field = it -> second;
		field.write(stream);
	}
}

// This function reads table information from disk using a stream object
void DB::Table::read(std::fstream& stream)
{	
	// Read in the field size and number of fields from disk
	int num_fields;
	unsigned int field_size;
	stream.read((char*)&num_fields, sizeof(int));
	stream.read((char*)&field_size, sizeof(unsigned int));

	for (int i = 0; i < num_fields; i++)
	{
		Field field(field_size);
		field.read(stream);
		fields[field.get_name().get()] = field;
	}
}

// This function adds a field to the Table
void DB::Table::add_field(std::string name, int type)
{
	Field new_field;
	new_field.set_name(name);
	new_field.set_type(type);
	
	fields[new_field.get_name().get()] = new_field;
}

// This function returns a copy of the table fields
std::map<std::string, DB::Field> DB::Table::get_fields()
{
	return fields;
}

// This function returns whether or not a field is in the table
bool DB::Table::is_field(std::string name)
{
	// Normalize the field name before searching
	FixedString8 fixed_name(name);
	name = fixed_name.get();
	
	if (fields.count(name) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

