/* This file contains function definitions for the PowderBase API 
*
* Author: Josh McIntyre
*
*/

#include <DB.h>

/* This constructor initializes metadata defaults when the database object is created
*
*/
DB::DB()
{
	is_loaded = false;
}

/* This API function creates the database file given a new table
*
* Argument: db_name
* Argument: table
*
*/
void DB::create(std::string db_name, DB::Table table)
{
	/* Open a stream with the database file
	*
	*/
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::fstream::out | std::ios::binary);
	
	/* Ensure we are at the beginning of the file to avoid any data corruption
	*
	*/
	if (db_file.tellg() != 0)
		return;
	
	/* Write the table to the database file
	*
	*/
	table.write(db_file);
	
	/* Initialize the record count and write it to file
	*
	*/
	record_count = 0;
	db_file.write(reinterpret_cast<const char*>(&record_count), sizeof(unsigned int));
		
	db_file.close();
	
	/* Set this database as loaded so record operations can be performed and store important DB metadata
	*
	*/
	this -> is_loaded = true;
	this -> db_name = db_name;
	this -> table = table;
}

/* This API function loads the database table in to memory given the database name
*
* Argument: db_name
*
*/
void DB::load(std::string db_name)
{
	
	/* Open a stream with the database file
	*
	*/
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::fstream::in | std::ios::binary);
	
	/* Ensure we are at the beginning of the file to avoid any data corruption 
	*
	*/
	if (db_file.tellg() != 0)
		return;
	
	/* Load the database table
	*
	*/
	table.read(db_file);
	
	/* Load the database record count
	*
	*/
	db_file.read((char*)&record_count, sizeof(unsigned int));
	
	db_file.close();
	
	/* Set this database as loaded so record operations can be performed and store important DB metadata
	*
	*/
	this -> is_loaded = true;
	this -> db_name = db_name;
}

/* This API function inserts a new record in the database
*
* Argument: record
*
*/
void DB::insert(DB::Record record)
{
	/* Open a stream with the database file
	*
	*/
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	/* Ensure we are at the beginning of the file to avoid any data corruption
	*
	*/
	if (db_file.tellp() != 0)
		return;

	/* Read in the field size and number of fields from disk
	*
	*/
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));

	/* Sanitize the record before writing any information to the database
	*
	*/
	record.sanitize();

	/* Calculate where to overwrite the record count calculating the table size
	* As well as the size of the record metadata
	* Increment the record count and overwrite the existing count
	*
	*/
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	db_file.seekp(table_offset);

	record_count++;
	record.set_id(record_count);
	int record_size;
	record_size = record.get_size();
	db_file.write(reinterpret_cast<const char*>(&record_count), sizeof(unsigned int));
	db_file.write(reinterpret_cast<const char*>(&record_size), sizeof(int));

	/* Reopen the file at the end to append the new record
	* Write the record information
	*
	*/
	db_file.close();
	db_file.open(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::ate | std::ios::binary);
	record.write(db_file);
	
	db_file.close();

}

/* This API function updates a record in the database
*
* Argument: record
*
*/
void DB::update(DB::Record record)
{
	/* If the provided record doesn't have a valid id, don't perform any update operations
	*
	*/
	if (record.get_id() == 0)
		return;

	if (record.get_id() > record_count)
		return;
	
	/* Open a stream with the database file
	*
	*/
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	/* Ensure we are at the beginning of the file to avoid any data corruption
	*
	*/
	if (db_file.tellp() != 0)
		return;

	/* Read in the field size and number of fields from disk
	*
	*/
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));

	/* Sanitize the record before writing any information to the database
	*
	*/
	record.sanitize();

	/* Calculate where to overwrite the existing record
	*
	*/
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	int record_offset = table_offset + sizeof(unsigned int) + sizeof(int) + (record.get_size() * (record.get_id() - 1));
	db_file.seekp(record_offset);

	/* Overwrite the existing record with the new information
	*
	*/
	record.write(db_file);
	
	db_file.close();

}

/* This function allows the user to search the database for a record
* based on the value in a particular integer field
*
* Argument: name
* Argument: value
* Return: records
*
*/
std::vector<DB::Record> DB::search_int(std::string name, int value)
{
	/* Create a vector of records to return
	*
	*/
	std::vector<Record> records;
	
	/* If the provided field isn't in the list of int fields, don't search
	* We first need to create a field object and normalize the field name for searching
	*
	*/
	std::map<std::string, Field> fields = table.get_fields();
	Field temp_field;
	temp_field.set_name(name);
	if (fields[temp_field.get_name()].get_type() != ATTR_INT)
		return records;

	/* Open a stream with the database file
	*
	*/
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	/* Ensure we are at the beginning of the file to avoid any data corruption
	*
	*/
	if (db_file.tellp() != 0)
		return records;
	
	/* Read in the field size and number of fields from disk
	*
	*/
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));


	/* Calculate where to start the record search by using the table offset and
	* retrieving the number of records
	*
	*/
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	db_file.seekp(table_offset);
	
	unsigned int record_count;
	int record_size;
	db_file.read((char*)&record_count, sizeof(unsigned int));
	db_file.read((char*)&field_size, sizeof(int));
	
	/* Search records by reading in records one by one (linear search)
	* and checking if the value in the desired field matches the desired value
	*
	*/
	for (unsigned int i = 1; i <= record_count; i++)
	{
		/* Read in the record
		*
		*/
		Record temp_record;
		temp_record.set_table(table);
		temp_record.read(db_file);

		/* Check for a matching value
		*
		*/
		if (temp_record.get_int(temp_field.get_name()) == value)
			records.push_back(temp_record);
	}
	
	return records;
}

/* This function allows the user to search the database for a record
* based on the value in a particular floating point field
*
* Argument: name
* Argument: value
* Return: records
*
*/
std::vector<DB::Record> DB::search_float(std::string name, float value)
{
	/* Create a vector of records to return
	*
	*/
	std::vector<Record> records;
	
	/* If the provided field isn't in the list of float fields, don't search
	* We first need to create a field object and normalize the field name for searching
	*
	*/
	std::map<std::string, Field> fields = table.get_fields();
	Field temp_field;
	temp_field.set_name(name);
	if (fields[temp_field.get_name()].get_type() != ATTR_FLOAT)
		return records;

	/* Open a stream with the database file
	*
	*/
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	/* Ensure we are at the beginning of the file to avoid any data corruption
	*
	*/
	if (db_file.tellp() != 0)
		return records;
	
	/* Read in the field size and number of fields from disk
	*
	*/
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));


	/* Calculate where to start the record search by using the table offset and
	* retrieving the number of records
	*
	*/
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	db_file.seekp(table_offset);
	
	unsigned int record_count;
	int record_size;
	db_file.read((char*)&record_count, sizeof(unsigned int));
	db_file.read((char*)&field_size, sizeof(int));
	
	/* Search records by reading in records one by one (linear search)
	* and checking if the value in the desired field matches the desired value
	*
	*/
	for (unsigned int i = 1; i <= record_count; i++)
	{
		/* Read in the record
		*
		*/
		Record temp_record;
		temp_record.set_table(table);
		temp_record.read(db_file);

		/* Check for a matching value
		*
		*/
		if (temp_record.get_float(temp_field.get_name()) == value)
			records.push_back(temp_record);
	}
	
	return records;
}

/* This function allows the user to search the database for a record
* based on the value in a particular char16 field
*
* Argument: name
* Argument: value
* Return: records
*
*/
std::vector<DB::Record> DB::search_char16(std::string name, std::string value)
{
	/* Create a vector of records to return
	*
	*/
	std::vector<Record> records;
	
	/* If the provided field isn't in the list of char16 fields, don't search
	* We first need to create a field object and normalize the field name for searching
	*
	*/
	std::map<std::string, Field> fields = table.get_fields();
	Field temp_field;
	temp_field.set_name(name);
	if (fields[temp_field.get_name()].get_type() != ATTR_CHAR16)
		return records;

	/* Open a stream with the database file
	*
	*/
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	/* Ensure we are at the beginning of the file to avoid any data corruption
	*
	*/
	if (db_file.tellp() != 0)
		return records;
	
	/* Read in the field size and number of fields from disk
	*
	*/
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));


	/* Calculate where to start the record search by using the table offset and
	* retrieving the number of records
	*
	*/
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	db_file.seekp(table_offset);
	
	unsigned int record_count;
	int record_size;
	db_file.read((char*)&record_count, sizeof(unsigned int));
	db_file.read((char*)&field_size, sizeof(int));
	
	/* Search records by reading in records one by one (linear search)
	* and checking if the value in the desired field matches the desired value
	*
	*/
	for (unsigned int i = 1; i <= record_count; i++)
	{
		/* Read in the record
		*
		*/
		Record temp_record;
		temp_record.set_table(table);
		temp_record.read(db_file);
		AttrChar16 temp_value;
		temp_value.set_data(value);
		
		/* Check for a matching value
		*
		*/
		if (temp_record.get_char16(temp_field.get_name()) == temp_value.get_data())
			records.push_back(temp_record);
	}
	
	return records;
}

/* This method deletes a record in the database
* by id
*
* Argument: id
*
*/
void DB::remove(unsigned int id)
{	
	/* If the provided id isn't within a valid range, return
	*
	*
	*/
	if (id > record_count || id <= 0)
		return;
	
	/* Open a stream with the database file
	*
	*/
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	/* Ensure we are at the beginning of the file to avoid any data corruption
	*
	*/
	if (db_file.tellp() != 0)
		return;
	
	/* Read in the field size and number of fields from disk
	*
	*/
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));


	/* Calculate where to start the record reading by using the table offset and
	* retrieving the number of records
	*
	*/
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	db_file.seekp(table_offset);
	
	unsigned int record_count;
	int record_size;
	db_file.read((char*)&record_count, sizeof(unsigned int));
	db_file.read((char*)&record_size, sizeof(int));
	
	/* Read all existing records into a vector
	*
	*/
	std::vector<Record> records;
	for (unsigned int i = 1; i <= record_count; i++)
	{
		/* Read in the record
		*
		*/
		Record temp_record;
		temp_record.set_table(table);
		temp_record.read(db_file);
		
		records.push_back(temp_record);
	}

	/* Update the indexes after the element to delete
	* then remove the desired record from the vector
	* Finally, update the database record count
	*
	*/
	int vector_index = id - 1;
	for (int i = vector_index + 1; i < records.size(); i++)
	{
		unsigned int existing_id = records[i].get_id();
		records[i].set_id(existing_id - 1);
	}

	records.erase(records.begin() + vector_index);
	record_count -= 1;
	this -> record_count = record_count;

	db_file.close();

	/* Now reopen a stream with the database file and overwrite with the updated record set
	*
	*/
	db_file.open(db_filename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

	table.write(db_file);
	db_file.write(reinterpret_cast<const char*>(&record_count), sizeof(unsigned int));
	db_file.write(reinterpret_cast<const char*>(&record_size), sizeof(int));
	for (int i = 0; i < records.size(); i++)
	{
		records[i].write(db_file);
	}
	
	db_file.close();
}
