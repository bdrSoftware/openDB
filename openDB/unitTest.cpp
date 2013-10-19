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
		_database.host("127.0.0.1");
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
	}
	catch (openDB::basic_exception& e) {cout <<e.what() <<endl;}
}


void print_list(const list<string>& _list) {
	for (list<string>::const_iterator it = _list.begin(); it!=_list.end(); it++)
		cout <<*it <<endl;
}
