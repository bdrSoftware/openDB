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

#ifndef __IWMGUI_INSERT_TABLE__
#define __IWMGUI_INSERT_TABLE__

#include <QDialog>
#include "table.hpp"
#include <unordered_map>

class QWidget;
class QLabel;
class QSpacerItem;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

namespace openDB {

class insert_table_dialog : public QDialog {

	Q_OBJECT

public:
	insert_table_dialog(table& _table, QWidget* parent = 0);

	~insert_table_dialog();

private:
	openDB::table& __table;

	struct iface {
		QLabel* label;
		QWidget* widget;
		QHBoxLayout* layout;
	};
	std::unordered_map<std::string, iface> widget_map;
	static const unsigned vchar_limit = 60;
	QWidget* create_widget(openDB::sqlType::type_info column_info);

	QPushButton* button_insert;
	QPushButton* button_cancel;
	QSpacerItem* button_central_spacer;
	QHBoxLayout* button_layout;

	QVBoxLayout* main_layout;


	std::string get_value(std::unordered_map<std::string, iface>::iterator& it);

signals:
	void insert_confirmed();
	void insert_aborted();

private slots:
	void insert();

};
};

#endif
