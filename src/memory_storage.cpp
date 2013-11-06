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

#include "memory_storage.hpp"
using namespace openDB;

std::unique_ptr<std::list<unsigned long>> memory_storage::internalID () const throw () {
	std::unique_ptr<std::list<unsigned long>> list_ptr (new std::list<unsigned long>);
	for (std::unordered_map<unsigned long, record>::const_iterator it = __recordMap.begin(); it!= __recordMap.end(); it++)
		list_ptr->push_back(it->first);
	return list_ptr;
}

unsigned long memory_storage::insert (std::unordered_map<std::string, std::string>& valuesMap, std::unordered_map<std::string, column>& columnsMap, enum record::state _state) throw (basic_exception&) {
	record _record(valuesMap, columnsMap, _state);
	__recordMap.insert(std::pair<unsigned long, record>(__lastKey, _record));
	return __lastKey++;
}

void memory_storage::erase (unsigned long ID) throw (storage_exception&) {
	std::unordered_map<unsigned long, record>::iterator it = __recordMap.find(ID);
	if (it != __recordMap.end())
		__recordMap.erase(it);
	else
		throw record_not_exists("There is no record with " + std::to_string(ID) + " id.");
}

const record& memory_storage::get_record(unsigned long ID) const throw (storage_exception&) {
	std::unordered_map<unsigned long, record>::const_iterator it = __recordMap.find(ID);
	if (it != __recordMap.end())
		return it->second;
	else
		throw record_not_exists("There is no record with " + std::to_string(ID) + " id.");
}

record& memory_storage::get_record(unsigned long ID) throw (storage_exception&) {
	std::unordered_map<unsigned long, record>::iterator it = __recordMap.find(ID);
	if (it != __recordMap.end())
		return it->second;
	else
		throw record_not_exists("There is no record with " + std::to_string(ID) + " id.");
}
