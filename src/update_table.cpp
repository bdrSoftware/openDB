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

#include "update_table.hpp"
#include <QtGui>

using namespace openDB;

update_table_dialog::update_table_dialog(openDB::table& _table, unsigned rowID, QWidget* parent) throw (openDB::record_not_exists&)
	: QDialog(parent), __table(_table), __rowID(rowID)
{
	main_layout = new QVBoxLayout;
	std::unique_ptr<std::list<std::string>> columns_name = __table.columns_name();
	for (std::list<std::string>::const_iterator col_name_it = columns_name->begin(); col_name_it != columns_name->end(); col_name_it++) {
		std::unique_ptr<std::unordered_map<std::string, std::string>> current_value = __table.current(__rowID);
		iface iface_widget;
		iface_widget.label = new QLabel(col_name_it->c_str(), this);
		iface_widget.widget = create_widget(__table.get_column(*col_name_it), *current_value);
		iface_widget.layout = new QHBoxLayout;
		iface_widget.layout->addWidget(iface_widget.label);
		iface_widget.layout->addWidget(iface_widget.widget);
		main_layout->addLayout(iface_widget.layout);
		widget_map.insert(std::pair<std::string, iface>(*col_name_it, iface_widget));
	}
	button_update = new QPushButton("Modifica", this);
	button_cancel = new QPushButton("Annulla", this);
	connect(button_update, SIGNAL(clicked()), this, SLOT(update()));
	connect(button_cancel, SIGNAL(clicked()), this, SLOT(close()));
	button_central_spacer = new QSpacerItem(size().width() - button_update->size().width() - button_cancel->size().width(), button_update->size().height());
	button_layout = new QHBoxLayout;
	button_layout->addWidget(button_update);
	button_layout->addSpacerItem(button_central_spacer);
	button_layout->addWidget(button_cancel);
	main_layout->addLayout(button_layout);
	setLayout(main_layout);
}

update_table_dialog::~update_table_dialog() {
	for (std::unordered_map<std::string, iface>::iterator it = widget_map.begin(); it != widget_map.end(); it++) {
		delete it->second.label;
		delete it->second.widget;
		delete it->second.layout;
	}
	delete button_update;
	delete button_cancel;
	delete button_central_spacer;
	delete button_layout;
	delete main_layout;
}

QWidget* update_table_dialog::create_widget(const openDB::column& column_ref, std::unordered_map<std::string, std::string>& value_map) {
	openDB::sqlType::type_info column_info = column_ref.get_type_info();
	std::string value = value_map.find(column_ref.name())->second;
	QWidget* widget = 0;
	if (column_info.type_name == openDB::sqlType::boolean::type_name) {
		QCheckBox* tmp = new QCheckBox(this);
		if (value == openDB::sqlType::boolean::true_default)
			tmp->setCheckState(Qt::Checked);
		widget = tmp;
	}
	if (column_info.type_name == openDB::sqlType::date::type_name) {
		QDateEdit* tmp = new QDateEdit(this);
		QDate date = QDate::fromString(QString(value.c_str()), openDB::sqlType::date::qt4_format.c_str());
		tmp->setDate(date);
		widget = tmp;
	}
	if (column_info.type_name == openDB::sqlType::time::type_name) {
		QTimeEdit* tmp = new QTimeEdit(this);
		QTime time = QTime::fromString(QString(value.c_str()), openDB::sqlType::time::qt4_format.c_str());
		tmp->setTime(time);
		widget = tmp;
	}
	if (column_info.type_name == openDB::sqlType::character::type_name ||
	column_info.type_name == openDB::sqlType::varchar::type_name) {
		QTextEdit* tmp = new QTextEdit(this);
		tmp->setText(value.c_str());
		widget = tmp;
	}
	if (column_info.type_name == openDB::sqlType::smallint::type_name ||
	column_info.type_name == openDB::sqlType::integer::type_name ||
	column_info.type_name == openDB::sqlType::bigint::type_name ||
	column_info.type_name == openDB::sqlType::real::type_name ||
	column_info.type_name == openDB::sqlType::double_precision::type_name ||
	column_info.type_name == openDB::sqlType::numeric::type_name) {
		QLineEdit* tmp = new QLineEdit(this);
		tmp->setText(value.c_str());
		widget = tmp;
	}
	return widget;
}

std::string update_table_dialog::get_value(std::unordered_map< std::string, iface >::iterator& it) {
	openDB::sqlType::type_info column_info = __table.get_column(it->first).get_type_info();

	if (column_info.type_name == openDB::sqlType::boolean::type_name) {
		if (((QCheckBox*)it->second.widget)->isChecked())
			return openDB::sqlType::boolean::true_default;
		else
			return openDB::sqlType::boolean::false_default;
	}
	if (column_info.type_name == openDB::sqlType::date::type_name) {
		return ((QDateEdit*)it->second.widget)->date().toString(openDB::sqlType::date::qt4_format.c_str()).toStdString();
	}
	if (column_info.type_name == openDB::sqlType::time::type_name) {
		return ((QTimeEdit*)it->second.widget)->time().toString(openDB::sqlType::time::qt4_format.c_str()).toStdString();
	}
	if (column_info.type_name == openDB::sqlType::varchar::type_name ||
	column_info.type_name == openDB::sqlType::character::type_name) {
		return ((QTextEdit*)it->second.widget)->toPlainText().toStdString();
	}
	if (column_info.type_name == openDB::sqlType::smallint::type_name ||
	column_info.type_name == openDB::sqlType::integer::type_name ||
	column_info.type_name == openDB::sqlType::bigint::type_name ||
    column_info.type_name == openDB::sqlType::real::type_name ||
    column_info.type_name == openDB::sqlType::double_precision::type_name ||
    column_info.type_name == openDB::sqlType::numeric::type_name) {
		return ((QLineEdit*)it->second.widget)->text().toStdString();
	}
	return "";
}

void update_table_dialog::update() {
	std::unordered_map<std::string, std::string> update_map;
	for (std::unordered_map<std::string, iface>::iterator it = widget_map.begin(); it != widget_map.end(); it++)
		update_map.insert(std::pair<std::string, std::string>(it->first, get_value(it)));

	try{
		__table.update(__rowID, update_map);
		close();
	}
	catch (openDB::basic_exception& e) {
		const char* msg = e.what().c_str();
		QMessageBox::warning(this, "Generata eccezione durante la verifica dei valori", msg, QMessageBox::Ok);
	}
}
