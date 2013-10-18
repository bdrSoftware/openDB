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

#ifndef __OPENDB_EXCEPTION_HEADER__
#define __OPENDB_EXCEPTION_HEADER__

#include <string>

namespace openDB {

/*
 */
class basic_exception {
public:
		basic_exception (const std::string& __message) : message(__message) {}
		std::string what() const {return message;}
private:
		std::string message;
};

/*
 */
class data_exception : public basic_exception{
public:
	data_exception (const std::string& __message) : basic_exception(__message) {}
};

/*
 */
class access_exception : public basic_exception {
public:
	access_exception (const std::string& __message) : basic_exception(__message) {}
};

/*
 */
class storage_exception : public basic_exception {
public:
	storage_exception (const std::string& __message) : basic_exception(__message) {}
};

/*
 */
class remote_exception : public basic_exception {
public:
	remote_exception (const std::string& __message) : basic_exception(__message) {}
};

/*
 */
class key_empty : public data_exception {
public:
	key_empty (const std::string& __message) : data_exception(__message) {}
};

/*
 */
class invalid_argument : public data_exception {
public:
	invalid_argument (const std::string& __message) : data_exception(__message) {}
};

/*
 */
class ambiguous_value : public data_exception {
public:
	ambiguous_value (const std::string& __message) : data_exception(__message) {}
};

/*
 */
class invalid_date : public data_exception {
public:
	invalid_date (const std::string& __message) : data_exception(__message) {}
};

/*
 */
class invalid_time : public data_exception {
public:
	invalid_time (const std::string& __message) : data_exception(__message) {}
};

/*
 */
class out_of_boud : public data_exception {
public:
	out_of_boud (const std::string& __message) : data_exception(__message) {}
};

/*
 */
class value_too_long : public data_exception {
public:
	value_too_long (const std::string& __message) : data_exception(__message) {}
};


/*
 */
class column_exists : public access_exception {
public:
	column_exists (const std::string& __message) : access_exception(__message) {}
};

/*
 */
class column_not_exists : public access_exception {
public:
	column_not_exists (const std::string& __message) : access_exception(__message) {}
};

/*
 */
class table_not_exists : public access_exception {
public:
	table_not_exists (const std::string& __message) : access_exception(__message) {}
};

/*
 */
class table_exists : public access_exception {
public:
	table_exists (const std::string& __message) : access_exception(__message) {}
};

/*
 */
class schema_not_exists : public access_exception {
public:
	schema_not_exists (const std::string& __message) : access_exception(__message) {}
};

/*
 */
class schema_exists : public access_exception {
public:
	schema_exists (const std::string& __message) : access_exception(__message) {}
};
/*
 */
class record_not_exists : public storage_exception {
public:
	record_not_exists (const std::string& __message) : storage_exception(__message) {}
};
/*
 */
class file_creation : public storage_exception {
public:
	file_creation (const std::string& __message) : storage_exception(__message) {}
};
/*
 */
class file_open : public storage_exception {
public:
	file_open (const std::string& __message) : storage_exception(__message) {}
};
/*
 */
class io_error : public storage_exception {
public:
	io_error (const std::string& __message) : storage_exception(__message) {}
};

/*
 */
class connection_error : public remote_exception {
public:
	connection_error (const std::string& __message) : remote_exception(__message) {}
};
/*
 */
class query_execution : public remote_exception {
public:
	query_execution (const std::string& __message) : remote_exception(__message) {}
};
/*
 */
class null_pointer : public remote_exception {
public:
	null_pointer (const std::string& __message) : remote_exception(__message) {}
};
/*
 */
class result_exception : public remote_exception {
public:
	result_exception (const std::string& __message) : remote_exception(__message) {}
};

};	/* end of openDB namespace */

#endif
