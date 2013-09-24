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
#include "memory_storage.hpp"
#include "file_storage.hpp"
#include "database.hpp"
#include <iostream>
#include <typeinfo>
using namespace std;

void print_list(const list<string>& _list);

int main () {
	try {
		openDB::table _table("tabella", "/home/ssaa/");

		_table.add_column("tipo_smallint", new openDB::sqlType::smallint);
		_table.add_column("tipo_integer", new openDB::sqlType::integer);
		_table.add_column("tipo_bigint", new openDB::sqlType::bigint);
		_table.add_column("tipo_real", new openDB::sqlType::real);
		_table.add_column("tipo_double", new openDB::sqlType::double_precision);
		_table.add_column("tipo_numeric", new openDB::sqlType::numeric(10,2));
		_table.add_column("tipo_date", new openDB::sqlType::date);
		_table.add_column("tipo_time", new openDB::sqlType::time);
		_table.add_column("tipo_varchar", new openDB::sqlType::varchar(15));
		_table.add_column("tipo_character", new openDB::sqlType::character(15), true);
		_table.add_column("tipo_boolean", new openDB::sqlType::boolean);

		unordered_map<string, string> valueMap1;
		valueMap1.emplace("tipo_smallint", "1");
		valueMap1.emplace("tipo_integer", "1");
		valueMap1.emplace("tipo_bigint", "1");
		valueMap1.emplace("tipo_real", "1");
		valueMap1.emplace("tipo_double", "1");
		valueMap1.emplace("tipo_numeric", "1");
		valueMap1.emplace("tipo_date", "1/1/2001");
		valueMap1.emplace("tipo_time", "11:11:11");
		valueMap1.emplace("tipo_varchar", "uno");
		valueMap1.emplace("tipo_character", "uno");
		valueMap1.emplace("tipo_boolean", "true");


		unordered_map<string, string> valueMap2;
		valueMap2.emplace("tipo_smallint", "2");
		valueMap2.emplace("tipo_integer", "2");
		valueMap2.emplace("tipo_bigint", "2");
		valueMap2.emplace("tipo_real", "2");
		valueMap2.emplace("tipo_double", "2");
		valueMap2.emplace("tipo_numeric", "2");
		valueMap2.emplace("tipo_date", "22/02/2002");
		valueMap2.emplace("tipo_time", "22:22:22");
		valueMap2.emplace("tipo_varchar", "due");
		valueMap2.emplace("tipo_character", "due");
		valueMap2.emplace("tipo_boolean", "true");

		unordered_map<string, string> valueMap3;
		valueMap3.emplace("tipo_smallint", "3");
		valueMap3.emplace("tipo_integer", "3");
		valueMap3.emplace("tipo_bigint", "3");
		valueMap3.emplace("tipo_real", "3");
		valueMap3.emplace("tipo_double", "3");
		valueMap3.emplace("tipo_numeric", "3");
		valueMap3.emplace("tipo_date", "3/03/2003");
		valueMap3.emplace("tipo_time", "03:03:03");
		valueMap3.emplace("tipo_varchar", "tre");
		valueMap3.emplace("tipo_character", "tre");
		valueMap3.emplace("tipo_boolean", "true");


		unordered_map<string, string> valueMap4;
		valueMap4.emplace("tipo_smallint", "4");
		valueMap4.emplace("tipo_integer", "4");
		valueMap4.emplace("tipo_bigint", "4");
		valueMap4.emplace("tipo_real", "4");
		valueMap4.emplace("tipo_double", "4");
		valueMap4.emplace("tipo_numeric", "4");
		valueMap4.emplace("tipo_date", "4/4/2004");
		valueMap4.emplace("tipo_time", "4:4:40");
		valueMap4.emplace("tipo_varchar", "quattro");
		valueMap4.emplace("tipo_character", "quattro");
		valueMap4.emplace("tipo_boolean", "true");



		unsigned long key1 = _table.insert(valueMap1, openDB::record::loaded);
		unsigned long key2 = _table.insert(valueMap2, openDB::record::loaded);
		unsigned long key3 = _table.insert(valueMap3, openDB::record::loaded);
		unsigned long key4 = _table.insert(valueMap4, openDB::record::loaded);

		cout <<"key1 = " <<key1 <<endl;
		cout <<"key2 = " <<key2 <<endl;
		cout <<"key3 = " <<key3 <<endl;
		cout <<"key4 = " <<key4 <<endl;

		_table.cancel(key2);
		_table.update(key3, valueMap2);

		unique_ptr<list<unsigned long>> key_ptr = _table.internalID();
		for (list<unsigned long>::const_iterator key_it = key_ptr->begin(); key_it != key_ptr->end(); key_it++) {
			cout <<*key_it <<endl;
			unique_ptr<unordered_map<std::string, std::string>> key_value = _table.current(*key_it);
			for (unordered_map<std::string, std::string>::const_iterator key_value_it = key_value->begin(); key_value_it != key_value->end(); key_value_it++)
				cout <<"\t\t" <<key_value_it->first <<"=" <<key_value_it->second <<endl;
		}

		_table.to_html("prova.html");

	}
	catch (openDB::basic_exception& e) {cout <<e.what() <<endl;}
}


void print_list(const list<string>& _list) {
	for (list<string>::const_iterator it = _list.begin(); it!=_list.end(); it++)
		cout <<*it <<endl;
}
