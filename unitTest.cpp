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

#include <QApplication>
#include <QtGui>
#include <unordered_map>
#include "database.hpp"
#include "login_dialog.hpp"
#include "insert_table.hpp"
#include "update_table.hpp"
using namespace std;


int main(int argc, char* argv[])
{
	QApplication application(argc, argv);

	try {

		openDB::database _database;
		_database.host("milky.no-ip.biz");
		_database.port("5432");
		_database.dbname("platinet_test");
		_database.user("platinet");
		_database.passwd("c0n0gel4t0");

		_database.connect();
		_database.load_structure();
		_database.load_tuple();

		openDB::table& _table = _database["public"]["fornitori"];

		openDB::insert_table_dialog* _update_dialog = new openDB::insert_table_dialog(_table);
		_update_dialog->show();
		application.exec();
		_table.to_html("/tmp/prova.html");

	}
	catch (openDB::basic_exception& e) {
		QMessageBox::warning(0, "Generata eccezione non gestita.", e.what().c_str(), QMessageBox::Ok);

	}
}
