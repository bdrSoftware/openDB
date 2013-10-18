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

#ifndef __OPENDB_REMOTE_DATABASE_MANAGEMENT_HEADER__
#define __OPENDB_REMOTE_DATABASE_MANAGEMENT_HEADER__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <memory>

#include "connection.hpp"

namespace openDB {
/*
 */
class dbms {
public :
	/*
	 */
	explicit dbms(unsigned _cuncurrend_connection = 5) throw (remote_exception&);
	~dbms();

	/*
	 */
	void host (std::string _host) throw ();
	std::string host () const throw ();

	/*
	 */
	void port (std::string _port) throw ();
	std::string port () const throw ();

	/*
	 */
	void dbname (std::string _dbname) throw ();
	std::string dbname () const throw ();

	/*
	 */
	void user (std::string _user) throw ();
	std::string user () const throw ();

	/*
	 */
	void passwd (std::string _passwd) throw ();
	std::string passwd () const throw ();


	/* Le funzioni seguenti gestiscono la connessione al DBMS.
	 * La funzione connect() avvia un tentativo di connessione al server postgres. Tale tentativo è bloccante per il thread che
	 * richiama la funzione, ciò vuo, dire che esso rimane in attesa del termine del tentativo. Qualora il tentativo di connessione
	 * fallisca, viene generata una eccezione di tipo connection_error, derivata da remote_exception, contenente un messaggio che
	 * illustra le cause del fallimento della connessione.
	 *
	 * La funzione disconnect() termina una connessione, liberando la memoria occupata e rilasciando le risorse necessarie a
	 * mantenerla attiva.
	 *
	 * La funzione reset() azzera un canale di comunicazione, ossia una connessione precedentemente attiva che per una qualche
	 * ragione è stata persa.
	 */
	void connect () throw (remote_exception&);
	void disconnect () throw ();
	void reset () throw ();

	unsigned long exec_query(std::string command) throw (basic_exception&);
	unsigned long exec_query_noblock(std::string command) throw (basic_exception&);

	bool executed (unsigned long resultID) const throw (result_exception&)
		{return get_iterator(resultID)->second.completed;}

	table& get_result(unsigned resultID) throw (result_exception&)
		{return *get_iterator(resultID)->second.result_table;}
	table& operator[] (unsigned resultID) throw (result_exception&)
		{return *get_iterator(resultID)->second.result_table;}

	void erase (unsigned long queryID) throw (result_exception&);

private:
	unsigned num_of_connection;

	struct connection_mtx {
		connection conn;
		std::mutex busy_mtx;
	};
	connection_mtx* connection_array;

	std::mutex connection_free_mtx;
	std::condition_variable_any connection_free_cv;
	unsigned num_of_free_connection;
	void execute_query (unsigned long id) throw ();

	unsigned long queryID;
	struct query {
		std::string command;
		std::unique_ptr<table> result_table;
		bool completed;
		query(std::string cmd) : command(cmd), completed(false) {}
	};
	std::unordered_map<unsigned long, query> query_map;
	std::unordered_map<unsigned long, query>::const_iterator get_iterator(unsigned long) const throw (result_exception&);
	std::unordered_map<unsigned long, query>::iterator get_iterator(unsigned long) throw (result_exception&);
};
};
#endif
