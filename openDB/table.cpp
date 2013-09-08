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
#include "table.hpp"
using namespace openDB;

table::table (std::string tableName, std::string storageDirectory, bool managesResult, bool store_on_file) throw (basic_exception&) : __managesResult(managesResult) {
	(tableName != "" ? __tableName = tableName : throw access_exception("Error creating a table: you can not create a table with no name. Check the 'tableName' paramether."));
	((store_on_file && storageDirectory == "") ? throw storage_exception("Error creating table '" + tableName + "': you must specify where to store table's rows. Check the 'storageDirectory' paramether.") : __storageDirectory = storageDirectory);
	// (store_on_file ? __storage = new file_storage() : __storage = new memory_storage);
}

std::unique_ptr<std::list<std::string>> table::columns_name (bool attach_table_name) const throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list<std::string>);
	for (std::unordered_map <std::string, column>::const_iterator it = __columnsMap.begin(); it != __columnsMap.end(); it++)
		(attach_table_name ? list_ptr -> push_back(__tableName + "." + it -> first) : list_ptr -> push_back(it -> first));
	return list_ptr;
}

std::unordered_map<std::string, column>::const_iterator table::get_iterator(std::string columnName) const throw (column_not_exists&) {
	std::unordered_map <std::string, column>::const_iterator it = __columnsMap.find(columnName);
	if (it != __columnsMap.end())
		return it;
	else
		throw column_not_exists("'" + columnName + "' doesn't exists in table '" + __tableName + "'");
}

std::unordered_map<std::string, column>::iterator table::get_iterator(std::string columnName) throw (column_not_exists&) {
	std::unordered_map <std::string, column>::iterator it = __columnsMap.find(columnName);
	if (it != __columnsMap.end())
		return it;
	else
		throw column_not_exists("'" + columnName + "' doesn't exists in table '" + __tableName + "'");
}
