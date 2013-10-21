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

void connection::connect () throw (remote_exception&) {
	std::string connection_string = "host='" + __host + "' port='" + __port + "' dbname='" + __dbname + "' user='" +  __user + "' password='" + __passwd + "'";
	__pgconnection = PQconnectdb(connection_string.c_str());
	if  (__pgconnection == 0)
		throw null_pointer("Can not establish a connection: memory is insufficient.");
	else
		if (PQstatus(__pgconnection) != CONNECTION_OK)
			throw connection_error ("Can not establish a connection: " + std::string(PQerrorMessage(__pgconnection)));
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


std::unique_ptr<table> connection::exec_query(unsigned long queryID, std::string command) const throw (basic_exception&) {
	if (__pgconnection == 0)
		throw connection_error("Connection not established!");

	PGresult* pgresult = PQexec(__pgconnection, command.c_str());
	if (pgresult != 0) {
		if (PQresultStatus(pgresult) ==  PGRES_COMMAND_OK || PQresultStatus(pgresult) == PGRES_TUPLES_OK)
			return process_result(queryID, pgresult);
		else
			throw query_execution(std::string(PQresStatus(PQresultStatus(pgresult))) + ": " + std::string(PQresultErrorMessage(pgresult)));
	}
	else
		throw null_pointer("Can not execute this query: memory is insufficient!");
}


std::unique_ptr<table> connection::process_result(unsigned long queryID, PGresult* pgresult) const throw (basic_exception&){
	std::unique_ptr<table> table_ptr(new table("table " + std::to_string(queryID), "", true, false));
	if (PQresultStatus(pgresult) ==  PGRES_COMMAND_OK) { // PGRES_COMMAND_OK is for commands that can never return rows (INSERT, UPDATE, etc.)
		table_ptr->add_column("result status", new sqlType::varchar());
		std::unordered_map<std::string, std::string> tmp;
		tmp.emplace("result status", std::string(PQresStatus(PGRES_COMMAND_OK)));
		table_ptr->load(tmp);
	}
	else {
		for (int col = 0; col < num_columns(pgresult); col++) //creazione delle colonne
			table_ptr->add_column(column_name(pgresult, col), new sqlType::varchar);

		for (int row = 0; row < num_tuples(pgresult); row++) { //riempimento delle tuple
			std::unordered_map<std::string, std::string> tmp;
			for (int col = 0; col < num_columns(pgresult); col++) //creazione della mappa per l'inserimento di una tupla
				tmp.emplace(column_name(pgresult, col), value(pgresult, row, col));
			table_ptr->load(tmp);
		}
	}
	return table_ptr;
}
