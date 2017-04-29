## General
____________

### Author
* Josh McIntyre

### Website
* jmcintyre.net

### Overview
* Powderbase provides a flat file, single table database engine and API

## Development
________________

### Git Workflow
* master for releases (merge development)
* development for bugfixes and new features

### Building
* make build
Build the library
* make tools
Build associated utilities
* make clean
Clean the build directory

### Features
* Create a database
* Load a database for operations
* Insert a record
* Update a record
* Search records
* Remove a record

### Requirements
* Requires the C++ standard library

### Platforms
* Linux
* Windows
* Mac OSX

## Usage
____________

### Including and Compiling
* Using the library in an application

Add `#include <DB.h>` to use the library

* Compiling an application that uses Powderbase

Add DB.a to the list of files to compile, and ensure the DB.h header is in the include path

Ex: `g++ -o bin/sample src/tools/sample.cpp lib/DB.a -Ilib`

### Creating and Loading a database
* Declare a database object

This object will be used to perform all of the desired operations on the database

`DB db;`

* Declare a table object and add fields to the table. In other words, define your table's columns

First declare a table object using `DB::Table table;`.

All table operations will be performed on this object.
Next, add fields to the table, specifying the field name and the field type (ATTR\_INT, ATTR\_FLOAT, ATTR\_CHAR16)
Ex: 

    table.add_field("Name", DB::ATTR_CHAR16);
    table.add_field("Squat", DB::ATTR_INT);
    table.add_field("Press", DB::ATTR_INT);
    table.add_field("Deadlift", DB::ATTR_INT);
    table.add_field("Wilks", DB::ATTR_FLOAT);

* After creating the table, create the database by specifying the name and the table to use

`db.create("sample", table);`

* Loading an existing database in an application

To load an existing database, create a database object and call db.load with the database name.
This allows the database engine to load important information from the database file and makes it ready for record operations

    DB db;
    db.load("db_name");

### Performing Record Operations
* Inserting a record

To insert a record, first create a record object, specify which table to use, and add data to each field that you specified in your table.
Ex:

    DB::Record record;
    record.set_table(table);
    record.add_char16("Name", "Josh");
    record.add_int("Squat", 245);
    record.add_int("Press", 105);

Note: If you don't specify data for a particular field, it will be filled with a default value (0, 0.0, "" for ATTR\_INT, ATTR\_FLOAT, and ATTR\_CHAR16 respectively).

Once the record object is filled in, call insert on the database object to commit that record

`db.insert(record);`

* Updating a record

To update a record, first set the record ID to the ID of the record in the database you wish to update. Then, use the add methods to update the data. Ex:

    record.set_id(1);
    record.add_int("Deadlift", 305);
    record.add_float("Wilks", 235.72);
    db.update(record);

* Searching for records

To search for records, you'll call the appropriate method based on which ATTR type you want to search on. Also specify the field name and value to match.

This method returns an `std::vector` of record objects, so you'll want to declare an `std::vector<DB::Record>` variable to store the search results in and then call the search function. Ex:

`std::vector<DB::Record> records = db.search_float("Wilks", 235.72);`

The returned records can then be used to retrieve the data as needed. Ex:

    for (int i = 0; i < records.size(); i++)
    {
        std::cout << "id: " << records[i].get_id() << "\\n";
        std::cout << "Name: " << records[i].get_char16("Name") << "\\n";
        std::cout << "Squat: " << records[i].get_int("Squat") << "\\n";
        std::cout << "Press: " << records[i].get_int("Press") << "\\n";
        std::cout << "Deadlift: " << records[i].get_int("Deadlift") << "\\n";
        std::cout << "Wilks: " << records[i].get_float("Wilks") << "\\n";
    }

The above example loops through all of the retrieved records and prints out the value in each field.

* Removing a record

Removing a record is one of the simplest operations. Call the remove function on the database, specifying the ID of the record to be removed. Ex:

`db.remove(1);`

Note: as of `development c18882b`, a removed record may not be deleted from the file on disk right away. In order to achieve better performance, records are initially kept in the file and marked as removed. When about half of the database is marked as removed, the engine will rewrite the file on disk to free up space.

### Code samples
* For a complete source code example, read `src/tools/sample.cpp` and `src/tools/perf.cpp`
