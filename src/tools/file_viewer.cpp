/* This file contains code that reads and prints the contents of a PowderBase .pb file
*
* Author: Josh McIntyre
*
*/

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <map>

/* This function is the main entry point for the program
* It takes the name of the file as a command line argument
*
* Argument: argc
* Argument: argv
* Return: 0 (exit status)
*
*/
int main(int argc, char* argv[])
{
	/* Define constants and variables for printing file information
	*
	*/
	const int NAME_SIZE = 8;
	const int CHAR_16_SIZE = 16;
	std::map<int, int> type_sizes; 
	type_sizes[-1] = sizeof(unsigned int); //AttrID
	type_sizes[0] = sizeof(int); //AttrInt
	type_sizes[1] = sizeof(float); //AttrFloat
	type_sizes[2] = sizeof(char) * CHAR_16_SIZE; //AttrChar16
	std::map<std::string, int> name_types;

	/* Get command line arguments
	*
	*/
	std::string file_name;
	bool verbose = false;

	for (int i = 1; i < argc; i++)
	{
		if (argv[i] == std::string("-f") || argv[i] == std::string("--file"))
		{
			file_name = argv[i + 1];
			i++;
			continue;
		}
		else if (argv[i] == std::string("-v") || argv[i] == std::string("--verbose"))
		{
			verbose = true;
		}
	}

	if (file_name == "")
	{
		std::cout << "Usage fileviewer [required: -f/--file <file path>] [optional: -v/--verbose <verbose output>]\n";
		exit(EXIT_FAILURE);
	}

	/* Print any database metadata in verbose mode
	*
	*/
	if (verbose)
	{
		std::fstream db_file(file_name.c_str(), std::fstream::in | std::ios::ate | std::ios::binary);
		std::cout << "Database size (bytes) " << db_file.tellg() << "\n";
		db_file.close();
	}

	/* Open a stream with the database file
	*
	*/
	std::fstream db_file(file_name.c_str(), std::fstream::in | std::ios::binary);
	
	/* Read and print the table information
	*
	*/

	if (verbose)
		std::cout << "Table\n";

	if (verbose)
		std::cout << " (byte " << db_file.tellg() << ") ";
	int num_fields;
	db_file.read((char*)&num_fields, sizeof(int));
	std::cout << num_fields << "\n";

	if (verbose)
		std::cout << " (byte " << db_file.tellg() << ") ";
	unsigned int field_size;
	db_file.read((char*)&field_size, sizeof(unsigned int));
	std::cout << field_size << "\n";

	for (int i = 0; i < num_fields; i++)
	{

		if (verbose)
			std::cout << " (byte " << db_file.tellg() << ") ";
		std::string name;
		name.resize(NAME_SIZE);
		db_file.read(&name[0], NAME_SIZE);
		std::cout << name << "\n";

		if (verbose)
			std::cout << " (byte " << db_file.tellg() << ") ";
		int type;
		db_file.read((char*)&type, sizeof(int));
		std::cout << type << "\n";


		/* Store table information for retrieving records
		*
		*/
		name_types[name] = type;

		/* In later versions, we will want to check for other table items
		*
		*
		if (field_size > 2)
		{

		}
		*/
	}

	/* Read and print record information
	*
	*/
	if (verbose)
		std::cout << "Records\n";

	if (verbose)
		std::cout << " (byte " << db_file.tellg() << ") ";
	unsigned int record_count;
	db_file.read((char*)&record_count, sizeof(unsigned int));
	std::cout << record_count << "\n";

	if (verbose)
		std::cout << " (byte " << db_file.tellg() << ") ";
	int record_size;
	db_file.read((char*)&record_size, sizeof(int));
	std::cout << record_size << "\n";

	for(int i = 0; i < record_count; i++)
	{
		for (int c = 0; c < num_fields; c++)
		{

			if (verbose)
				std::cout << " (byte " << db_file.tellg() << ") ";
			std::string name;
			name.resize(NAME_SIZE);
			db_file.read(&name[0], NAME_SIZE);
			std::cout << name << "\n";

			/* Determine how much to read based on the field type in the table
			*
			*/
			int type = name_types[name];

			if (type == -1)
			{
				if (verbose)
					std::cout << " (byte " << db_file.tellg() << ") ";
				unsigned int id;
				db_file.read((char*)&id, sizeof(unsigned int));
				std::cout << id << "\n";
			}
			else if (type == 0) //AttrInt
			{
				if (verbose)
					std::cout << " (byte " << db_file.tellg() << ") ";
				int data;
				db_file.read((char*)&data, sizeof(int));
				std::cout << data << "\n";
			}
			else if (type == 1) //AttrFloat
			{
				if (verbose)
					std::cout << " (byte " << db_file.tellg() << ") ";
				float data;
				db_file.read((char*)&data, sizeof(float));
				std::cout << data << "\n";
			}
			else if (type == 2) //AttrChar16
			{
				if (verbose)
					std::cout << " (byte " << db_file.tellg() << ") ";
				std::string data;
				data.resize(CHAR_16_SIZE);
				db_file.read(&data[0], CHAR_16_SIZE);
				std::cout << data << "\n";
			}
		}

	}

	db_file.close();

}
