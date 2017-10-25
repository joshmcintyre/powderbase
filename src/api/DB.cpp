/* This file contains function definitions for the PowderBase API 
*
* Author: Josh McIntyre
*/

#include <DB.h>

// This constructor initializes metadata defaults when the database object is created
DB::DB()
{
	is_loaded = false;
}

// This API function creates the database file given a new table
void DB::create(std::string db_name, DB::Table table)
{
	// Open a stream with the database file
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::fstream::out | std::ios::binary);
	
	// Ensure we are at the beginning of the file to avoid any data corruption
	if (db_file.tellg() != 0)
		return;
	
	// Write the table to the database file
	table.write(db_file);
	
	// Initialize the record count and write it to file
	record_count = 0;
	removed_count = 0;
	db_file.write(reinterpret_cast<const char*>(&record_count), sizeof(unsigned int));
		
	db_file.close();
	
	// Set this database as loaded so record operations can be performed and store important DB metadata
	this -> is_loaded = true;
	this -> db_name = db_name;
	this -> table = table;
}

/* This API function loads the database table in to memory given the database name
*
* Argument: db_name
*/
void DB::load(std::string db_name)
{
	
	// Open a stream with the database file
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::fstream::in | std::ios::binary);
	
	// Ensure we are at the beginning of the file to avoid any data corruption 
	if (db_file.tellg() != 0)
		return;
	
	// Load the database table
	table.read(db_file);
	
	// Load the database record count
	db_file.read((char*)&record_count, sizeof(unsigned int));
	unsigned int record_size;
	db_file.read((char*)&record_size, sizeof(unsigned int));

	// Read through the records to determine the removed count
	removed_count = 0;
	for (unsigned int i = 1; i <= record_count; i++)
	{
		// Read in the record
		Record temp_record;
		temp_record.set_table(table);
		temp_record.read(db_file);

		// Check for a matching value
		if (temp_record.get_id() == 0)
			removed_count++;
	}

	// Set this database as loaded so record operations can be performed and store important DB metadata
	this -> is_loaded = true;
	this -> db_name = db_name;
	
	db_file.close();
}

// This API function inserts a new record in the database
void DB::insert(DB::Record record)
{
	// Open a stream with the database file
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	// Ensure we are at the beginning of the file to avoid any data corruption
	if (db_file.tellp() != 0)
		return;

	// Read in the field size and number of fields from disk
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));

	// Sanitize the record before writing any information to the database
	record.sanitize();

	/* Calculate where to overwrite the record count calculating the table size
	* As well as the size of the record metadata
	* Increment the record count and overwrite the existing count
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
	*/
	db_file.close();
	db_file.open(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::ate | std::ios::binary);
	record.write(db_file);
	
	db_file.close();

}

// This API function updates a record in the database
void DB::update(DB::Record record)
{
	// If the provided record doesn't have a valid id, don't perform any update operations
	if (record.get_id() == 0)
		return;

	if (record.get_id() > record_count)
		return;
	
	// Open a stream with the database file
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	// Ensure we are at the beginning of the file to avoid any data corruption
	if (db_file.tellp() != 0)
		return;

	// Read in the field size and number of fields from disk
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));

	// Sanitize the record before writing any information to the database
	record.sanitize();

	// Calculate where to overwrite the existing record
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	int record_offset = table_offset + sizeof(unsigned int) + sizeof(int) + (record.get_size() * (record.get_id() - 1));
	db_file.seekp(record_offset);

	// Overwrite the existing record with the new information
	record.write(db_file);
	
	db_file.close();

}

/* This function allows the user to search the database for a record
* based on the value in a particular integer field
*/
std::vector<DB::Record> DB::search_int(std::string name, int value)
{
	// Create a vector of records to return
	std::vector<Record> records;
	
	// If the provided field isn't in the list of int fields, don't search
	std::map<std::string, Field> fields = table.get_fields();
	FixedString8 fixed_name(name);
	if (fields[fixed_name.get()].get_type() != ATTR_INT)
		return records;

	// Open a stream with the database file
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	// Ensure we are at the beginning of the file to avoid any data corruption
	if (db_file.tellp() != 0)
		return records;
	
	// Read in the field size and number of fields from disk
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));


	/* Calculate where to start the record search by using the table offset and
	* retrieving the number of records
	*/
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	db_file.seekp(table_offset);
	
	unsigned int record_count;
	int record_size;
	db_file.read((char*)&record_count, sizeof(unsigned int));
	db_file.read((char*)&field_size, sizeof(int));
	
	/* Search records by reading in records one by one (linear search)
	* and checking if the value in the desired field matches the desired value
	*/
	for (unsigned int i = 1; i <= record_count; i++)
	{
		// Read in the record
		Record temp_record;
		temp_record.set_table(table);
		temp_record.read(db_file);

		// Check for a matching value
		if (temp_record.get_int(fixed_name.get()) == value && temp_record.get_id() != 0)
			records.push_back(temp_record);
	}
	
	return records;
}

/* This function allows the user to search the database for a record
* based on the value in a particular floating point field
*/
std::vector<DB::Record> DB::search_float(std::string name, float value)
{
	// Create a vector of records to return
	std::vector<Record> records;
	
	// If the provided field isn't in the list of float fields, don't search
	std::map<std::string, Field> fields = table.get_fields();
	FixedString8 fixed_name(name);
	if (fields[fixed_name.get()].get_type() != ATTR_FLOAT)
		return records;

	// Open a stream with the database file
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	// Ensure we are at the beginning of the file to avoid any data corruption
	if (db_file.tellp() != 0)
		return records;
	
	// Read in the field size and number of fields from disk
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));


	/* Calculate where to start the record search by using the table offset and
	* retrieving the number of records
	*/
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	db_file.seekp(table_offset);
	
	unsigned int record_count;
	int record_size;
	db_file.read((char*)&record_count, sizeof(unsigned int));
	db_file.read((char*)&field_size, sizeof(int));
	
	/* Search records by reading in records one by one (linear search)
	* and checking if the value in the desired field matches the desired value
	*/
	for (unsigned int i = 1; i <= record_count; i++)
	{
		// Read in the record
		Record temp_record;
		temp_record.set_table(table);
		temp_record.read(db_file);

		// Check for a matching value
		if (temp_record.get_float(fixed_name.get()) == value && temp_record.get_id() != 0)
			records.push_back(temp_record);
	}
	
	return records;
}

/* This function allows the user to search the database for a record
* based on the value in a particular char16 field
*/
std::vector<DB::Record> DB::search_char16(std::string name, std::string value)
{
	// Create a vector of records to return
	std::vector<Record> records;
	
	// If the provided field isn't in the list of char16 fields, don't search
	std::map<std::string, Field> fields = table.get_fields();
	FixedString8 fixed_name(name);
	if (fields[fixed_name.get()].get_type() != ATTR_CHAR16)
		return records;

	// Open a stream with the database file
	std::string db_filename = db_name + DB_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);

	// Ensure we are at the beginning of the file to avoid any data corruption
	if (db_file.tellp() != 0)
		return records;
	
	// Read in the field size and number of fields from disk
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));


	/* Calculate where to start the record search by using the table offset and
	* retrieving the number of records
	*/
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	db_file.seekp(table_offset);
	
	unsigned int record_count;
	int record_size;
	db_file.read((char*)&record_count, sizeof(unsigned int));
	db_file.read((char*)&field_size, sizeof(int));
	
	/* Search records by reading in records one by one (linear search)
	* and checking if the value in the desired field matches the desired value
	*/
	for (unsigned int i = 1; i <= record_count; i++)
	{
		// Read in the record
		Record temp_record;
		temp_record.set_table(table);
		temp_record.read(db_file);
		AttrChar16 temp_value;
		temp_value.set_data(value);
		
		// Check for a matching value
		if (temp_record.get_char16(fixed_name.get()) == value && temp_record.get_id() != 0)
			records.push_back(temp_record);
	}
	
	return records;
}

// This method deletes a record in the database by id
void DB::remove(unsigned int id)
{	
	// If the provided id isn't within a valid range, return
	if (id > record_count || id <= 0)
		return;
	
	// Open a stream with the database file and the temporary file
	std::string db_filename = db_name + DB_EXT;
	std::string db_filename_temp = db_name + DB_EXT + TEMP_EXT;
	std::fstream db_file(db_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);
	std::fstream db_file_temp(db_filename_temp.c_str(), std::ios::out | std::ios::binary);

	// Ensure we are at the beginning of the file to avoid any data corruption
	if (db_file.tellp() != 0)
		return;
	
	// Read in the field size and number of fields from disk
	int num_fields;
	unsigned int field_size;
	db_file.read((char*)&num_fields, sizeof(int));
	db_file.read((char*)&field_size, sizeof(unsigned int));


	/* Calculate where to start the record reading by using the table offset and
	* retrieving the number of records
	*/
	int table_offset = sizeof(int) + sizeof(unsigned int) + (field_size * num_fields);
	db_file.seekp(table_offset);
	
	unsigned int record_count;
	int record_size;
	db_file.read((char*)&record_count, sizeof(unsigned int));
	db_file.read((char*)&record_size, sizeof(int));

	// Calculate an offset to the record so it can be marked as removed
	Record temp_record;
	temp_record.set_table(table);
	int record_offset = table_offset + sizeof(unsigned int) + sizeof(int) + (record_size * (id - 1));
	db_file.seekp(record_offset);

	/* Retrieve the record, mark the record as removed, and rewrite it
	* If the record is already marked as removed, return
	*/
	temp_record.read(db_file);

	if (temp_record.get_id() == 0)
		return;

	temp_record.set_id(0);
	db_file.seekp(record_offset);
	temp_record.write(db_file);

	removed_count++;

	// Check to see if the removed count has reached the threshold for rewriting records
	if (( (double) removed_count / (double) record_count) < (1 / (double) REMOVED_THRESHOLD_DENOM))
	{
		db_file.close();
		db_file_temp.close();
		return;
	}
	
	/* Read existing records and rewrite to the temporary database
	* Do not rewrite records marked as deleted
	* Update the ids of records along the way to ensure there are no gaps
	* Start by writing the table data to the temporary file and a temporary record count and record size as a placeholder
	* After the new record count has been determined it will be rewritten
	*/
	table.write(db_file_temp);
	db_file_temp.write(reinterpret_cast<const char*>(&record_count), sizeof(unsigned int));
	db_file_temp.write(reinterpret_cast<const char*>(&record_size), sizeof(int));

	record_offset = table_offset + sizeof(unsigned int) + sizeof(int);
	db_file.seekp(record_offset);
	int shift = 0;
	for (unsigned int i = 1; i < record_count + 1; i++)
	{
		// Read in the record
		Record temp_record;
		temp_record.set_table(table);
		temp_record.read(db_file);
		unsigned int cur_id = temp_record.get_id();

		/* If the record is marked as removed, don't rewrite it
		* Decrease the shift counter so subsequent records ids can be shifted
		*/
		if (cur_id == 0)
		{
			shift--;
		}
		else
		{
			temp_record.set_id(cur_id + shift);
			temp_record.write(db_file_temp);
		}
	}

	/* Update the record count and removed count
	* Write the table and record info to the temporary file
	*/
	record_count += shift;
	this -> record_count = record_count;
	removed_count = 0;

	db_file_temp.seekp(table_offset);
	db_file_temp.write(reinterpret_cast<const char*>(&record_count), sizeof(unsigned int));

	db_file.close();
	db_file_temp.close();

	// Finally, rename the temporary file to replace the main database file
	std::remove(db_filename.c_str());
	std::rename(db_filename_temp.c_str(), db_filename.c_str());
}
