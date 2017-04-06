/* This file contains class definitions for the PowderBase DB API class and associated classes
*
* Author: Josh McIntyre
*
*/

#ifndef DB_H
#define DB_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <vector>

/* Define constants for the database API
*
*/
const std::string DB_EXT = ".pb";
const std::string TEMP_EXT = ".tmp";

/* This class defines the public DB API
* Its member functions provide end user functionality such as
* database creation and record insertion
*
*/
class DB
{
	/* This block defines private DB classes and variables
	*
	*/
	private:
		
		/* Define important constants for manipulating the database
		*
		*/
		static const int ATTR_ID = -1;
		static const int REMOVED_THRESHOLD_DENOM = 2;

		/* This class stores information about attributes in a flat table
		*
		*/
		class Attr
		{

			/* This block defines variables and constants for storing Attr information
			*
			*/
			protected:
				static const int NAME_SIZE = 8;
				std::string name;

			/* This block defines functions for handling Attr information
			*
			*/
			public:
				void set_name(std::string name);
				std::string get_name();
				int get_name_size();
		};

		/* This class stores information about record ID attributes in a flat table
		*
		*/
		class AttrID : public Attr
		{
			/* This block defines variables for storing record ID Attr information
			*
			*/
			private:
				unsigned int data;
				unsigned int size;

			/* This block defines functions for handling ID Attr information
			*
			*/
			public:
				AttrID();
				void write(std::fstream& stream);
				void read(std::fstream& stream);
				void set_data(const unsigned int data);
				unsigned int get_data();
				unsigned int get_size();

		};

		/* This class stores information about integer attributes in a flat table
		*
		*/
		class AttrInt : public Attr
		{
			/* This block defines variables for storing integer Attr information
			*
			*/
			private:
				int data;
				unsigned int size;

			/* This block defines functions for handling integer Attr information
			*
			*/
			public:
				AttrInt();
				void write(std::fstream& stream);
				void read(std::fstream& stream);
				void set_data(const int data);
				int get_data();
				unsigned int get_size();

		};

		/* This class stores information about floating point attributes in a flat table
		*
		*/
		class AttrFloat : public Attr
		{
			/* This block defines variables for storing integer Attr information
			*
			*/
			private:
				float data;
				unsigned int size;

			/* This block defines functions for handling floating point Attr information
			*
			*/
			public:
				AttrFloat();
				void write(std::fstream& stream);
				void read(std::fstream& stream);
				void set_data(const float data);
				float get_data();
				unsigned int get_size();

		};

		/* This class stores information about character attributes in a flat table
		* This attribute stores a maximum of 16 characters
		*
		*/
		class AttrChar16: public Attr
		{
			/* This block defines variables for storing integer Attr information
			*
			*/
			private:
				static const int CHAR_16_SIZE = 16;
				std::string data;
				unsigned int size;

			/* This block defines functions for handling floating point Attr information
			*
			*/
			public:
				AttrChar16();
				void write(std::fstream& stream);
				void read(std::fstream& stream);
				void set_data(std::string data);
				std::string get_data();
				unsigned int get_size();

		};

		/* This class stores table field information
		*
		*/
		class Field
		{
			/* This block defines variables and constants for storing Table information
			*
			*/
			private:
				static const int NAME_SIZE = 8;
				unsigned int size;
				std::string name;
				int type;

			/* This block defines functions for handling Table information
			*
			*/
			public:
				Field();
				Field(unsigned int size);
				void write(std::fstream& stream);
				void read(std::fstream& stream);
				void set_name(std::string name);
				void set_type(int type);
				unsigned int get_size();
				std::string get_name();
				int get_type();
		};

	/* This block exposes public database API functions as well as public data types
	*
	*/
	public:
		
		/* This enum declares the available Field datatypes in the database
		*
		*/
		enum ATTR_TYPES { ATTR_INT, ATTR_FLOAT, ATTR_CHAR16 };
		
		/* This class stores table information
		*
		*/
		class Table
		{
			/* This block defines variables for storing table information
			*
			*/
			private:
				std::map<std::string, Field> fields;
				
			/* This block defines functions for setting table information
			*
			*/
			public:
				void write(std::fstream& stream);
				void read(std::fstream& stream);
				void add_field(std::string name, int type);
				std::map<std::string, Field> get_fields();
				bool is_field(std::string name);
		};

		/* This class stores a record built of dynamically specified Attrs
		*
		*/
		class Record
		{
			/*This block defines variables for storing a record
			* It stores maps of name, Attr pairs for each Attr data type
			* It also stores a unique ID for the record
			* It stores a copy of the database table to verify new data members
			*
			*/
			private:
				Table table;
				AttrID attr_id;
				std::map<std::string, AttrInt> attr_ints;
				std::map<std::string, AttrFloat> attr_floats;
				std::map<std::string, AttrChar16> attr_char16s;
		
			/* This block defines functions for building and searching records
			*
			*/
			public:
				void write(std::fstream& stream);
				void read(std::fstream& stream);
				void set_table(Table table);
				void set_id(unsigned int id);
				void add_int(std::string name, int data);
				void add_float(std::string name, float data);
				void add_char16(std::string name, std::string data);
				unsigned int get_id();
				int get_int(std::string name);
				float get_float(std::string name);
				std::string get_char16(std::string name);
				int get_size();
				void sanitize();
		};

		DB();
		void create(std::string db_name, Table table);
		void load(std::string db_name);
		void insert(Record record);
		void update(Record record);
		std::vector<Record> search_int(std::string field, int value);
		std::vector<Record> search_float(std::string field, float value);
		std::vector<Record> search_char16(std::string field, std::string value);
		void remove(unsigned int id);

	private:

		/* Initialize database metadata
		* This information will be updated on database insert and load operations
		*
		*/
		bool is_loaded;
		Table table;
		unsigned int record_count;
		unsigned int removed_count;
		std::string db_name;
};

#endif
