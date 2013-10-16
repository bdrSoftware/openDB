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
#include <map>
#include <memory>

#include "query.hpp"

namespace openDB {
/*
 */
class dbms {
public :
	/*
	 */
	explicit dbms(unsigned _cuncurrend_connection = 5) throw ();
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
	void connect () throw (connection_error&);
	void disconnect () throw ();
	void reset () throw ();

	unsigned long exec_query(std::string command) const throw (remote_exception&);


private:
	unsigned __cuncurrent_connection;

	struct connection_manager {
		unsigned __id;
		connection __pgConnection;
		std::mutex __busy_mtx;
	};
	connection_manager* __pgConnection_array;

	unsigned long __queryID;
	std::map<unsigned long, query> __queryMap;

	std::mutex __connection_free_mtx;
	std::condition_variable_any __connection_free_cv;
	unsigned __connection_free;
	void execute_query (unsigned long id) throw ();
};

};
#endif
