/* This file contains driver code that shows sample PowderBase usage
* This file contains the main entry point for the program
*
* Author: Josh McIntyre
*
*/

#include <DB.h>
#include <iostream>

/* This function is the main entry point for the program
*
* Argument: argc
* Argument: argv
* Return: 0 (exit status)
*
*/
int main(int argc, char* argv[])
{
	DB db;

	/* Test table creation and database creation
	*
	*/
	DB::Table table;
	table.add_field("Name", DB::ATTR_CHAR16);
	table.add_field("Squat", DB::ATTR_INT);
	table.add_field("Press", DB::ATTR_INT);
	table.add_field("Deadlift", DB::ATTR_INT);
	table.add_field("Wilks", DB::ATTR_FLOAT);
	db.create("sample", table);
	
	/* Test record creation
	*
	*/
	DB::Record record;
	record.set_table(table);
	record.add_char16("Name", "Josh");
	record.add_int("Squat", 245);
	record.add_int("Press", 105);
	db.insert(record);
	
	DB::Record record2;
	record2.set_table(table);
	record2.add_char16("Name", "Joe Lifter");
	record2.add_int("Squat", 315);
	record2.add_int("Press", 135);
	record2.add_int("Deadlift", 365);
	record2.add_float("Wilks", 235.72);
	db.insert(record2);

	/* Test record update
	*
	*/
	record.set_id(1);
	record.add_int("Deadlift", 305);
	record.add_float("Wilks", 235.72);
	db.update(record);

	/* Test record search before deletion
	*
	*/
	std::vector<DB::Record> records = db.search_float("Wilks", 235.72);
	std::cout << "Search 1\n";
	for (int i = 0; i < records.size(); i++)
	{
		std::cout << "id: " << records[i].get_id() << "\n";
		std::cout << "Name: " << records[i].get_char16("Name") << "\n";
		std::cout << "Squat: " << records[i].get_int("Squat") << "\n";
		std::cout << "Press: " << records[i].get_int("Press") << "\n";
		std::cout << "Deadlift: " << records[i].get_int("Deadlift") << "\n";
		std::cout << "Wilks: " << records[i].get_float("Wilks") << "\n";
	}

	/* Test record removal
	*
	*/
	db.remove(2);
	
	/* Test record search after deletion
	*
	*/
	records = db.search_float("Wilks", 235.72);
	std::cout << "Search 2\n";
	for (int i = 0; i < records.size(); i++)
	{
		std::cout << "id: " << records[i].get_id() << "\n";
		std::cout << "Name: " << records[i].get_char16("Name") << "\n";
		std::cout << "Squat: " << records[i].get_int("Squat") << "\n";
		std::cout << "Press: " << records[i].get_int("Press") << "\n";
		std::cout << "Deadlift: " << records[i].get_int("Deadlift") << "\n";
		std::cout << "Wilks: " << records[i].get_float("Wilks") << "\n";
	}

}


