/* This file contains driver code that tests the performance of core PowderBase methods
* This file contains the main entry point for the program
*
* Author: Josh McIntyre
*/

#include <DB.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <cstdlib>

// This function is the main entry point for the program
int main(int argc, char* argv[])
{
	// Get command line arguments
	int num_records = 0;
	bool verbose = false;

	for (int i = 1; i < argc; i++)
	{
		if (argv[i] == std::string("-n") || argv[i] == std::string("--num"))
		{
			std::stringstream ss(argv[i + 1]);
			if (! (ss >> num_records))
			{
				std::cout << "Usage fileviewer [required: -n/--num <num records>]\n";
				exit(EXIT_FAILURE);
			}
			i++;
			continue;
		}
	}

	if (num_records == 0)
	{
		std::cout << "Usage fileviewer [required: -n/--num <num records>]\n";
		exit(EXIT_FAILURE);
	}
	
	/* Create a database object
	* Create counter variables
	*/
	DB db;
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;
	
	// Test table creation and database creation
	start = std::chrono::high_resolution_clock::now();
	DB::Table table;
	table.add_field("Name", DB::ATTR_CHAR16);
	table.add_field("Squat", DB::ATTR_INT);
	table.add_field("Press", DB::ATTR_INT);
	table.add_field("Deadlift", DB::ATTR_INT);
	table.add_field("Wilks", DB::ATTR_FLOAT);
	db.create("perf", table);
	end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	
	std::cout << "Create: " << duration << " ms\n";
	
	// Test record creation
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < num_records; i++)
	{
		DB::Record record;
		record.set_table(table);
		record.add_char16("Name", "Josh");
		record.add_int("Squat", 245);
		record.add_int("Press", 105);
		db.insert(record);
	}
	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	
	std::cout << "Insert: " << duration << " ms\n";
	
	// Test record update
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < num_records; i++)
	{
		DB::Record record;
		record.set_table(table);
		record.set_id(i + 1);
		record.add_char16("Name", "Josh");
		record.add_int("Squat", 245);
		record.add_int("Press", 105);
		record.add_int("Deadlift", 305);
		record.add_float("Wilks", 235.72);
		db.update(record);
	}
	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	
	std::cout << "Update: " << duration << " ms\n";
	
	// Test record search
	start = std::chrono::high_resolution_clock::now();
	std::vector<DB::Record> records = db.search_int("Squat", 245);
	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	
	std::cout << "Search: " << duration << " ms\n";

	// Test record removal
	start = std::chrono::high_resolution_clock::now();
	for (int i = num_records; i > 0; i--)
	{
		db.remove(i);
	}
	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	
	std::cout << "Remove: " << duration << " ms\n";

	return 0;
}
