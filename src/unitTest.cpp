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
#include "connection.hpp"
#include <iostream>
using namespace std;

void print_list(const list<string>& _list);

const unsigned num_conn = 5;
const unsigned num_tuples = 50;

int main () {
	try {
		openDB::table _table("prova", "", 0, false, false);

		_table.add_column("boolean", new openDB::sqlType::boolean);
		_table.add_column("date", new openDB::sqlType::date);
		_table.add_column("time", new openDB::sqlType::time);

		unordered_map<string, string> value1 = {
			{"boolean", "true"},
			{"date", "20/01/1990"},
			{"time", "23:04"}
		};
		unordered_map<string, string> value2 = {
			{"boolean", "false"},
			{"date", "20/01/1990"},
			{"time", "23:4:0"}
		};
		unordered_map<string, string> value3 = {
			{"boolean", "true"},
			{"date", "20/1/1990"},
			{"time", "23:04:00"}
		};
		unordered_map<string, string> value4 = {
			{"boolean", "true"},
			{"date", "1990/1/22"},
			{"time", "23:04:00"}
		};
		unordered_map<string, string> value5 = {
			{"boolean", "true"},
			{"date", "1990-1-22"},
			{"time", "23:04:00"}
		};


		_table.insert(value1);
		_table.insert(value2);
		_table.insert(value3);
		_table.insert(value4);
		_table.insert(value5);

		_table.to_html("/tmp/prova.html");

	}
	catch (openDB::basic_exception& e) {cout <<e.what() <<endl;}
}


void print_list(const list<string>& _list) {
	for (list<string>::const_iterator it = _list.begin(); it!=_list.end(); it++)
		cout <<*it <<endl;
}
