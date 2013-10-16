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

#ifndef __OPENDB_CONNECTION_HEADER__
#define __OPENDB_CONNECTION_HEADER__

#include "exception.hpp"
#include "libpq-fe.h"
#include <string>

namespace openDB {
/*
 */
class connection {
public:
	/*
	 */
	connection(std::string _host = "", std::string _port = "5432", std::string _dbname = "", std::string _user = "", std::string _passwd = "") throw () : 
		__host(_host), __port(_port), __dbname(_dbname), __user(_user), __passwd(_passwd), __pgconnection(0) {}

	~connection()
		{disconnect();}

	/*
	 */
	void host (std::string _host) throw ()
		{__host = _host;}
	std::string host () const throw ()
		{return __host;}
	void port (std::string _port) throw ()
		{__port = _port;}
	std::string port () const throw ()
		{return __port;}
	void dbname (std::string _dbname) throw ()
		{__dbname = _dbname;}
	std::string dbname () const throw ()
		{return __dbname;}
	void user (std::string _user) throw ()
		{__user = _user;}
	std::string user () const throw ()
		{return __user;}
	void passwd (std::string _passwd) throw ()
		{__passwd = _passwd;}
	std::string passwd () const throw ()
		{return __passwd;}

	/*
	 */
	void connect () throw (connection_error&);
	void disconnect () throw ();
	void reset () throw ();
	PGconn* operator() () const throw ()
		{return __pgconnection;}

private:
	/*
	 */
	std::string __host;
	std::string __port;
	std::string __dbname;
	std::string __user;
	std::string __passwd;
	PGconn* __pgconnection;
};
};
#endif
