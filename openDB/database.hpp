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
#ifndef __OPENDB_DATABASE_HEADER__
#define __OPENDB_DATABASE_HEADER__

#include "schema.hpp"
#include "dbms.hpp"

namespace openDB {
/* Un oggetto di tipo database astrae il concetto di database sql. Mette a disposizione la possibilità di definire schemi (vedi oggetto schema, definito nell'header
 * schema.hpp), tabelle (vedi header table.hpp) e colonne (vedi header column.hpp) di alcuni tipi sql basilari (vedi header sqlType.hpp).
 * Gli oggetti definiti in questa libreria definiscono metodologie per la creazione di comandi sql da inviare ad un DBMS per gestire inserimento di nuove informazioni,
 * modifica o eliminazione di informazioni esistenti, recupero di informazioni anche complesse dal DBMS.
 */
class database {
public:
		/* Per costruire un oggetto di tipo database è necessario specificare un nome. Tale nome viene utilizzato per la memorizzazione di tutte le informazioni relative
		 * agli oggetti di tipo schema e table che ne compongono la struttura. Le informazioni vengono salvate in una cartella il cui nome corrisponde al nome indicati.
		 * Se il nome non viene indicato viene generata una eccezione di tipo 'storage_exception'.
		 */
		explicit database(unsigned _cuncurrend_connection = 5) throw (storage_exception&);

		/* La funzione consente l'aggiunta di uno schema alla struttura del database. Se esiste già uno schema con nome uguale a quello che si sta per inserire, viene
		 * generata una eccezione di tipo 'schema_exists', derivata da 'access_exception'
		 */
		void add_schema(std::string schemaName) throw (schema_exists&)
			{(find_schema(schemaName) ? throw("Schema '" + schemaName + "' already exists in database.") : __schemasMap.emplace(schemaName, schema(schemaName, __storageDirectory)));}

		/* La funzione restituisce il numero di schemi che compongono il database.
		 */
		unsigned number_of_schemas() const throw ()
			{return __schemasMap.size();}

		/* La funzione schemas_name restituisce una lista di stringhe contenente i nomi degli schemi che compongono il database
		 */
		std::unique_ptr<std::list<std::string>> schemas_name () const throw ();

		/* La funzione find_schema restituisce true se uno schema con lo stesso nome di quello indicato compone la struttura del database. Restituisce false altrimenti.
		 */
		bool find_schema(std::string schemaName) const throw ()
			{ if( __schemasMap.find(schemaName) != __schemasMap.end()) return true; return false;}

		/* La funzione drop_schema consente l'eliminazione di un schema dalla struttura del database, unitamente alle colonne che lo compongono. Se lo schema non esiste viene
		 * generata una eccezione di tipo schema_not_exists, derivata da access_exception.
		 */
		void drop_schema(std::string schemaName) throw (schema_not_exists&)
			{__schemasMap.erase(get_iterator(schemaName));}

		/* La funzione get_schema restituisce un riferimento, sia costante che non, ad un oggetto schema, di cui si specifica il nome, che compone la struttura di un oggetto
		 * database. Non si tratta di una copia, ma dell'oggetto vero e proprio. Tale riferimento può essere usato per richiamare direttamente i metodi della classe schema
		 * con molta più rapidità.
		 * La versione che restituisce un riferimento costante torna utile quando ci si vuole assicurare che nessuna modificazione venga subita dall'oggetto in questione.
		 *
		 */
		schema& get_schema(std::string schemaName) throw (schema_not_exists&)
			{return get_iterator(schemaName)->second;}
		const schema& get_schema(std::string schemaName) const throw (schema_not_exists&)
			{return get_iterator(schemaName)->second;}
		schema& operator[] (std::string schemaName) throw (schema_not_exists&)
			{return get_iterator(schemaName)->second;}
		const schema& operator[] (std::string schemaName) const throw (schema_not_exists&)
			{return get_iterator(schemaName)->second;}



		/* Le seguenti consentono di impostare o ottenere i parametri di connessione al database remoto.
		 */
		void host (std::string _host) throw ()
			{__remote_database.host(_host);}
		std::string host () const throw ()
			{return __remote_database.host();}
		void port (std::string _port) throw ()
			{__remote_database.port(_port);}
		std::string port () const throw ()
			{return __remote_database.port();}
		void dbname (std::string _dbname) throw ()
			{__remote_database.dbname(_dbname);}
		std::string dbname () const throw ()
			{return __remote_database.dbname();}
		void user (std::string _user) throw ()
			{__remote_database.user(_user);}
		std::string user () const throw ()
			{return __remote_database.user();}
		void passwd (std::string _passwd) throw ()
			{__remote_database.passwd(_passwd);}
		std::string passwd () const throw ()
			{return __remote_database.passwd();}

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
		void connect () throw (remote_exception&)
			{__remote_database.connect();}
		void disconnect () throw ()
			{__remote_database.disconnect();}
		void reset () throw ()
			{__remote_database.reset();}

		/* La funzione exec_query consente di eseguire un comando sql sul database remoto. Restituisce un identificativo unico attraverso il
		 * quale e' possibile accedere ai risultati di esecuzione della query. Causa il blocco del thread che la richiama fino al termine
 		 * delle operazioni e di interpretazione dei risultati.
		 * La funzione exec_query_nonblock non causa il blocck del thread che la richiama. Prima di accedere al result � necessario attende-
		 * re il termine delle operazioni. E' possibile richiamare la funzione executed() che restituisce true se l'esecuzione di una query
		 * e'terminata
		 * Puo' generare una eccezione di tipo 'connection_error' nel caso in cui si tenti l'esecuzione di una query su una connessione
		 * non attiva o non valida, oppure 'query_execution' nel caso in cui l'esecuzione della query non vada a buon fine oppure ancora
		 * una eccezione di tipo null_pointer nel caso in cui il tentativo di esecuzione della query non è stato avviato.
		 */
		unsigned long exec_query(std::string command) throw (basic_exception&)
			{return __remote_database.exec_query(command);}
		unsigned long exec_query_noblock(std::string command) throw (basic_exception&)
			{return __remote_database.exec_query_noblock(command);}

		/* La funzione executed() permette di verificare il termine dell'esecuzione di una query
		 */
		bool executed (unsigned long queryID) const throw (result_exception&)
			{return __remote_database.executed(queryID);}

		/* La funzione get_result restituisce un oggetto 'table' contenente il risultato di esecuzione di una query. E' bene, nel caso in cui
		 * l'esecuzione sia stata avviata chiamando la funzione exec_query_noblock(), verificare che l'esecuzione sia terminata richiamando la
		 * funzione executed().
		 */
		table& get_result(unsigned queryID) throw (result_exception&)
			{return __remote_database.get_result(queryID);}

		/* La seguente libera la memoria occupata dai risultati di esecuzione di una query. Tali risultati non saranno pi� disponibili.
		 */
		void erase (unsigned long queryID) throw (result_exception&)
			{__remote_database.erase(queryID);}

		/* Le due seguenti funzioni consentono, rispettivamente, di generare localmente la struttura del database remoto e di caricare localmente
		 * le tuple gestite dal database remoto.
		 */
		void load_structure() throw (basic_exception&);
		void load_tuple() throw (basic_exception&);

		/* La seguente consente di rendere effettive tutte le modifiche effetuate localmente, eseguendo comandi sql sul database remoto.
		 * E' bene richiamare la funzione load_tuple al termine delle  operazioni di commit. Restituisce una lista contenente gli id corrispondenti
		 * ai result generatll'esecuzione delle query.
		 */
		std::unique_ptr<std::list<unsigned long>> commit() throw (basic_exception&);

private:
		std::string									__storageDirectory;		/*	percorso della cartella contenente altre cartelle e file dove sono memorizzate i contenuti degli
																			 *	oggetti che compongono il database.
																			 */
		static const std::string					__tmpStorageDirectory;	/*	percorso base della cartella in cui viene creato l'insieme di file e directory per lo storage delle
																			 *	tabelle e degli schemi che compongono il database
																			 */
		std::unordered_map<std::string, schema>		__schemasMap;			/*	mappa degli schemi che compongono il database
																			 *	Gli schemi vengono organizzati in una struttura di tipo 'unordered_map', ossia un contenitore di tipo
																			 *  associativo che consente di accedere a qualsiasi posizione in tempo costante. Le chiavi di accesso a
																			 * 	tale struttura sono i nomi degli schemi.
																			 * 	Il prefisso 'unordered' non deve trarre in inganno: gli oggetti appaiono nello stesso ordine in cui
																			 * 	vengono inseriti nel contenitore, ma, di fatti, vengono ordinati utilizzando il valore hash della
																			 * 	chiave. Di conseguenza l'ordine in cui compaiono scorrendo il contenitore potrebbe differire dall'
																			 * 	ordine di creazione.
																			 */
		/* La funzione get_iterator restituisce un iteratore valido ad un oggetto di classe schema. Se l'oggetto non dovesse esistere, viene generata una eccezione
		 * di tipo schema_not_exist, derivata da access_exception.
		 * Queste funzioni relativamente semplici vengono usate praticamente ovunque, all'interno di questo modulo, qualora si deve accedere ad un oggetto schema, poichè
		 * semplificano il codice, riducendo il numero di linee, e riducono la quantità di test da effettuare.
		 */
		std::unordered_map<std::string, schema>::const_iterator get_iterator(std::string schemaName) const throw (schema_not_exists&);
		std::unordered_map<std::string, schema>::iterator get_iterator(std::string schemaName) throw (schema_not_exists&);


		/*parte "remota"*/
		static const std::string __all_column;
		struct all_column_query_field_name {
			std::string table_schema;
			std::string table_name;
			std::string column_name;
			std::string udt_name;
			std::string character_maximum_length;
			std::string numeric_precision;
			std::string numeric_scale;
			std::string datetime_precision;
			std::string column_default;
		};
		static const all_column_query_field_name all_column_field_name;
		sqlType::type_base* column_type(std::string udt_name, std::string character_maximum_length, std::string numeric_precision, std::string numeric_scale);
		void create_structure(table& structure_table);

		static const std::string __key_column;
		struct key_column_query_field_name {
			std::string table_schema;
			std::string table_name;
			std::string column_name;
		};
		static const key_column_query_field_name key_column_field_name;
		void define_key(table& key_table);

		static const std::string __referential;
		struct referential_query_field_name {
			std::string table_schema;
			std::string table_name;
			std::string column_name;
			std::string referred_table_schema;
			std::string referred_table_name;
			std::string referred_column_name;
		};
		static const referential_query_field_name referential_field_name;

		dbms __remote_database;

		/* La funzione seguente restituisce un puntatore 'intelligente' ad un oggetto lista di stringhe contenente comandi sql relativi alle operazioni di aggiornamento da
		 * effettuare sul database remoto a fronte delle modifiche apportate localmente ai record gestiti dalle tabelle che compongono gli schemi del database considerato.
		 * Ciascuno di questi comandi deve essere inviato al database remoto. Nessuna modifica viene effettuata sui record dopo la generazione dei comandi sql, quindi sa'
		 * necessario ricaricarli dal database.
		 */
		std::unique_ptr<std::list<std::string>> command_generator() const throw ();

};	/*	end of	database declaration	*/
};	/*	end of openDB namespace	*/
#endif
