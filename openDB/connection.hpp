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

#include "result.hpp"
#include <string>

namespace openDB {
/* La classe connection gestisce la connessione con un database remoto. Essa possiede i seguenti attributi:
 * 	- host: indirizzo o nome dell'host che ospita il server postgres che gestisce il database;
 * 	- post: porta da utilizzare per la connessione, di default è 5432;
 * 	- dbname : è il nome del database al quale ci si vuole collegare;
 * 	- user : è il nome dell'utente che vuole effettuare la connessione;
 * 	- passwd: è la password associata a 'user' con il quale il DBMS remoto identifica l'utente.
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

	/* La funzione exec query permette l'esecuzione di un comando sql su un database remoto al quale sia stata effettuala la
	 * connessione. La chiamata a questa funzione risulta bloccante per il thread chiamante. Il risultato dell'esecuzione della
	 * query è un oggetto di tipo 'result' (vedi header 'result.hpp') per i dettagli.
	 * Può generare una eccezione di tipo 'connection_error' nel caso in cui si tenti l'esecuzione di una query su una connessione
	 * non attiva o non valida, oppure 'query_execution' nel caso in cui l'esecuzione della query non vada a buon fine.
	 */
	result& exec_query(std::string command) const throw (remote_exception&);

	/* L'operatore () restituisce il puntatore alla struttura che materialmente gestisce la connessione con il server postgres.
	 * Per ulteriori dettagli bisogna consultare il manuale di postgres, in particolare la sezione sulla libreria C che gestisce
	 * la comunicazione con il server.
	 */
	PGconn* operator() () const throw ()
		{return __pgconnection;}

private:
	/* 	- host: indirizzo o nome dell'host che ospita il server postgres che gestisce il database;
	 * 	- post: porta da utilizzare per la connessione, di default è 5432;
	 * 	- dbname : è il nome del database al quale ci si vuole collegare;
	 * 	- user : è il nome dell'utente che vuole effettuare la connessione;
	 * 	- passwd: è la password associata a 'user' con il quale il DBMS remoto identifica l'utente.
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
