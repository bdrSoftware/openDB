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
#ifndef __OPENDB_TABLE_HEADER__
#define __OPENDB_TABLE_HEADER__

#include "column.hpp"
#include <memory>
#include <list>
#include <unordered_map>

namespace openDB {
/* Un oggetto di tipo table, oltre che un insieme di oggetti di tipo column, ognuno dei qualli contiene le proprietà di un insieme di valori semanticamente
 * affini, si occupa, anche se indirettamente, della loro memorizzazione e gestione.
 * Questo oggetto introduce, a livello concettuale, il concetto di riga, definendo metodologie per l'inserimento, la modifica, la cancellazione remota e
 * locale delle stesse.
 */
class table {
public:
		/* Un oggetto table viene costruito specificando almeno due parametri:
		 * - tableName: è il nome della tabella. DEVE essere univoco per ciascuna tabella all'interno di uno stesso schema (vedi oggetto schema in schema.hpp)
		 * 		  	    perchè utilizzato per accedere ad una tabella. Se il nome della tabella non viene specificato, viene generata una eccezione di tipo
		 * 				access_exception, derivata di basic_exception.
		 * - storageDirectory: è il percorso della directory dove deve essere creato il file per la memorizzazione delle righe gestite dall'ogetto in essere.
		 * 					   Nel caso in cui non venisse specificato e il parametro store_on_file fosse true, viene generata una eccezione di tipo
		 * 					   storage_exception, derivata di basic_exception.
		 *
		 * Altri parametri opzionali sono:
		 * - managesResult: deve essere impostato a true nel caso in cui l'oggetto in essere deve essere destinato alla memorizzazione di informazioni generate
		 * 					a seguito di una interrogazione al DBMS. In questo modo, operazioni di inserimento, modifica o cancellazione, che sarebbero prive di
		 * 					senso su una tabella di questo tipo, vengono evitate.
		 * - store_on_file: di default è true, in questo caso le righe gestite dalla tabella vengono memorizzate su file. Nel caso in cui sia false, si può
		 * 					evitare di specificare un valore per il parametro storageDirectory
		 */
		table (std::string tableName, std::string storageDirectory, bool managesResult = false, bool store_on_file = true) throw (basic_exception&);

		/* Questa funzione restituisce il nome di una tabella. Il nome di ogni tabella all'interno di uno stesso schema dovrebbe essere univoco (vedi oggetto
		 * schema, definito in schema.hpp) perché usato per l'accesso ad esse.
		 */
		std::string name() const throw ()
			{return __tableName;}

		/* La funzione manage_result restituisce true se la tabella gestisce il risultato di esecuzione di una query, false altrimenti.
		 */
		bool manages_result () const throw ()
			{return __managesResult;}

		/* La funzione add_column consente l'aggiunta di una colonna alla struttura della tabella.
		 * Per costruire un oggetto di tipo column sono necessari tre parametri:
		 * 	- columnName: il nome della colonna. Dove essere UNIVOCO all'interno di una stessa tabella (vedi oggetto table, definito in table.hpp)
		 *  - columnType: il tipo della colonna. Per creare una colonna di tipo varchar(20), ad esempio, si può procedere come segue:
		 * 			table.add_column("colonna", new sqlType::varchar(10));
		 * 	 per costruire una colonna di tipo boolean, invece
		 * 			table.add_column("colonna", new sqlType::boolean);
		 *  - key : se questo paramentro assume valore true, allora vuol dire che la colonna compone la chiave per la tabella a cui appartiene. È previsto
		 * 		    un controllo sulle colonne chiave: quando si valida un valore con la funzione validate_value (vedi sotto), essa genera una eccezione di
		 * 		    tipo key_empty, derivata di data_exception, se la stringa che si passa alla funzione è vuota.
		 *
		 * Se nella tabella esiste una colonna con lo stesso nome di quella che si sta aggiungendo, la funzione add_column genera una eccezione di tipo column_exists,
		 * tipo di eccezione derivato da access_exception.
		 *
		 * LA SEGUENTE SITUAZIONE È DA EVITARE
		 * 		openDB::sqlType::type_base* base_ptr = new openDB::sqlType::boolean;
		 *		{
		 *		table.add_column("colonna", base_ptr, true);
		 *		...
		 *		}
		 *		try {base_ptr -> validate_value("scemo");}
		 *		catch(openDB::data_exception& e) {cout <<e.what() <<endl;}
		 * ESEGUENDO QUESTO CODICE VIENE GENERATO ERRORE DI SEGMENTAZIONE. È UNA COSA VOLUTA. IL DISTRUTTORE DI COLUMN DEALLOCA AUTOMATICAMENTE LO SPAZIO
		 * OCCUPATO DALL'OGGETTO CHE GESTISCE IL TIPO DELLA COLONNA;
		 */
		void add_column(std::string columnName,	sqlType::type_base* columnType, bool key = false) throw (column_exists&)
			{(find_column(columnName) ? throw column_exists("'" + columnName + "' already exists in table'" + __tableName + "'") : __columnsMap.emplace(columnName, column(columnName, columnType, key)));}

		/* La funzione number_of_columns restituisce un intero senza segno corrispondente al numero di colonne che fanno parte della struttura della tabella.
		 */
		unsigned number_of_columns() const throw ()
			{return __columnsMap.size();}

		/* La funzione columns_name restituisce un oggetto std::unique_ptr<std::list<std::string>>, ossia un puntatore intelligente ad un oggetto di tipo std::list di
		 * oggetti std::string. Tale lista di stringhe contiene il nome delle colonne che fanno parte della struttura della tabella.
		 * L'ordine in cui si trovano i nomi all'interno di tale lista potrebbe non corrispondere all'ordine con cui le colonne sono state aggiunte alla struttura della
		 * tabella.
		 */
		std::unique_ptr<std::list<std::string>> columns_name (bool attach_table_name = false) const throw ();

		/* La funzione find_column restituisce true se una colonna con lo stesso nome di quella indicata compone la struttura della tabella. Restituisce false altrimenti.
		 */
		bool find_column(std::string columnName) const throw ()
			{ if( __columnsMap.find(columnName) != __columnsMap.end()) return true; return false;}

		/* La funzione drop_column consente l'eliminazione di una colonna. Se la colonna non esiste viene generata una eccezione di tipo column_not_exists, derivata da
		 * access_exception
		 */
		void drop_column(std::string columnName) throw (column_not_exists&)
			{__columnsMap.erase(get_iterator(columnName));}

		/* La funzione get_column restituisce un riferimento, sia costante che non, ad un oggetto column, di cui si specifica il nome, che compone la struttura di un oggetto
		 * table. Non si tratta di una copia, ma dell'oggetto vero e proprio. Tale riferimento può essere usato per richiamare direttamente i metodi della classe column
		 * con molta più rapidità.
		 * Si consideri il seguente esempio:
		 *
		 *		openDB::column& _column_ref = _table.get_column("colonna_date");
		 *		openDB::query_attribute _attribute = _column_ref.get_attribute();
		 *		_attribute.project(true);
		 * 		_attribute.selectValue("scemo cretino");
		 *		_column_ref.set_attribute(_attribute);
		 *		openDB::query_attribute _attribute2 = _column_ref.get_attribute();
		 *		cout <<"colonna_date will be" <<(!_attribute2.project() ? " not " : " ") <<"projected" <<endl; 
		 *		cout <<"colonna_date will be" <<(!_attribute2.select() ? " not " : " ") <<"selected" <<endl; 
		 *		cout <<"colonna_date will be selected with '" <<_attribute2.selectValue() <<"'" <<endl;
		 *
		 * oppure
		 *
		 * 		openDB::query_attribute _attribute = _table.get_column("colonna_date").get_attribute();
		 *		_attribute.project(true);
		 *		_attribute.selectValue("scemo cretino");
		 *		_table.get_column("colonna_date").set_attribute(_attribute);
		 *		openDB::query_attribute _attribute2 = _table.get_column("colonna_date").get_attribute();
		 *		cout <<"colonna_date will be" <<(!_attribute2.project() ? " not " : " ") <<"projected" <<endl; 
		 *		cout <<"colonna_date will be" <<(!_attribute2.select() ? " not " : " ") <<"selected" <<endl; 
		 *		cout <<"colonna_date will be selected with " <<_attribute2.selectValue() <<endl; 
		 * 
		 * entrambi generano il seguente output:
		 *
		 * 	colonna_date will be projected
		 *	colonna_date will be not selected
		 *	colonna_date will be selected with 'scemo cretino'
		 *
		 * La versione che restituisce un riferimento costante torna utile quando ci si vuole assicurare che nessuna modificazione venga subita dall'oggetto in questione,
		 * ad esempio, si consideri il seguente codice:
		 *
		 * 		const openDB::column& _const_column_ref = _table.get_column("colonna_date");
		 *		try {_const_column_ref.validate_value("05/12/13");}
		 *		catch (openDB::data_exception& e) {cout <<e.what() <<endl;}
		 *
		 * oppure
		 *
		 * 		try {_table.get_column("colonna_date").validate_value("05/12/13");}
		 *		catch (openDB::data_exception& e) {cout <<e.what() <<endl;}
		 * 
		 * il cui output, in entrambi i casi, è il seguente:
		 *
		 * 		Ambiguous value for type 'date': 05/12/13 is ambiguous.
		 *
		 * Nel caso in cui la colonna specifica non esiste, viene generata una eccezione di tipo column_not_exists, derivata da access_exception.
		 */
		column& get_column(std::string columnName) throw (column_not_exists&)
			{return get_iterator(columnName)->second;}
		const column& get_column(std::string columnName) const throw (column_not_exists&)
			{return get_iterator(columnName)->second;}
		
private:
		std::string									__tableName;			/*	nome della tabella		*/
		std::string									__storageDirectory;		/*	percorso della cartella contenente il file dove sono memorizzate tutte le tuple della tabella	*/
		bool										__managesResult;		/*	se la tabella gestisce il risultato di esecuzione di una query, questo attributo è true	*/
		std::unordered_map<std::string, column>		__columnsMap;			/*	mappa delle colonne che compongono la tabella
																			 *	Le colonne vengono organizzate in una struttura di tipo 'unordered_map', ossia un contenitore di tipo
																			 *  associativo che consente di accedere a qualsiasi posizione in tempo costante. Le chiavi di accesso a
																			 * 	tale struttura sono i nomi delle colonne.
																			 * 	Il prefisso 'unordered' non deve trarre in inganno: gli oggetti appaiono nello stesso ordine in cui
																			 * 	vengono inseriti nel contenitore, ma, di fatti, vengono ordinati utilizzando il valore hash della
																			 * 	chiave. Di conseguenza l'ordine in cui compaiono scorrendo il contenitore potrebbe differire dall'
																			 * 	ordine di creazione.
																			 */
		/* La funzione get_iterator restituisce un iteratore valido ad un oggetto di classe column. Se l'oggetto non dovesse esistere, viene generata una eccezione
		 * di tipo column_not_exist, derivata da access_exception.
		 * Queste funzioni relativamente semplici vengono usate praticamente ovunque, all'interno di questo modulo, qualora si deve accedere ad un oggetto column, poichè
		 * semplificano il codice e riducono la quantità di test da effettuare.
		 */
		std::unordered_map<std::string, column>::const_iterator get_iterator(std::string columnName) const throw (column_not_exists&);
		std::unordered_map<std::string, column>::iterator get_iterator(std::string columnName) throw (column_not_exists&);
};
};
#endif
