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
#include "record.hpp"
#include "common.hpp"
using namespace openDB;

record::record (std::unordered_map<std::string, std::string>& valuesMap, std::unordered_map<std::string, column>& columnsMap, enum state _state) throw (basic_exception&) {
	validate_column_name(valuesMap, columnsMap);
	validate_columns_value(valuesMap, columnsMap);
	build_value_map(valuesMap, columnsMap);
	__state = _state;
	(__state != deleting ? __visible = true : __visible = false);
}

void record::update (std::unordered_map<std::string, std::string>& valuesMap, std::unordered_map<std::string, column>& columnsMap) throw (basic_exception&) {
	validate_column_name(valuesMap, columnsMap);
	validate_columns_value(valuesMap, columnsMap);
	switch (__state) {
		case empty :
		case inserting :
			__valueMap.clear();
			build_value_map(valuesMap, columnsMap);
			__state = inserting;
			break;
		case loaded :
		case updating :
			update_value_map(valuesMap, columnsMap);
			__state = updating;
			break;
		case deleting :
			break;
	}
}

std::unique_ptr<std::unordered_map<std::string, std::string>> record::current() const throw () {
	std::unique_ptr<std::unordered_map<std::string, std::string>> map_ptr(new std::unordered_map<std::string, std::string>);
	for (std::unordered_map<std::string, value>::const_iterator it = __valueMap.begin(); it != __valueMap.end(); it++)
		map_ptr->emplace(it->first, it->second.current);
	return map_ptr;
}

std::unique_ptr<std::unordered_map<std::string, std::string>> record::old() const throw () {
	std::unique_ptr<std::unordered_map<std::string, std::string>> map_ptr(new std::unordered_map<std::string, std::string>);
	for (std::unordered_map<std::string, value>::const_iterator it = __valueMap.begin(); it != __valueMap.end(); it++)
			map_ptr->emplace(it->first, it->second.old);
	return map_ptr;
}

std::streamoff record::size() const throw () {
	unsigned long size = 3 * sizeof(unsigned) * __valueMap.size() + 1;
	for (std::unordered_map<std::string, value>::const_iterator it = __valueMap.begin(); it != __valueMap.end(); it++)
		size += it->first.size() + it->second.current.size() + it->second.old.size();
	return (std::streamoff) size;
}

void record::write (std::fstream& stream) const	throw (storage_exception&) {
	stream.write(reinterpret_cast<const char*> (&__state), sizeof(enum state));
	stream.write(reinterpret_cast<const char*> (&__visible), sizeof(bool));
	unsigned num_of_elements = __valueMap.size();
	stream.write(reinterpret_cast <const char*> (&num_of_elements), sizeof (unsigned));
	for (std::unordered_map<std::string, value>::const_iterator it = __valueMap.begin(); it != __valueMap.end(); it++) {
		openDB::write(stream, it->first);
		openDB::write(stream, it->second.current);
		openDB::write(stream, it->second.old);
	}
}

void record::read (std::fstream& stream) throw (storage_exception&) {
	__valueMap.clear();
	stream.read(reinterpret_cast<char*> (&__state), sizeof(enum state));
	stream.read(reinterpret_cast<char*> (&__visible), sizeof(bool));
	unsigned num_of_elements = 0;
	stream.read(reinterpret_cast <char*> (&num_of_elements), sizeof (unsigned));
	for (unsigned i = 0; i < num_of_elements; i++) {
		std::string first;
		openDB::read(stream, first);
		std::string current;
		openDB::read(stream, current);
		std::string old;
		openDB::read(stream, old);
		__valueMap.emplace(first, value(current, old));
	}
}

void record::validate_column_name(std::unordered_map<std::string, std::string>& valueMap, std::unordered_map<std::string, column>& columnsMap) const throw (column_not_exists&) {
	for (std::unordered_map<std::string, std::string>::const_iterator valueMap_it = valueMap.begin(); valueMap_it != valueMap.end(); valueMap_it++)
		if (columnsMap.find(valueMap_it->first) == columnsMap.end())
			throw column_not_exists("'" + valueMap_it->first + "' column doesn't exists.");
}

void record::validate_columns_value(std::unordered_map<std::string, std::string>& valueMap, std::unordered_map<std::string, column>& columnsMap) const throw (data_exception&) {
	for (std::unordered_map<std::string, std::string>::const_iterator valueMap_it = valueMap.begin(); valueMap_it != valueMap.end(); valueMap_it++) {
		if (columnsMap.find(valueMap_it->first)->second.is_key() && valueMap_it->second == "")
				throw key_empty("Value for a key-column can not be null or empty!");
		columnsMap.find(valueMap_it->first)->second.validate_value(valueMap_it->second);
	}
}

void record::build_value_map(std::unordered_map<std::string, std::string>& valueMap, std::unordered_map<std::string, column>& columnsMap) throw (key_empty&) {
	for (std::unordered_map<std::string, column>::const_iterator columnsMap_it = columnsMap.begin(); columnsMap_it != columnsMap.end(); columnsMap_it++) {
		std::unordered_map<std::string, std::string>::const_iterator valueMap_it = valueMap.find(columnsMap_it->first);
		if (valueMap_it!=valueMap.end())
			__valueMap.emplace(valueMap_it->first, value(valueMap_it->second, ""));
		else
			if (columnsMap_it->second.is_key())
				throw key_empty("Value for a key-column can not be null or empty!");
			else
				__valueMap.emplace(columnsMap_it->first, value(columnsMap_it->second.default_value(), ""));
	}
}

void record::update_value_map(std::unordered_map<std::string, std::string>& valueMap, std::unordered_map<std::string, column>& columnsMap) throw () {
	for (std::unordered_map<std::string, column>::const_iterator columnsMap_it = columnsMap.begin(); columnsMap_it != columnsMap.end(); columnsMap_it++) {
		std::unordered_map<std::string, std::string>::const_iterator valueMap_it = valueMap.find(columnsMap_it->first);
		if (valueMap_it!=valueMap.end() && !columnsMap_it->second.is_key()) {
			__valueMap.find(valueMap_it->first)->second.old = __valueMap.find(valueMap_it->first)->second.current;
			__valueMap.find(valueMap_it->first)->second.current = valueMap_it->second;
		}
	}
}
