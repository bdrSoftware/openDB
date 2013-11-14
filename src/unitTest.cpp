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
		openDB::database _database;
		_database.host("192.168.1.4");
		_database.port("5432");
		_database.dbname("platinet_test");
		_database.user("platinet");
		_database.passwd("c0n0gel4t0");

		_database.connect();
		_database.load_structure();
		_database.load_tuple();

		openDB::table& _table = _database["public"]["fornitori"];

		unique_ptr<list<string>> column_name = _table.columns_name();
		for (list<string>::const_iterator it = column_name->begin(); it != column_name->end(); it++)
			cout <<*it <<endl;

		_table.to_html("/tmp/prova.html");

	}
	catch (openDB::basic_exception& e) {cout <<e.what() <<endl;}
}


void print_list(const list<string>& _list) {
	for (list<string>::const_iterator it = _list.begin(); it!=_list.end(); it++)
		cout <<*it <<endl;
}
