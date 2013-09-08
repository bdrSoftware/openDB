/* Copyright 2013 Salvatore Barone <salvator.barone@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "database.hpp"

#if !defined __WINDOWS_COMPILING_
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#else
//#include ??
#endif

using namespace openDB;

const std::string database::__tmpStorageDirectory = "platinet";

database::database(std::string databaseName) throw (access_exception&) {
	(databaseName != "" ? __databaseName = databaseName : throw access_exception("Error creating database: you can not create a database with no name. Check the 'databaseName' paramether."));
	#if !defined __WINDOWS_COMPILING_
		/*	codice per la creazione di cartelle dedite alla memorizzazione di schemi e tabelle che compongono il database per sistema operativo linux	*/
		__storageDirectory = "/tmp/" + __tmpStorageDirectory + "/";
		mkdir(__storageDirectory.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		__storageDirectory += std::to_string(getpid()) + "/";
		mkdir(__storageDirectory.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		__storageDirectory += __databaseName + "/";
		mkdir(__storageDirectory.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#else
		/*	codice per la creazione di cartelle dedite alla memorizzazione di schemi e tabelle che compongono il database per sistema operativo windows	*/
	#endif
}

std::unique_ptr<std::list<std::string>> database::schemas_name (bool attach_database_name) const throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list<std::string>);
	for (std::unordered_map <std::string, schema>::const_iterator it = __schemasMap.begin(); it != __schemasMap.end(); it++)
		(attach_database_name ? list_ptr -> push_back(__databaseName + "." + it -> first) : list_ptr -> push_back(it -> first));
	return list_ptr;
}

std::unordered_map<std::string, schema>::const_iterator database::get_iterator(std::string schemaName) const throw (schema_not_exists&) {
	std::unordered_map <std::string, schema>::const_iterator it = __schemasMap.find(schemaName);
	if (it != __schemasMap.end())
		return it;
	else
		throw schema_not_exists("'" + schemaName + "' doesn't exists in database '" + __databaseName + "'");
}

std::unordered_map<std::string, schema>::iterator database::get_iterator(std::string schemaName) throw (schema_not_exists&) {
	std::unordered_map <std::string, schema>::iterator it = __schemasMap.find(schemaName);
	if (it != __schemasMap.end())
		return it;
	else
		throw schema_not_exists("'" + schemaName + "' doesn't exists in database '" + __databaseName + "'");
}
