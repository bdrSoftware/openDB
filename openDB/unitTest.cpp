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

int main () {
	try {

		openDB::database _database;
		_database.host("192.168.1.4");
		_database.port("5432");
		_database.dbname("platinet_test");
		_database.user("platinet");
		_database.passwd("c0n0gel4t0");

		_database.connect();
		cout << "Connessione effettuata" <<endl;
		cout << "Inizio caricamento struttura del database..." <<endl;
		_database.load_structure();
		cout << "Caricamento struttura del database completata!" <<endl;
		cout << "Inizio caricamento tuple..." <<endl;
		_database.load_tuple();
		cout << "Caricamento tuple completato!" <<endl;

		openDB::table& _table = _database["tipi_dato"]["tabella_varchar"];
		unique_ptr<list<string>> columns_name = _table.columns_name();
		cout <<"Comincio la generazione delle tuple per il test" <<endl;
		const unsigned num_tuples = 5000;
		cout <<"Test effettuato con " <<num_tuples <<" tuple." <<endl;
		for (unsigned i = 0; i < num_tuples; i++) {
			unordered_map<string, string> values_map;
			for (list<string>::const_iterator it = columns_name->begin(); it != columns_name->end(); it++)
				values_map.emplace(*it, "valore colonna " + *it + ", riga " + to_string(i));
			_table.insert(values_map);
		}
		cout <<"Inizio delle operazioni di commit." <<endl;
		unique_ptr<list<unsigned long>> id_list = _database.commit();
		cout <<"In attesa del completamento del Commit..." <<endl;
		for (list<unsigned long>::const_iterator it = id_list->begin(); it != id_list->end(); it++)
			while(!_database.executed(*it));
		cout <<"Commit completato!" <<endl;
		cout << "Inizio caricamento tuple..." <<endl;
		_database.load_tuple();
		cout << "Caricamento tuple completato!" <<endl;
		_table.to_html("tabella_varchar.html");
	}
	catch (openDB::basic_exception& e) {cout <<e.what() <<endl;}
}


void print_list(const list<string>& _list) {
	for (list<string>::const_iterator it = _list.begin(); it!=_list.end(); it++)
		cout <<*it <<endl;
}
