/* Copyright 2013-2014 Salvatore Barone <salvator.barone@gmail.com>
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
#include "dbms.hpp"
using namespace openDB;

dbms::dbms(unsigned _cuncurrend_connection) throw (remote_exception&) :
	num_of_connection(_cuncurrend_connection), connection_array(0),  num_of_free_connection(num_of_connection), queryID(0) {

	if (num_of_connection == 0)
		num_of_connection = 1;

	connection_array = new connection_mtx[num_of_connection];
}

dbms::~dbms() {
	delete [] connection_array;
}

void dbms::host (std::string _host) throw () {
	for (unsigned i = 0; i < num_of_connection; i++)
		connection_array[i].conn.host(_host);
}

void dbms::port (std::string _port) throw () {
	for (unsigned i = 0; i < num_of_connection; i++)
		connection_array[i].conn.port(_port);
}

void dbms::dbname (std::string _dbname) throw () {
	for (unsigned i = 0; i < num_of_connection; i++)
		connection_array[i].conn.dbname(_dbname);
}

void dbms::user (std::string _user) throw () {
	for (unsigned i = 0; i < num_of_connection; i++)
		connection_array[i].conn.user(_user);
}

void dbms::passwd (std::string _passwd) throw () {
	for (unsigned i = 0; i < num_of_connection; i++)
		connection_array[i].conn.passwd(_passwd);
}

void dbms::connect() throw (remote_exception&) {
	for (unsigned i = 0; i < num_of_connection; i++)
		connection_array[i].conn.connect();
}

void dbms::reset() throw () {
	for (unsigned i = 0; i < num_of_connection; i++)
		connection_array[i].conn.reset();
}

void dbms::disconnect() throw () {
	for (unsigned i = 0; i < num_of_connection; i++)
		connection_array[i].conn.disconnect();
}

unsigned long dbms::exec_query(std::string command) throw (basic_exception&) {
	++queryID;
	query_map.insert(std::pair<long unsigned, query>(queryID, query(command)));
	execute_query(queryID);
	return queryID;
}

unsigned long dbms::exec_query_noblock(std::string command) throw (basic_exception&) {
	++queryID;
	query_map.insert(std::pair<unsigned long, query>(queryID, query(command)));
	std::thread thr(&dbms::execute_query, std::ref(*this), queryID);
	thr.detach();
	return queryID;
}

void dbms::erase (unsigned long resultID) throw (result_exception&) {
	std::unordered_map<unsigned long, query>::iterator it = query_map.find(resultID);
		if (it == query_map.end())
			throw result_exception("Result does not exists!");
		else
			query_map.erase(it);
}

void dbms::execute_query (unsigned long id) throw () {
	std::unordered_map<unsigned long, query>::iterator it = query_map.find(id);
	connection_free_mtx.lock();
	while (num_of_free_connection == 0)
		connection_free_cv.wait(connection_free_mtx);
	num_of_free_connection--;
	connection_free_mtx.unlock();

	unsigned free_connection_index = 0;
	while (free_connection_index < num_of_connection && !connection_array[free_connection_index].busy_mtx.try_lock())
		free_connection_index++;

	it->second.result_table = connection_array[free_connection_index].conn.exec_query(id, it->second.command);
	it->second.completed = true;

	connection_array[free_connection_index].busy_mtx.unlock();

	connection_free_mtx.lock();
	num_of_free_connection++;
	if (num_of_free_connection == 1)	connection_free_cv.notify_all();
	connection_free_mtx.unlock();
}

std::unordered_map<unsigned long, dbms::query>::const_iterator dbms::get_iterator(unsigned long id) const throw (result_exception&) {
	std::unordered_map<unsigned long, dbms::query>::const_iterator it = query_map.find(id);
	if (it == query_map.end())
		throw result_exception("Result does not exists!");
	else
		return it;
}

std::unordered_map<unsigned long, dbms::query>::iterator dbms::get_iterator(unsigned long id) throw (result_exception&) {
	std::unordered_map<unsigned long, dbms::query>::iterator it = query_map.find(id);
		if (it == query_map.end())
			throw result_exception("Result does not exists!");
		else
			return it;
}

