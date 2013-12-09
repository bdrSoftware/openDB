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
#include <iostream>

#if !defined __WINDOWS_COMPILING_
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#define base_path "/tmp/"
#else
//#include ??
	#define base_path "/tmp/"
#endif

using namespace openDB;

const std::string database::__tmpStorageDirectory = "platinet";

const std::string database::__other_column = "select col.table_schema,col.table_name,col.column_name,col.udt_name,col.character_maximum_length,col.numeric_precision,col.numeric_scale from information_schema.columns col where col.table_schema not in('pg_catalog', 'information_schema') and (col.table_schema, col.table_name, col.column_name) not in (select col.table_schema, col.table_name, col.column_name from information_schema.columns col join information_schema.constraint_column_usage ccl on col.table_schema=ccl.table_schema and col.table_name=ccl.table_name and col.column_name=ccl.column_name join information_schema.table_constraints ts on ccl.constraint_name=ts.constraint_name where ts.constraint_type = 'PRIMARY KEY' and col.table_schema not in ('pg_catalog','information_schema')) order by col.table_schema,col.table_name, col.ordinal_position";
const std::string database::__key_column  = "select col.table_schema,col.table_name,col.column_name,col.udt_name,col.character_maximum_length,col.numeric_precision,col.numeric_scale from information_schema.columns col where col.table_schema not in('pg_catalog', 'information_schema') and (col.table_schema, col.table_name, col.column_name) in (select col.table_schema, col.table_name, col.column_name from information_schema.columns col join information_schema.constraint_column_usage ccl on col.table_schema=ccl.table_schema and col.table_name=ccl.table_name and col.column_name=ccl.column_name join information_schema.table_constraints ts on ccl.constraint_name=ts.constraint_name where ts.constraint_type = 'PRIMARY KEY' and col.table_schema not in ('pg_catalog','information_schema')) order by col.table_schema,col.table_name, col.ordinal_position";


const database::column_query_field_name database::column_field_name = {
		"table_schema",
		"table_name",
		"column_name",
		"udt_name",
		"character_maximum_length",
		"numeric_precision",
		"numeric_scale",
};


database::database(unsigned _cuncurrend_connection) throw (storage_exception&) : __remote_database(_cuncurrend_connection) {
	#if !defined __WINDOWS_VERSION
		/*	codice per la creazione di cartelle dedite alla memorizzazione di schemi e tabelle che compongono il database per sistema operativo linux/unix-like	*/
		__storageDirectory = base_path + __tmpStorageDirectory + "/";
		mkdir(__storageDirectory.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		__storageDirectory += std::to_string(getpid()) + "/";
		mkdir(__storageDirectory.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#else
		/*	codice per la creazione di cartelle dedite alla memorizzazione di schemi e tabelle che compongono il database per sistema operativo windows	*/
	#endif
}

std::unique_ptr<std::list<std::string>> database::schemas_name () const throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list<std::string>);
	for (std::unordered_map <std::string, schema>::const_iterator it = __schemasMap.begin(); it != __schemasMap.end(); it++)
		list_ptr -> push_back(it -> first);
	return list_ptr;
}

std::unordered_map<std::string, schema>::const_iterator database::get_iterator(std::string schemaName) const throw (schema_not_exists&) {
	std::unordered_map <std::string, schema>::const_iterator it = __schemasMap.find(schemaName);
	if (it != __schemasMap.end())
		return it;
	else
		throw schema_not_exists("'" + schemaName + "' doesn't exists in database.");
}

std::unordered_map<std::string, schema>::iterator database::get_iterator(std::string schemaName) throw (schema_not_exists&) {
	std::unordered_map <std::string, schema>::iterator it = __schemasMap.find(schemaName);
	if (it != __schemasMap.end())
		return it;
	else
		throw schema_not_exists("'" + schemaName + "' doesn't exists in database.");
}

void database::load_structure() throw (basic_exception&) {
	__schemasMap.clear();
	unsigned int other_column_id = __remote_database.exec_query_noblock(__other_column);
	unsigned int key_column_id = __remote_database.exec_query(__key_column);

	table& _key_column_table = __remote_database.get_result(key_column_id);
	create_structure(_key_column_table, true);
	__remote_database.erase(key_column_id);

	while(!__remote_database.executed(other_column_id)); //attesa completamento query
	table& _other_column_table = __remote_database.get_result(other_column_id);
	create_structure(_other_column_table, false);
	__remote_database.erase(other_column_id);
}

sqlType::type_base* database::column_type(std::string udt_name, std::string character_maximum_length, std::string numeric_precision, std::string numeric_scale) {
	if (udt_name == sqlType::varchar::udt_name)
		if (character_maximum_length.empty())
			return new sqlType::varchar;
		else
			return new sqlType::varchar(std::stoul(character_maximum_length));
	if (udt_name == sqlType::character::udt_name)
		if (character_maximum_length.empty())
			return new sqlType::character;
		else
			return new sqlType::character(std::stoul(character_maximum_length));
	if (udt_name == sqlType::smallint::udt_name)
		return new sqlType::smallint;
	if (udt_name == sqlType::integer::udt_name)
		return new sqlType::integer;
	if (udt_name == sqlType::bigint::udt_name)
		return new sqlType::bigint;
	if (udt_name == sqlType::real::udt_name)
		return new sqlType::real;
	if (udt_name == sqlType::double_precision::udt_name)
		return new sqlType::double_precision;
	if (udt_name == sqlType::numeric::udt_name)
		return new sqlType::numeric(std::stoul(numeric_precision), std::stoul(numeric_scale));
	if (udt_name == sqlType::boolean::udt_name)
		return new sqlType::boolean;
	if (udt_name == sqlType::date::udt_name)
		return new sqlType::date;
	if (udt_name == sqlType::time::udt_name)
		return new sqlType::time;
	return 0;
}

void database::create_structure(table& structure_table, bool key) {
	std::unique_ptr<std::list<unsigned long>> _all_column_table_tupleID = structure_table.internalID();
	for (std::list<unsigned long>::const_iterator it = _all_column_table_tupleID->begin(); it != _all_column_table_tupleID->end(); it++) {
		std::unique_ptr<std::unordered_map<std::string, std::string>> tuple = structure_table.current(*it);
		std::string schema_name = tuple->find(column_field_name.table_schema)->second;
		std::string table_name = tuple->find(column_field_name.table_name)->second;
		std::string column_name = tuple->find(column_field_name.column_name)->second;
		std::string udt_name = tuple->find(column_field_name.udt_name)->second;
		std::string character_maximum = tuple->find(column_field_name.character_maximum_length)->second;
		std::string numeric_precision = tuple->find(column_field_name.numeric_precision)->second;
		std::string numeric_scale = tuple->find(column_field_name.numeric_scale)->second;
		sqlType::type_base* type = column_type(udt_name, character_maximum, numeric_precision, numeric_scale);

		if (!find_schema(schema_name))
			add_schema(schema_name);
		schema& _schema = get_schema(schema_name);
		if (!_schema.find_table(table_name))
			_schema.add_table(table_name);
		table& _table = _schema.get_table(table_name);
		_table.add_column(column_name, type, key);
	}
}

void database::load_tuple() throw (basic_exception&) {
	for (std::unordered_map<std::string, schema>::iterator schema_it = __schemasMap.begin(); schema_it!=__schemasMap.end(); schema_it++) {
		std::unique_ptr<std::unordered_map<std::string, std::string>> load_commands = schema_it->second.load_command();
		for (std::unordered_map<std::string, std::string>::const_iterator commands_it = load_commands->begin(); commands_it != load_commands->end(); commands_it++) {
			unsigned long query_id = __remote_database.exec_query(commands_it->second);
			table& _result = __remote_database.get_result(query_id);
			table& _table = schema_it->second.get_table(commands_it->first);
			_table.clear();
			std::unique_ptr<std::list<unsigned long>> tuple_id = _result.internalID();
			for (std::list<unsigned long>::const_iterator tuple_it = tuple_id->begin(); tuple_it != tuple_id->end(); tuple_it++)
				_table.load(*_result.current(*tuple_it));
			__remote_database.erase(query_id);
		}
	}
}

std::unique_ptr<std::list<unsigned long>> database::commit() throw (basic_exception&) {
	std::unique_ptr<std::list<std::string>> command_list = command_generator();
	std::unique_ptr<std::list<unsigned long>> id_list(new std::list<unsigned long>);
	for (std::list<std::string>::const_iterator it = command_list->begin(); it != command_list->end(); it++)
		id_list->push_back(__remote_database.exec_query(*it));
	return id_list;
}

std::unique_ptr<std::list<unsigned long>> database::commit_noblock() throw (basic_exception&) {
	std::unique_ptr<std::list<std::string>> command_list = command_generator();
	std::unique_ptr<std::list<unsigned long>> id_list(new std::list<unsigned long>);
	for (std::list<std::string>::const_iterator it = command_list->begin(); it != command_list->end(); it++)
		id_list->push_back(__remote_database.exec_query_noblock(*it));
	return id_list;
}

std::unique_ptr<std::list<std::string>> database::command_generator() const throw () {
	std::unique_ptr<std::list<std::string>> list_ptr(new std::list<std::string>);
	for (std::unordered_map<std::string, schema>::const_iterator schema_it = __schemasMap.begin(); schema_it!=__schemasMap.end(); schema_it++) {
		std::unique_ptr<std::list<std::string>> tmp = schema_it->second.commit();
		list_ptr->splice(list_ptr->end(), *tmp);
	}
	return list_ptr;
}
