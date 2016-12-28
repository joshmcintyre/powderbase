/* This file contains function definitions for Attr class
*
* Author: Josh McIntyre
*
*/

#include <DB.h>

/* This setter sets the name
* If the passed in name is longer than the name size limit, truncate it
* If it is shorter than the name size limit, pad the string
*
* Argument: name
*
*/
void DB::Attr::set_name(std::string name)
{
	name = name.substr(0, NAME_SIZE);
	
	if (name.size() < NAME_SIZE)
	{
		int pad = NAME_SIZE - name.size();
		name.append(pad, ' ');
	}
	
	this -> name = name;
}

/* This getter returns the Attr name
*
* Return: name
*
*/
std::string DB::Attr::get_name()
{
	return name;
}

/* This getter returns the Attr name size
*
* Return: name_size
*
*/
int DB::Attr::get_name_size()
{
	return NAME_SIZE;
}