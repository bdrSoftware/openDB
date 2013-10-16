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
#ifndef __OPENDB_SCHEMA_HEADER__
#define __OPENDB_SCHEMA_HEADER__

#include "table.hpp"

namespace openDB {
/* Un oggetto di tipo schema, astrae e riproduce il concetto di schema ER di un database. Esso, cioè, è un insieme di tabelle semanticamente affini.
 * Si pensi a tutte le tabelle che gestiscono informazioni riguardo ai pazienti di un ospedale, ad esempio "anagrafica_pazienti" e "cartelle_cliniche":
 * tali tabelle potrebbero essere "raggruppate" logicamente all'interno dello schema "pazienti".
 */
class schema {
public :
		/* Per costruire un oggetto di tipo schema è necessario specificare due parametri:
		 * 	- schemaName: il nome dello schema. All'interno dello stesso database non possono esserci due schemi con lo stesso nome. Il nome dello schema viene
		 * 				  utilizzato dalle funzioni di accesso, se non viene specificato viene generata una eccezione di tipo 'access_exception', derivata da
		 * 				  'basic_exception';
		 *  - storageDirectory: il percorso dove viene creato l'albero di directory preposto alla memorizzazione delle tuple gestire dalle tabelle che compongono
		 * 						lo schema. Se non viene specificato viene generata una eccezione di tipo 'storage_exception', derivata di 'basic_exception'.
		 */
		schema (std::string schemaName, std::string storageDirectory) throw (basic_exception&);

		/* Questa funzione restituisce il nome di uno schema. Il nome di ogni schema all'interno di uno stesso database deve essere univoco (vedi oggetto
		 * database, definito in database.hpp) perché usato per l'accesso ad esse.
		 */
		std::string name() const throw ()
			{return __schemaName;}

		/* La funzione add_table con un solo parametro di tipo stringa, consente di aggiungere un oggetto table ad un oggetto schema. Se, all'interno dello schema
		 * esiste già una tabella con lo stesso nome di quella che si sta tentando di aggiungere, viene generata una eccezione di tipo 'access_exception', derivata di
		 * basic_exception.
		 * Se il nome della tabella non viene specificato, viene generata una eccezione di tipo	access_exception.
		 *
		 * La seconda versione di add_table, con argomento un oggetto di tipo table, consente l'aggiunta di una tabella già esistente allo schema. In questo caso
		 * le proprietà della tabella vengono copiate da quella già esistente e viene creata una copia dell'oggetto in modo che, nel caso venga distrutto, non si
		 * verifichino errori di memoria o perdite di dati.
		 * Se all'interno dello schema esiste già una tabella con lo stesso nome di quella che si stà aggiungendo, viene generata una eccezione di tipo access_exception.
		 */
		void add_table(std::string tableName) throw (basic_exception&)
			{(find_table(tableName) ? throw table_exists("Table '" + tableName + "' already exists in schema '" + __schemaName + "'") : __tablesMap.emplace(tableName, table(tableName, __storageDirectory)));}
		void add_table(table& _tableObject) throw (basic_exception&);	//non ancora implementata

		/*	Restituisce il numero di tabelle che compongono lo schema.
		 */
		unsigned number_of_tables() const throw ()
			{return __tablesMap.size();}

		/* Restituisce una lista di streinghe contenente i nomi delle tabelle che compongono lo schema.
		 */
		std::unique_ptr<std::list<std::string>> tables_name (bool attach_schema_name = false) const throw ();

		/* La funzione find_table restituisce true se una tabella con lo stesso nome di quella indicata compone la struttura dello schema. Restituisce false altrimenti.
		 */
		bool find_table(std::string tableName) const throw ()
			{ if( __tablesMap.find(tableName) != __tablesMap.end()) return true; return false;}

		/* La funzione drop_table consente l'eliminazione di una tabella. Se la tabella non esiste viene generata una eccezione di tipo table_not_exists, derivata da
		 * access_exception. La tabella viene eliminata unitamente alle sue righe e lo spazio da esse occupate viene liberato.
		 */
		void drop_table(std::string tableName) throw (table_not_exists&)
			{__tablesMap.erase(get_iterator(tableName));}

		/* La funzione get_table restituisce un riferimento, sia costante che non, ad un oggetto table, di cui si specifica il nome, che compone la struttura di un oggetto
		 * schema. Non si tratta di una copia, ma dell'oggetto vero e proprio. Tale riferimento può essere usato per richiamare direttamente i metodi della classe table
		 * con molta più rapidità.
		 * La versione che restituisce un riferimento costante torna utile quando ci si vuole assicurare che nessuna modificazione venga subita dall'oggetto in questione.
		 */
		table& get_table(std::string tableName) throw (table_not_exists&)
			{return get_iterator(tableName)->second;}
		const table& get_table(std::string tableName) const throw (table_not_exists&)
			{return get_iterator(tableName)->second;}
		table& operator[] (std::string tableName) throw (table_not_exists&)
			{return get_iterator(tableName)->second;}
		const table& operator[] (std::string tableName) const throw (table_not_exists&)
			{return get_iterator(tableName)->second;}

		/* La funzione commit restituisce un puntatore 'intelligente' ad un oggetto lista di stringhe contenente comandi sql relativi alle operazioni di aggiornamento da
		 * effettuare sul database remoto a fronte delle modifiche apportate localmente ai record gestiti dalle tabelle che compongono l'oggetto schema considerato.
		 * Ciascuno di questi comandi deve essere inviato al database remoto. Nessuna modifica viene effettuata sui record dopo la generazione dei comandi sql, quindi sa'
		 * necessario ricaricarli dal database.
		 */
		std::unique_ptr<std::list<std::string>> commit() const throw ();

private :
		std::string 								__schemaName;			/*	nome dello schema. Deve essere univoco all'interno di uno stesso database perchè usato dalle
																			 * 	funzioni di accesso dell'oggetto database (vedi header database.hpp)
																			 */				
		std::string									__storageDirectory;		/*	percorso della cartella contenente i file dove sono memorizzate tutte le tuple delle tabelle che
																			 * 	compongono la struttura dello schema
																			 */
		std::unordered_map<std::string, table>		__tablesMap;			/*	mappa delle tabelle che compongono lo schema
																			 *	Le tabelle vengono organizzate in una struttura di tipo 'unordered_map', ossia un contenitore di tipo
																			 *  associativo che consente di accedere a qualsiasi posizione in tempo costante. Le chiavi di accesso a
																			 * 	tale struttura sono i nomi delle tabelle.
																			 * 	Il prefisso 'unordered' non deve trarre in inganno: gli oggetti appaiono nello stesso ordine in cui
																			 * 	vengono inseriti nel contenitore, ma, di fatti, vengono ordinati utilizzando il valore hash della
																			 * 	chiave. Di conseguenza l'ordine in cui compaiono scorrendo il contenitore potrebbe differire dall'
																			 * 	ordine di creazione.
																			 */
		/* La funzione get_iterator restituisce un iteratore valido ad un oggetto di classe table. Se l'oggetto non dovesse esistere, viene generata una eccezione
		 * di tipo table_not_exist, derivata da access_exception.
		 * Queste funzioni relativamente semplici vengono usate praticamente ovunque, all'interno di questo modulo, qualora si deve accedere ad un oggetto table, poichè
		 * semplificano il codice, riducendo il numero di linee, e riducono la quantità di test da effettuare.
		 */
		std::unordered_map<std::string, table>::const_iterator get_iterator(std::string tableName) const throw (table_not_exists&);
		std::unordered_map<std::string, table>::iterator get_iterator(std::string tableName) throw (table_not_exists&);

		/* Le funzioni insert_sql, update_sql e delete_sql generano stringhe contenenti comandi sql per, rispettivamente, inserimento, modifica e cancellazione
		 * delle informazioni relative ad un record, di cui si specifica l'ID, gestito da un oggetto di tipo table, di cui si specifica il nome.
		 * Queste funzioni prendono, in ingresso, due parametri:
		 * 	- tableName: è il nome della tabella contenente il record contenente le informazioni da utilizzare per la generazione del comando sql;
		 *	- ID: id interno del record contenente le informazioni da utilizzare per la generazione del comando sql.
		 * Queste funzioni possono generare i seguenti tipi di eccezione:
		 *	- table_not_exists: eccezione di tipo derivato da access_exception, generata se non esiste nessuna tabella il cui nome corrisponda a quello
		 * 	  specificato dal parametro tableName;
		 *  - record_not_exists: se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel parametro ID specifico
		 *  - file_open : eccezione derivata da storage_exception, viene generata se, a causa di un errore qualsiasi genere, non fosse possibile aprire il file dove sono memorizzati
		 *				  i record;
		 *  - io_error : eccezione derivata da storage_exception, viene generata se la dimensione dei dati scritti-letti non coincide con la dimensione del record.
		*/
		std::string insert_sql(std::string tableName, unsigned long ID) const throw (basic_exception&);
		std::string update_sql(std::string tableName, unsigned long ID) const throw (basic_exception&);
		std::string delete_sql(std::string tableName, unsigned long ID) const throw (basic_exception&);

};	/*	end of schema definition	*/
};	/*	end of openDB namespace	*/
#endif
