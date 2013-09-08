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
#include <typeinfo>
using namespace std;

void print_list(const list<string>& _list);

int main () {
	try {
		openDB::database _database("unit_test");
		_database.add_schema("schema_prova1");
		_database.add_schema("schema_prova2");
		_database.add_schema("schema_prova3");

		unique_ptr<list<string>> _list = _database.schemas_name();
		print_list(*_list);

		_database.get_schema("schema_prova1").add_table("prova_tipi");
		_database.get_schema("schema_prova1").add_table("prova_tipi2");
		_list = _database.get_schema("schema_prova1").tables_name();
		print_list(*_list);
		
		try {_database.get_schema("schema_prova1").add_table("prova_tipi");}
		catch (openDB::basic_exception& e) {cout <<e.what() <<endl;}
		
		openDB::table& _table = _database.get_schema("schema_prova1").get_table("prova_tipi");
		
		_table.add_column("colonna_boolean", new openDB::sqlType::boolean);
		_table.add_column("colonna_date", new openDB::sqlType::date);
		_table.add_column("colonna_time", new openDB::sqlType::time);
		_table.add_column("colonna_character", new openDB::sqlType::character(10));
		_table.add_column("colonna_varchar", new openDB::sqlType::varchar(10));
		_table.add_column("colonna_smallint", new openDB::sqlType::smallint);
		_table.add_column("colonna_integer", new openDB::sqlType::integer);
		_table.add_column("colonna_bigint", new openDB::sqlType::bigint);
		_table.add_column("colonna_real", new openDB::sqlType::real);
		_table.add_column("colonna_double", new openDB::sqlType::double_precision);
		_table.add_column("colonna_numeric", new openDB::sqlType::numeric(6, 2));

		_list = _database.get_schema("schema_prova1").get_table("prova_tipi").columns_name();
		print_list(*_list);

	}
	catch (openDB::basic_exception& e) {cout <<e.what() <<endl;}
}


void print_list(const list<string>& _list) {
	for (list<string>::const_iterator it = _list.begin(); it!=_list.end(); it++)
		cout <<*it <<endl;
}
