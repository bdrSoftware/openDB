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
#include "connection.hpp"
using namespace openDB;

void connection::connect () throw (connection_error&) {
	std::string connection_string = "host='" + __host + "' port='" + __port + "' dbname='" + __dbname + "' user='" +  __user + "' password='" + __passwd + "'";
	__pgconnection = PQconnectdb(connection_string.c_str());
	if  (__pgconnection == 0)
		throw connection_error("Can not establish a connection: memory is insufficient.");
	else
		if (PQstatus(__pgconnection) == CONNECTION_BAD) {
			disconnect();
			throw connection_error ("Can not establish a connection: " + std::string(PQerrorMessage(__pgconnection)));
		}
}

void connection::disconnect () throw () {
	if (__pgconnection != 0) {
		PQfinish(__pgconnection);
		__pgconnection = 0;
	}
}

void connection::reset () throw () {
	if (__pgconnection != 0)
		PQreset(__pgconnection);
}



