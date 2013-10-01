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

#include "common.hpp"

std::unique_ptr<std::list <std::string>> openDB::tokenize (std::string _string, char sep) throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list <std::string>);
	std::string token;
	bool finished = false;
	std::size_t tabPos = 0;
	do {
		tabPos = _string.find_first_of(sep, 0);
		switch (tabPos) {
		case std::string::npos:
			token = _string;
			finished = true;
			break;
		case 0 :
			token = "";
			_string.erase(0, 1);
			break;
		default:
			token = _string.substr(0, tabPos);
			_string.erase(0, tabPos + 1);
		}
		list_ptr-> push_back(token);
	} while(!finished);
	return list_ptr;
}

void openDB::write (std::fstream& stream, const std::string& _string) throw () {
	unsigned length;
	length = _string.size();
	stream.write(reinterpret_cast <const char*> (&length), sizeof (unsigned));
	stream.write(_string.c_str(), length);
}

void openDB::read (std::fstream& stream, std::string& _string) throw () {
	unsigned length;
	char* tmp;
	stream.read(reinterpret_cast <char*> (&length), sizeof (unsigned));
	tmp = new char [length + 1];
	stream.read(tmp, length);
	_string = std::string (tmp, length);
	delete [] tmp;
}

void openDB::write (std::fstream& stream, const std::list <std::string>& _list) throw () {
	unsigned elementNumber = _list.size();
	stream.write(reinterpret_cast<const char*> (&elementNumber), sizeof(unsigned));
	for (std::list <std::string>::const_iterator it = _list.begin(); it != _list.end(); it++)
		write(stream, *it);
}

void openDB::read (std::fstream& stream, std::list <std::string>& _list) throw () {
	unsigned elementNumber;
	stream.read(reinterpret_cast <char*> (&elementNumber), sizeof(unsigned));
	std::string _string;
	for (unsigned i = 0; i < elementNumber; i++) {
		read(stream, _string);
		_list.push_back(_string);
	}
}
