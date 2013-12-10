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

#ifndef __IWMGUI_LOGIN_DIALOG__
#define __IWMGUI_LOGIN_DIALOG__

#include <string>
#include <QDialog>
#include "database.hpp"

class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

namespace openDB {

/* login_dialog è la classe derivata da QDialog che implementa una finestra di dialogo attraverso la quale l'utente può inserire
 * le informazioni necessarie alla connessione al database.
 * Un oggetto login_dialog è composto, semplicemente, da 5 etichette e 5 caselle testuali attraverso il quale l'utente interagisce
 * con l'interfaccia. Due pulsanti sono previsti per permettere la conferma o l'annullamento delle operazioni;
 */
class login_dialog : public QDialog {

	Q_OBJECT	//la chiamata alla macro Q_OBJECT è necessaria per tutte le classi che definiscono signals e slots

public:

	login_dialog(database& _database, QWidget* parent = 0);

	~login_dialog ();

private :
	openDB::database& __database;

	QLabel* host_label;
	QLabel* port_label;
	QLabel* database_label;
	QLabel* user_label;
	QLabel* password_label;

	QLineEdit* host_edit;
	QLineEdit* port_edit;
	QLineEdit* database_edit;
	QLineEdit* user_edit;
	QLineEdit* password_edit;

	QPushButton* OK_button;
	QPushButton* Cancel_button;

	QVBoxLayout* label_layout;
	QVBoxLayout* edit_layout;
	QHBoxLayout* main_layout;

private slots:
	void try_to_connect();

};

};

#endif
