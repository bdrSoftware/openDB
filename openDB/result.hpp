/*
 * Copyright 2013 Salvatore Barone <salvator.barone@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef __OPENDB_RESULT_MANAGEMENT_HEADER__
#define __OPENDB_RESULT_MANAGEMENT_HEADER__

#include "exception.hpp"
#include "table.hpp"
#include "libpq-fe.h"
#include <string>

namespace openDB {
/*
 */
class result {
public:
	result (PGresult* pgresult) throw (remote_exception&) : __pgresult(pgresult), __result(0) {}

private:
	PGresult*	__pgresult;
	table*		__result;

	enum status {empty, commandOK, tuplesOK, nonFatal, fatal};
	enum status queryStatus	() const;
	std::string statusMsg () const;
	std::string errorMsg () const;

	int num_tuples () const;
	int num_columns	() const;
	std::string column_name	(int colNumber) const;
	std::string value (int row, int col) const;
	bool is_null (int row, int col) const;
};
};
#endif
