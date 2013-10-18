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
#include "schema.hpp"

#if !defined __WINDOWS_COMPILING_
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#else
//#include ??
#endif

using namespace openDB;

schema::schema (std::string schemaName, std::string storageDirectory) throw (basic_exception&) {

	(schemaName != "" ? __schemaName = schemaName : throw access_exception("Error creating schema: you can not create a schema with no name. Check the 'schemaName' paramether."));

	#if !defined __WINDOWS_COMPILING_
		(storageDirectory != "" ? __storageDirectory = storageDirectory + __schemaName + "/" : throw storage_exception("Error creating schema '" + schemaName + "': you must specify where to store this schema. Check the 'storageDirectory' paramether."));
		/*	codice per la creazione di cartelle dedite alla memorizzazione di schemi e tabelle che compongono il database per sistema operativo linux	*/
		mkdir(__storageDirectory.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#else
		/*	codice per la creazione di cartelle dedite alla memorizzazione di schemi e tabelle che compongono il database per sistema operativo windows	*/
	#endif
};


void schema::add_table(table& _tableObject) throw (basic_exception&) {}

std::unique_ptr<std::list<std::string>> schema::tables_name (bool attach_schema_name) const throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list<std::string>);
	for (std::unordered_map <std::string, table>::const_iterator it = __tablesMap.begin(); it != __tablesMap.end(); it++)
		(attach_schema_name ? list_ptr -> push_back(__schemaName + "." + (it -> first)) : list_ptr -> push_back(it -> first));
	return list_ptr;
}

std::unordered_map<std::string, table>::const_iterator schema::get_iterator(std::string tableName) const throw (table_not_exists&) {
	std::unordered_map <std::string, table>::const_iterator it = __tablesMap.find(tableName);
	if (it != __tablesMap.end())
		return it;
	else
		throw table_not_exists("'" + tableName + "' doesn't exists in schema '" + __schemaName + "'");
}

std::unordered_map<std::string, table>::iterator schema::get_iterator(std::string tableName) throw (table_not_exists&) {
	std::unordered_map <std::string, table>::iterator it = __tablesMap.find(tableName);
	if (it != __tablesMap.end())
		return it;
	else
		throw table_not_exists("'" + tableName + "' doesn't exists in schema '" + __schemaName + "'");
}

std::unique_ptr<std::list<std::string>> schema::commit() const throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list<std::string>);
	for (std::unordered_map <std::string, table>::const_iterator table_it = __tablesMap.begin(); table_it != __tablesMap.end(); table_it++) {
		if (!table_it->second.manages_result()) {
			std::unique_ptr<std::list<unsigned long>> ID_list = table_it->second.internalID();
			for(std::list<unsigned long>::const_iterator ID_it = ID_list->begin(); ID_it !=  ID_list->end(); ID_it++)
				switch(table_it->second.state(*ID_it)) {
					case record::inserting:
						list_ptr->push_back(insert_sql(table_it->first, *ID_it));
						break;
					case record::updating:
						list_ptr->push_back(update_sql(table_it->first, *ID_it));
						break;
					case record::deleting:
						list_ptr->push_back(delete_sql(table_it->first, *ID_it));
						break;
					default: break;
					}
		}
	}
	return list_ptr;
}

std::string schema::load_command(std::string tableName) const throw (table_not_exists&) {
	std::unordered_map <std::string, table>::const_iterator it = __tablesMap.find(tableName);
		if (it != __tablesMap.end()) {
			std::string sql_column_name;
			std::string sql_where;
			std::unique_ptr<std::list<std::string>> column_list = it->second.columns_name();
			for (std::list<std::string>::const_iterator list_it = column_list->begin(); list_it != column_list->end(); list_it++) {
				if(!sql_column_name.empty())
					sql_column_name += ", ";
				sql_column_name += *list_it;
				if (it->second.get_column(*list_it).get_attribute().select()) {
					if (!sql_where.empty())
						sql_where+= " and ";
					sql_where += *list_it + it->second.get_column(*list_it).get_attribute().compareOperator() + it->second.get_column(*list_it).get_attribute().selectValue();
				}
			}
			return "select " + sql_column_name + " from " + __schemaName + "." + tableName + ((!sql_where.empty()) ? (" where " + sql_where) : "" );
		}
		else
			throw table_not_exists("'" + tableName + "' doesn't exists in schema '" + __schemaName + "'");
}

std::unique_ptr<std::unordered_map<std::string, std::string>> schema::load_command() const throw () {
	std::unique_ptr<std::unordered_map<std::string, std::string>> list_ptr (new std::unordered_map<std::string, std::string>);
	for (std::unordered_map <std::string, table>::const_iterator it = __tablesMap.begin(); it != __tablesMap.end(); it++)
		list_ptr->emplace(it->first, load_command(it->first));
	return list_ptr;
}

std::string schema::insert_sql(std::string tableName, unsigned long ID) const throw (basic_exception&) {
	std::unordered_map <std::string, table>::const_iterator it = __tablesMap.find(tableName);
	if (it != __tablesMap.end()) {
		std::string sql_column;
		std::string sql_values;
		std::unique_ptr<std::unordered_map<std::string, std::string>> value_map_ptr = it->second.current(ID);
		for (std::unordered_map<std::string, std::string>::const_iterator value_it = value_map_ptr->begin(); value_it != value_map_ptr->end(); value_it++) {
			if (!sql_column.empty())
				sql_column+= ", ";
			sql_column += value_it->first;
			if (!sql_values.empty())
				sql_values += ", ";
			sql_values += it->second.get_column(value_it->first).prepare_value(value_it -> second);
		}

		std::string sql_command = "insert into " + __schemaName + "." + tableName + " (" + sql_column + ") values (" + sql_values + ")";
		return sql_command;
	}
	else
		throw table_not_exists("'" + tableName + "' doesn't exists in schema '" + __schemaName + "'");
}

std::string schema::update_sql(std::string tableName, unsigned long ID) const throw (basic_exception&) {
	std::unordered_map <std::string, table>::const_iterator it = __tablesMap.find(tableName);
	if (it != __tablesMap.end()) {
		std::string sql_value;
		std::string sql_where;
		std::unique_ptr<std::unordered_map<std::string, std::string>> value_map_ptr = it->second.current(ID);
		for (std::unordered_map<std::string, std::string>::const_iterator value_it = value_map_ptr->begin(); value_it != value_map_ptr->end(); value_it++) {
			if (it->second.get_column(value_it->first).is_key()) {
				if (!sql_where.empty())
					sql_where += " and ";
				sql_where += value_it -> first + "=" + it->second.get_column(value_it->first).prepare_value(value_it -> second);
			}
			else {
				if (!sql_value.empty())
					sql_value += ", ";
				sql_value += value_it -> first + "=" + it->second.get_column(value_it->first).prepare_value(value_it -> second);
			}
		}

		if (sql_where.empty()) {
			std::unique_ptr<std::unordered_map<std::string, std::string>> old_value_map_ptr = it->second.old(ID);
			for (std::unordered_map<std::string, std::string>::const_iterator value_it = old_value_map_ptr->begin(); value_it != old_value_map_ptr->end(); value_it++) {
				if (sql_where != "")
					sql_where += " and ";
				sql_where += value_it -> first + "=" + it->second.get_column(value_it->first).prepare_value(value_it -> second);
			}
		}

		std::string sql_command = "update " + __schemaName + "." + tableName + " set " + sql_value + " where " + sql_where;
		return sql_command;
	}
	else
		throw table_not_exists("'" + tableName + "' doesn't exists in schema '" + __schemaName + "'");
}

std::string schema::delete_sql(std::string tableName, unsigned long ID) const throw (basic_exception&) {
	std::unordered_map <std::string, table>::const_iterator it = __tablesMap.find(tableName);
	if (it != __tablesMap.end()) {
		std::string sql_where;
		std::unique_ptr<std::unordered_map<std::string, std::string>> value_map_ptr = it->second.current(ID);
		for (std::unordered_map<std::string, std::string>::const_iterator value_it = value_map_ptr->begin(); value_it != value_map_ptr->end(); value_it++)
			if (it->second.get_column(value_it->first).is_key()) {
				if (!sql_where.empty())
					sql_where += " and ";
				sql_where += value_it -> first + "=" + it->second.get_column(value_it->first).prepare_value(value_it -> second);
			}

		if (sql_where.empty()) {
			std::unique_ptr<std::unordered_map<std::string, std::string>> old_value_map_ptr = it->second.old(ID);
			for (std::unordered_map<std::string, std::string>::const_iterator value_it = old_value_map_ptr->begin(); value_it != old_value_map_ptr->end(); value_it++) {
				if (sql_where != "")
					sql_where += " and ";
				sql_where += value_it -> first + "=" + it->second.get_column(value_it->first).prepare_value(value_it -> second);
			}
		}

		std::string sql_command = "delete from " + __schemaName + "." + tableName + " where " + sql_where;;
		return sql_command;
	}
	else
		throw table_not_exists("'" + tableName + "' doesn't exists in schema '" + __schemaName + "'");
}

