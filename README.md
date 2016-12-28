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

### Library Usage
* Using the library in an application
Add `#include <DB.h>` to use the library
* Compiling an application that uses Powderbase
Add DB.a to the list of files to compile, and ensure the DB.h header is in the include path
Ex: `g++ -o bin/sample src/tools/sample.cpp lib/DB.a -Ilib`
* Creating a table and database in an application
See src/tools/sample.cpp for an example of table creation and database create
* Loading an existing database in an application
To load an existing database, create a database object and call db.load with the database name
`DB db; db.load("db_name");`
* Performing CRUD operations in an application
See src/tools/sample.cpp for examples of insert, update, search, and remove operations
