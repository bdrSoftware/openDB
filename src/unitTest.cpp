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

		openDB::database _database(num_conn);
		_database.host("milky.no-ip.biz");
		_database.port("5432");
		_database.dbname("platinet_test");
		_database.user("platinet");
		_database.passwd("c0n0gel4t0");
		cout << "Connessione in corso..." <<endl;
		_database.connect();
		cout << "Connessione effettuata. " <<num_conn <<" connessioni attive." <<endl;
		cout << "Inizio caricamento struttura del database..." <<endl;
		_database.load_structure();
		cout << "Caricamento struttura del database completata!" <<endl;
		cout << "Inizio caricamento tuple..." <<endl;
		_database.load_tuple();
		cout << "Caricamento tuple completato!" <<endl;

		openDB::table& _table = _database["tipi_dato"]["tabella_varchar"];
		unique_ptr<list<string>> columns_name = _table.columns_name();
		cout <<"Comincio la generazione delle tuple per il test" <<endl;

		cout <<"Test effettuato con " <<num_tuples <<" tuple." <<endl;
		for (unsigned i = 0; i < num_tuples; i++) {
			unordered_map<string, string> values_map;
			for (list<string>::const_iterator it = columns_name->begin(); it != columns_name->end(); it++)
				values_map.emplace(*it, "valore colonna " + *it + ", riga " + to_string(i));
			_table.insert(values_map);
		}
		cout <<"Inizio delle operazioni di commit." <<endl;
		unique_ptr<list<unsigned long>> id_list = _database.commit_noblock();
		for (list<unsigned long>::const_iterator it = id_list->begin(); it != id_list->end(); it++) {
			cout << "In attesa del completamento della query " <<*it <<"...";
			while(!_database.executed(*it));
			cout << "Completata! \t";
			unique_ptr<unordered_map<string, string>> result_map = _database.get_result(*it).current();
			cout << result_map->find("result")->second <<endl;
			_database.erase(*it);
		}
		cout <<"Commit completato!" <<endl;
		cout << "Inizio caricamento tuple..." <<endl;
		_database.load_tuple();
		cout << "Caricamento tuple completato!" <<endl;
		_table.to_html("/tmp/tabella_varchar.html");

		list<unsigned long> select_list;
		for (unsigned i = 0; i < num_tuples; i++)
			select_list.push_back(_database.exec_query_noblock("select * from tipi_dato.tabella_varchar"));

		for (list<unsigned long>::const_iterator it = select_list.begin(); it != select_list.end(); it++) {
			cout <<"In attesa del completamento della query " <<*it <<"...";
			while(!_database.executed(*it));
			cout <<"\tCompletata!" <<endl;
		}
		
		_database["tipi_dato"]["tabella_tipi_dato"].to_html("/tmp/tabella_tipi_dato.html");
	}
	catch (openDB::basic_exception& e) {cout <<e.what() <<endl;}
}


void print_list(const list<string>& _list) {
	for (list<string>::const_iterator it = _list.begin(); it!=_list.end(); it++)
		cout <<*it <<endl;
}
