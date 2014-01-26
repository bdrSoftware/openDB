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

#ifndef __IWMGUI_UPDATE_TABLE__
#define __IWMGUI_UPDATE_TABLE__

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

class update_table_dialog : public QDialog {

	Q_OBJECT

public:
	update_table_dialog(table& _table, unsigned rowID, QWidget* parent = 0) throw (openDB::record_not_exists&);

	~update_table_dialog();

private:
	openDB::table& __table;
	unsigned long __rowID;

	struct iface {
		QLabel* label;
		QWidget* widget;
		QHBoxLayout* layout;
	};
	std::unordered_map<std::string, iface> widget_map;
	QWidget* create_widget(const openDB::column& column_ref, std::unordered_map<std::string, std::string>& value_map);
	
	QPushButton* button_update;
	QPushButton* button_cancel;
	QSpacerItem* button_central_spacer;
	QHBoxLayout* button_layout;
	
	QVBoxLayout* main_layout;
		
	std::string get_value(std::unordered_map<std::string, iface>::iterator& it);
	
private slots:
	void update();

};
};

#endif
