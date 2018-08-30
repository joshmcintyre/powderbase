/* This file contains driver code that shows sample PowderBase usage
* This file contains the main entry point for the program
*
* Author: Josh McIntyre
*/

#include <DB.h>
#include <iostream>

/* This function is the main entry point for the program
*
* Argument: argc
* Argument: argv
* Return: 0 (exit status)
*/
int main(int argc, char* argv[])
{
	DB db;

	// Test table creation and database creation
	DB::Table table;
	table.add_field("Name", DB::ATTR_CHAR16);
	table.add_field("StudentIdentification", DB::ATTR_INT);
	table.add_field("Grade", DB::ATTR_FLOAT);
	db.create("test", table);
	
	// Test record creation
	DB::Record record;
	record.set_table(table);
	record.add_char16("Name", "Josh");
	record.add_int("StudentIdentification", 111);
	record.add_float("Grade", 95.50);
	db.insert(record);

	// Test record update
	record.set_id(1);
	record.add_int("Grade", 97.50);
	db.update(record);

	// Test record search before deletion
	std::vector<DB::Record> records = db.search_int("StudentI", 111);
	std::cout << "Search for StudentI 111\n";
	for (int i = 0; i < records.size(); i++)
	{
		std::cout << "id: " << records[i].get_id() << "\n";
		std::cout << "Name: " << records[i].get_char16("Name") << "\n";
		std::cout << "StudentIdentification: " << records[i].get_int("StudentIdentification") << "\n";
		std::cout << "Grade: " << records[i].get_float("Grade") << "\n";
	}

	// Test record removal
	db.remove(1);
	
	// Test record search after deletion
	records = db.search_int("StudentI", 111);
	std::cout << "Search for StudentI 111\n";
	for (int i = 0; i < records.size(); i++)
	{
		std::cout << "id: " << records[i].get_id() << "\n";
		std::cout << "Name: " << records[i].get_char16("Name") << "\n";
		std::cout << "StudentIdentification: " << records[i].get_int("StudentIdentification") << "\n";
		std::cout << "Grade: " << records[i].get_float("Grade") << "\n";
	}

}


