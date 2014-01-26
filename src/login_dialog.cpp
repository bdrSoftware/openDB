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

#include "login_dialog.hpp"
#include <QtGui>
using namespace openDB;

login_dialog::login_dialog(openDB::database& _database, QWidget* parent)
	: QDialog(parent), __database(_database)
{
	host_label = new QLabel("Host:");
	port_label = new QLabel("Porta:");
	database_label = new QLabel("Database:");
	user_label = new QLabel("User");
	password_label = new QLabel("Password:");

	host_edit = new QLineEdit;
	port_edit = new QLineEdit;
	database_edit = new QLineEdit;
	user_edit = new QLineEdit;
	password_edit = new QLineEdit;
	password_edit->setEchoMode(QLineEdit::Password);

	OK_button = new QPushButton("OK");
	connect(OK_button, SIGNAL(clicked()), this, SLOT(try_to_connect()));
	Cancel_button = new QPushButton("Cancel");
	connect(Cancel_button, SIGNAL(clicked()), this, SLOT(close()));


	label_layout = new QVBoxLayout;
	label_layout->addWidget(host_label);
	label_layout->addWidget(port_label);
	label_layout->addWidget(database_label);
	label_layout->addWidget(user_label);
	label_layout->addWidget(password_label);
	label_layout->addWidget(OK_button);

	edit_layout = new QVBoxLayout;
	edit_layout->addWidget(host_edit);
	edit_layout->addWidget(port_edit);
	edit_layout->addWidget(database_edit);
	edit_layout->addWidget(user_edit);
	edit_layout->addWidget(password_edit);
	edit_layout->addWidget(Cancel_button);

	main_layout = new QHBoxLayout;
	main_layout->addLayout(label_layout);
	main_layout->addLayout(edit_layout);
	setLayout(main_layout);

	setWindowTitle("Login");
	setFixedHeight(sizeHint().height());
	setFixedWidth(sizeHint().width());
}

login_dialog::~login_dialog ()
{
	delete host_label;
	delete port_label;
	delete database_label;
	delete user_label;
	delete password_label;

	delete host_edit;
	delete port_edit;
	delete database_edit;
	delete user_edit;
	delete password_edit;

	delete OK_button;
	delete Cancel_button;

	delete label_layout;
	delete edit_layout;
	delete main_layout;
}


void login_dialog::try_to_connect() {
	__database.host(host_edit->text().toStdString());
	__database.port(port_edit->text().toStdString());
	__database.dbname(database_edit->text().toStdString());
	__database.user(user_edit->text().toStdString());
	__database.passwd(password_edit->text().toStdString());

	try {
		__database.connect();
		__database.load_structure();
		__database.load_tuple();
		QMessageBox::warning(this, "", "Connessione effettuata correttamente.", QMessageBox::Ok);
		close();
	}
	catch (openDB::basic_exception& e) {
		QMessageBox::warning(this, "Generata eccezione durante il tentativo di connessione.", e.what().c_str(), QMessageBox::Ok);
	}
}
