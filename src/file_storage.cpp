/* Copyright 2013-2014 Salvatore Barone <salvator.barone@gmail.com>
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
#include "file_storage.hpp"
using namespace openDB;

file_storage::file_storage(std::string fileName) throw (file_creation&) : storage(), __fileName(fileName), __trashID(0) {
	std::fstream file;
	file.open(__fileName.c_str(), std::ios::binary|std::ios::out);
	if(!file.is_open())
		throw file_creation("Error: '" + __fileName + "' can not be created!");
	file.close();
}

std::unique_ptr<std::list<unsigned long>> file_storage::internalID () const throw () {
	std::unique_ptr<std::list<unsigned long>> ptr(new std::list<unsigned long>);
	for (std::unordered_map <unsigned long, segment>::const_iterator it = __recordMap.begin(); it != __recordMap.end(); it++)
		ptr -> push_back(it -> first);
	return ptr;
}

void file_storage::clear () throw () {
	__recordMap.clear();
	__trash.clear();
	__lastKey = 0;
	__trashID = 0;
}

unsigned long file_storage::insert (std::unordered_map<std::string, std::string>& valuesMap, std::unordered_map<std::string, column>& columnsMap, enum record::state _state) throw (basic_exception&) {
	record _record(valuesMap, columnsMap, _state);
	unsigned long free_space = recycle(_record.size());
	if (free_space == 0)
		append(_record, __recordMap[__lastKey]);
	else {
		__recordMap[__lastKey + 1] = popTrash(free_space, _record.size());
		write(_record, __recordMap[__lastKey]);
	}
	return __lastKey++;
}

void file_storage::update (unsigned long ID, std::unordered_map<std::string, std::string>& valuesMap, std::unordered_map<std::string, column>& columnsMap) throw (basic_exception&) {
	std::unique_ptr<record> record_ptr = get_record(ID);
	record_ptr -> update(valuesMap, columnsMap);
	pushTrash(__recordMap[ID]);
	unsigned long free_space = recycle(record_ptr -> size());
	if (free_space == 0)
		append(*record_ptr, __recordMap[ID]);
	else {
		__recordMap[ID] = popTrash(free_space, record_ptr->size());
		write(*record_ptr, __recordMap[ID]);
	}
}

void file_storage::cancel (unsigned long ID) throw (storage_exception&) {
	std::unique_ptr<record> tuple_ptr = get_record(ID);
	tuple_ptr -> cancel();
	write(*tuple_ptr, __recordMap[ID]);
}

void file_storage::erase (unsigned long ID) throw (storage_exception&) {
	std::unordered_map<unsigned long, segment>::const_iterator record_it = __recordMap.find(ID);
	if (record_it != __recordMap.end()) {
		pushTrash(record_it ->second);
		__recordMap.erase(record_it);
	}
	else
		throw record_not_exists("There is no record with " + std::to_string(ID) + " id.");
}

std::unique_ptr<record>	file_storage::get_record (unsigned long ID) const throw (storage_exception&) {
	std::unordered_map<unsigned long, segment>::const_iterator record_it = __recordMap.find(ID);
	if (record_it != __recordMap.end()) {
		std::unique_ptr<record> ptr(new record);
		read(*ptr, record_it->second);
		return ptr;
	}
	else
		throw record_not_exists("There is no record with " + std::to_string(ID) + " id.");
}

void file_storage::write (const record& _record, const segment _segment) const throw (storage_exception&) {
	std::fstream file;
	file.open(__fileName.c_str(), std::ios::binary|std::ios::in|std::ios::out);
	if(!file.is_open())
		throw file_open("Error: '" + __fileName + "' can not be opened!");
	file.seekp(_segment.begin);
	_record.write(file);
	if (file.tellg() != _segment.end + (std::streampos)1)
		throw io_error("I/O error during write: '" + __fileName + "' may be corrupt or there may be a bug in the program!");
	file.close();
}

void file_storage::append (const record& _record, segment& _segment) throw (storage_exception&) {
	std::fstream file;
	file.open(__fileName.c_str(), std::ios::binary|std::ios::in|std::ios::out|std::ios::ate);
	if(!file.is_open())
		throw file_open("Error: '" + __fileName + "' can not be opened!");
	_segment.begin = file.tellp();
	_record.write(file);
	_segment.end = file.tellp() - (std::streamoff)1;
	if (_record.size() != _segment.size())
		throw io_error("I/O error during append: '" + __fileName + "' may be corrupt or there may be a bug in the program!");
	file.close();
}

void file_storage::read (record& _record, const segment _segment) const throw (storage_exception&) {
	std::fstream file;
	file.open(__fileName.c_str(), std::ios::binary|std::ios::in|std::ios::out);
	if(!file.is_open())
		throw file_open("Error: '" + __fileName + "' can not be opened!");
	file.seekg(_segment.begin);
	_record.read(file);
	if ((_record.size() != _segment.size()) | (file.tellg() != _segment.end + (std::streamoff)1))
		throw io_error("I/O error during read: '" + __fileName + "' may be corrupt or there may be a bug in the program!");
	file.close();
}

void file_storage::pushTrash (segment _segment) {
	bool neighbor = false;
	std::unordered_map <unsigned long, segment>::iterator it = __trash.begin();
	if (!__trash.empty())
		while (it != __trash.end() && !neighbor) {	//se il segmento che si aggiunge al cestino è adiacente ad uno già esistente, allora il segmento nel cestino viene
													//opportunamente esteso
			if (it->second.end + (std::streamoff)1 == _segment.begin) {
				it->second.end = _segment.end;
				neighbor = true;
			}
			if (it->second.begin - (std::streamoff)1 == _segment.end) {
				it->second.begin = _segment.begin;
				neighbor = true;
			}
			it++;
		}
	if (!neighbor) {							//altrimenti viene aggiunto un nuovo segmento nel cestino
		__trash.insert(std::pair<unsigned long,segment>(__trashID + 1, _segment));
		++__trashID;
	}
}

unsigned long file_storage::recycle (std::streamoff byte) const {
	unsigned long index = 0;
	if (!__trash.empty()) {
		std::unordered_map <unsigned long, segment>::const_iterator it = __trash.begin();
		while (index == 0 && it != __trash.end())
			if (it -> second.size() >= byte)
				index = it -> first;
			else
				it++;
	}
	return index;
}

file_storage::segment file_storage::popTrash (unsigned long id, std::streamoff byte) {
	std::unordered_map<unsigned long, segment>::iterator it = __trash.find(id);
	segment _segment;
	_segment.begin = it->second.begin;
	_segment.end = _segment.begin + byte - (std::streamoff)1;
	it->second.begin = _segment.end + (std::streamoff)1;
	if (it->second.size() == 0)
		__trash.erase(it);
	return _segment;
}
