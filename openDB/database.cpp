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
#else
//#include ??
#endif

using namespace openDB;


const std::string database::__tmpStorageDirectory = "platinet";

const std::string database::__all_column = "select col.table_schema,col.table_name,col.column_name,col.udt_name,col.character_maximum_length,col.numeric_precision,col.numeric_scale,col.datetime_precision,col.column_default from information_schema.columns col where col.table_schema not in('pg_catalog', 'information_schema') order by col.table_schema, col.table_name, col.ordinal_position desc";
const database::all_column_query_field_name database::all_column_field_name = {
		"table_schema",
		"table_name",
		"column_name",
		"udt_name",
		"character_maximum_length",
		"numeric_precision",
		"numeric_scale",
		"datetime_precision",
		"column_default"
};

const std::string database::__key_column = "select col.table_schema, col.table_name, col.column_name from information_schema.columns col join information_schema.constraint_column_usage ccl on col.table_schema=ccl.table_schema and col.table_name=ccl.table_name and col.column_name=ccl.column_name join information_schema.table_constraints ts on ccl.constraint_name=ts.constraint_name where ts.constraint_type = 'PRIMARY KEY' and col.table_schema not in ('pg_catalog','information_schema')";
const database::key_column_query_field_name database::key_column_field_name = {
		"table_schema",
		"table_name",
		"column_name"
};

const std::string database::__referential = "select tc.table_schema, tc.table_name, kcu.column_name, ccu.table_schema as referred_table_schema, ccu.table_name as referred_table_name, ccu.column_name as referred_column_name from information_schema.table_constraints tc left join information_schema.key_column_usage kcu ON tc.constraint_catalog = kcu.constraint_catalog AND tc.constraint_schema = kcu.constraint_schema AND tc.constraint_name = kcu.constraint_name left join information_schema.referential_constraints rc ON tc.constraint_catalog = rc.constraint_catalog AND tc.constraint_schema = rc.constraint_schema AND tc.constraint_name = rc.constraint_name left join information_schema.constraint_column_usage ccu ON rc.unique_constraint_catalog = ccu.constraint_catalog AND rc.unique_constraint_schema = ccu.constraint_schema AND rc.unique_constraint_name = ccu.constraint_name where tc.constraint_type = 'FOREIGN KEY'";
const database::referential_query_field_name database::referential_field_name = {
		"table_schema",
		"table_name",
		"column_name",
		"referred_table_schema",
		"referred_table_name",
		"referred_column_name"
};

database::database(std::string databaseName, unsigned _cuncurrend_connection) throw (storage_exception&) : __remote_database(_cuncurrend_connection) {
	(databaseName != "" ? __databaseName = databaseName : throw storage_exception("Error creating database: you can not create a database with no name, it's used in storing operation.  Check the 'databaseName' paramether."));
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

void database::load_structure() throw (basic_exception&) {
	__schemasMap.clear();
//	unsigned int referential_id = __remote_database.exec_query(__referential);
	unsigned int key_column_id = __remote_database.exec_query(__key_column);
	unsigned int all_column_id = __remote_database.exec_query(__all_column);

	//creazione della struttura del database
	table& _all_column_table = __remote_database.get_result(all_column_id);
	_all_column_table.to_html("structure.html");
	create_structure(_all_column_table);


	//definizione delle colonne chiave
	table& _key_column_table = __remote_database.get_result(key_column_id);
	_key_column_table.to_html("key_column.html");
	define_key(_key_column_table);


	__remote_database.erase(all_column_id);
//	__remote_database.erase(key_column_id);

//	std::unique_ptr<table> _referential_table = __remote_database.get_result(referential_id);

//	__remote_database.erase(referential_id);
}

sqlType::type_base* database::column_type(std::string udt_name, std::string character_maximum_length, std::string numeric_precision, std::string numeric_scale) {
	if (udt_name == "varchar") {
		if (character_maximum_length.empty())
			return new sqlType::varchar;
		else
			return new sqlType::varchar(std::stoul(character_maximum_length));
	}
	if (udt_name == "bpchar") {
		if (character_maximum_length.empty())
			return new sqlType::character;
		else
			return new sqlType::character(std::stoul(character_maximum_length));
	}
	if (udt_name == "int2")
		return new sqlType::smallint;
	if (udt_name == "int4")
		return new sqlType::integer;
	if (udt_name == "int8")
		return new sqlType::bigint;
	if (udt_name == "float4")
		return new sqlType::real;
	if (udt_name == "float8")
		return new sqlType::double_precision;
	if (udt_name == "numeric")
		return new sqlType::numeric(std::stoul(numeric_precision), std::stoul(numeric_scale));
	if (udt_name == "bool")
		return new sqlType::boolean;
	if (udt_name == "date")
		return new sqlType::date;
	if (udt_name == "time")
		return new sqlType::time;
}

void database::create_structure(table& structure_table) {
	std::unique_ptr<std::list<unsigned long>> _all_column_table_tupleID = structure_table.internalID();
	for (std::list<unsigned long>::const_iterator it = _all_column_table_tupleID->begin(); it != _all_column_table_tupleID->end(); it++) {
		std::unique_ptr<std::unordered_map<std::string, std::string>> tuple = structure_table.current(*it);
		std::string schema_name = tuple->find(all_column_field_name.table_schema)->second;
		std::string table_name = tuple->find(all_column_field_name.table_name)->second;
		std::string column_name = tuple->find(all_column_field_name.column_name)->second;
		std::string udt_name = tuple->find(all_column_field_name.udt_name)->second;
		std::string character_maximum = tuple->find(all_column_field_name.character_maximum_length)->second;
		std::string numeric_precision = tuple->find(all_column_field_name.numeric_precision)->second;
		std::string numeric_scale = tuple->find(all_column_field_name.numeric_scale)->second;
		sqlType::type_base* type = column_type(udt_name, character_maximum, numeric_precision, numeric_scale);
		std::string default_value = tuple->find(all_column_field_name.column_default)->second;

		if (!find_schema(schema_name))
			add_schema(schema_name);
		schema& _schema = get_schema(schema_name);
		if (!_schema.find_table(table_name))
			_schema.add_table(table_name);
		table& _table = _schema.get_table(table_name);
		_table.add_column(column_name, type);
	}
}

void database::define_key(table& key_table) {
	std::unique_ptr<std::list<unsigned long>> _key_column_table_tupleID = key_table.internalID();
	for (std::list<unsigned long>::const_iterator it = _key_column_table_tupleID->begin(); it != _key_column_table_tupleID->end(); it++) {
		std::unique_ptr<std::unordered_map<std::string, std::string>> tuple = key_table.current(*it);
		std::string schema_name = tuple->find(key_column_field_name.table_schema)->second;
		std::string table_name = tuple->find(key_column_field_name.table_name)->second;
		std::string column_name = tuple->find(key_column_field_name.column_name)->second;
		get_schema(schema_name).get_table(table_name).get_column(column_name).is_key(true);
	}
}

void database::load_tuple() throw (basic_exception&) {
	for (std::unordered_map<std::string, schema>::iterator schema_it = __schemasMap.begin(); schema_it!=__schemasMap.end(); schema_it++) {
		std::unique_ptr<std::unordered_map<std::string, std::string>> load_commands = schema_it->second.load_command();
		for (std::unordered_map<std::string, std::string>::const_iterator commands_it = load_commands->begin(); commands_it != load_commands->end(); commands_it++) {
			std::cout <<"Loading " <<commands_it->first <<std::endl;
			std::cout <<"Query " <<commands_it->second <<std::endl;
			unsigned long query_id = __remote_database.exec_query(commands_it->second);
			table& _result = __remote_database.get_result(query_id);
			table& _table = schema_it->second.get_table(commands_it->first);
			_table.clear();
			std::unique_ptr<std::list<unsigned long>> tuple_id = _result.internalID();
			for (std::list<unsigned long>::const_iterator tuple_it = tuple_id->begin(); tuple_it != tuple_id->end(); tuple_it++)
				_table.insert( *_result.current(*tuple_it), record::inserting);
			__remote_database.erase(query_id);
		}
	}
}

std::unique_ptr<std::list<std::string>> database::commit() const throw (basic_exception&) {
	std::unique_ptr<std::list<std::string>> command_list = command_generator();
	std::unique_ptr<std::list<std::string>> log_list(new std::list<std::string>);

	return log_list;
}

std::unique_ptr<std::list<std::string>> database::command_generator() const throw () {
	std::unique_ptr<std::list<std::string>> list_ptr(new std::list<std::string>);
	for (std::unordered_map<std::string, schema>::const_iterator schema_it = __schemasMap.begin(); schema_it!=__schemasMap.end(); schema_it++) {
		std::unique_ptr<std::list<std::string>> tmp = schema_it->second.commit();
		list_ptr->splice(list_ptr->end(), *tmp);
	}
	return list_ptr;
}
