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
#include "queryAttribute.hpp"
using namespace openDB;

std::string query_attribute::compareOperator() const throw () {
	switch (__compare_operator) {
	case more:			return ">";
	case moreEqual:		return ">=";
	case equal:			return "=";
	case disequal:		return "!=";
	case like:			return "like";
	case notLike:		return "not like";
	case lessEqual:		return "<=";
	case less:			return "<";
	case in:			return "in";
	case notIn:			return "not in";
	default :			return "";
	};
}

std::string query_attribute::orderMode () const throw () {
	if (__order_mode == asc)
		return "asc";
	else
		return "desc";
}


