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
#include <fstream>
using namespace openDB;

table::table (std::string tableName, std::string storageDirectory, schema* parent, bool managesResult, bool store_on_file) throw (basic_exception&) : __parent(parent),__managesResult(managesResult) {
	(!tableName.empty() ? __tableName = tableName : throw access_exception("Error creating a table: you can not create a table with no name. Check the 'tableName' paramether."));
	((store_on_file && storageDirectory.empty()) ? throw storage_exception("Error creating table '" + tableName + "': you must specify where to store table's rows. Check the 'storageDirectory' paramether.") : __storageDirectory = storageDirectory);
	(store_on_file ? __storage = std::unique_ptr<storage>(new file_storage(storageDirectory + __tableName + ".oDB")) : __storage = std::unique_ptr<storage>(new memory_storage));
}

void table::add_column(std::string columnName, sqlType::type_base* columnType, bool key) throw (column_exists&) {
	if (find_column(columnName))
		throw column_exists("'" + columnName + "' already exists in table'" + __tableName + "'");
	__columnsMap.insert(std::pair<std::string, column>(columnName, column(columnName, columnType, this, key)));
	__columnsOrder.push_back(columnName);
}


std::unique_ptr<std::list<std::string>> table::columns_name (bool attach_table_name) const throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list<std::string>);
	for (std::list <std::string>::const_iterator it =  __columnsOrder.begin(); it != __columnsOrder.end(); it++)
		(attach_table_name ? list_ptr -> push_back(__tableName + "." + *it) : list_ptr -> push_back(*it));
	return list_ptr;
}

void table::to_html (std::string fileName, bool print_row, std::string bgcolor) const throw (storage_exception&) {
	std::fstream file;
	file.open(fileName.c_str(), std::ios::out);
	if(!file.is_open())
		throw file_creation("Error: '" + fileName + "' can not be created!");

	file<<"<html>" <<std::endl
	<<"<title> openDB "  <<__tableName.c_str() <<"</title>" <<std::endl
	<<"<head> <h3> Table name: <b>" <<__tableName.c_str() << " </h3> </b></head>" <<std::endl
	<<"<body>" <<std::endl
	<<"<table border=\"1\">" <<std::endl
	<<"<tr>" <<std::endl;
	for (std::list <std::string>::const_iterator it =  __columnsOrder.begin(); it != __columnsOrder.end(); it++)
		file <<"<td><b>" <<*it <<"</b></td>" <<std::endl;
	file <<"</tr>" <<std::endl;

	bool hightlight = true;
	std::unique_ptr<std::list<unsigned long>> record_id = __storage->internalID();
	for (std::list<unsigned long>::const_iterator id_it = record_id->begin(); id_it != record_id->end(); id_it++) {
		if (__storage->visible(*id_it)) {
			((print_row && hightlight) ? file <<"<tr bgcolor=\"" <<bgcolor <<"\">" <<std::endl : file <<"<tr>" <<std::endl);
			std::unique_ptr<std::unordered_map<std::string, std::string>> values = __storage->current(*id_it);
			for (std::list <std::string>::const_iterator it =  __columnsOrder.begin(); it != __columnsOrder.end(); it++)
				file <<"<td>" <<values->find(*it)->second <<"</td>" <<std::endl;
			file <<"</tr>" <<std::endl;
			hightlight =! hightlight;
		}
	}

	file<<"</table>" <<std::endl
	<<"<p><p>" <<std::to_string(record_id->size()).c_str() <<" rows" <<std::endl
	<<"</body>" <<std::endl
	<<"</html>" <<std::endl;

	file.close();
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
