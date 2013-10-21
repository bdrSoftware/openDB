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
#include "storage.hpp"
#include "memory_storage.hpp"
#include "file_storage.hpp"
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

		/* La funzione Key_column_name restituisce un oggetto unique_ptr contenente il puntatore ad un oggetto list<string> contenente i nomi delle colonne che compongono
		 * la chiave primaria per l'ogetto table considerato.
		 */
		std::unique_ptr<std::list<std::string>> key_columns_name (bool attach_table_name = false) const throw ();

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
		 * Gli operatori [] sono perfettamente equivalenti alle funzioni get_column e per loro vale quanto gia' detto precedentemente.
		 */
		column& get_column(std::string columnName) throw (column_not_exists&)
			{return get_iterator(columnName)->second;}
		const column& get_column(std::string columnName) const throw (column_not_exists&)
			{return get_iterator(columnName)->second;}
		column& operator[] (std::string columnName) throw (column_not_exists&)
			{return get_iterator(columnName)->second;}
		const column& operator[] (std::string columnName) const throw (column_not_exists&)
			{return get_iterator(columnName)->second;}

		/* La funzione membro 'internalID' restituisce un oggetto std::list di unsigned long, più precisamente un oggetto unique_ptr contenente un puntatore ad un oggetto
		 * std::list<unsigned long>, che contiene l'elenco delle chiavi generate che sono ancora valide.
		 * Una chiave è valida quando la corrispondenza chiave-valore è valida, cioè quando ne chiave ne valore sono oggetto di cancellazione.
		 */
		std::unique_ptr<std::list<unsigned long>> internalID () const throw ()
			{return __storage->internalID();}

		/* La funzione numRecords restituisce il numero di record gestiti. Corrisponde al numero di chiavi valide.
		 */
		unsigned long numRecords () const throw ()
			{return __storage->numRecords();}

		/* La funzione clear svuota il gestore, liberando lo spazio occupato dai record e riportando il gestore allo stato in cui si troverebbe se fosse stato appena
		 * creato.
		 */
		void clear () throw ()
			{__storage->clear();}

		/* La funzione insert consente di creare un nuovo record e di inserirlo tra quelli gestiti dal gestore.
		 * I paramentri sono:
		 * 	- valueMap : mappa il cui primo campo è il nome della colonna in cui inserire il valore contenuto nel secondo campo.
		 * Possono essere generate le seguanti tipologie di eccezione:
		 * - key_empty : se ad una colonna chiave, o che compone la chiave, è associato un valore nullo.
		 * - column_not_exists : se una delle corrispondenze colonna-valore in valuesMap non è valida, cioè la colonna non esiste in columnsMap;
		 * - data_exception : viene generata una eccezione di tipo derivato da data_exception (vedi header 'exception.hpp') quando la corrispondenza colonna-valore non è
		 * 					  valida a causa di un errore dovuto ad un valore non compatibile con il tipo della colonna.
		 *  - file_open : eccezione derivata da storage_exception, viene generata se, a causa di un errore qualsiasi genere, non fosse possibile aprire il file dove sono memorizzati
		 *				  i record;
		 *  - io_error : eccezione derivata da storage_exception, viene generata se la dimensione dei dati scritti-letti non coincide con la dimensione del record.
		 *
		 *  La funzione load è, in un certo senso, simile alla funzione insert. Mentre la funzione insert va usata quando si vuole inserire tuple nella tabella affinchè
		 *  esse siano inserite nel database, la funzione load è utile a caricare tuple già esistenti, magari provenienti da un risultato di esecuzione di una query,
		 *  all'interno dell'oggetto tupla. Per tanto tale funzione deve essere utilizzata soltanto in occasione del caricamento in locale delle tuple remote.
		 */
		unsigned long insert (std::unordered_map<std::string, std::string>& valuesMap) throw (basic_exception&)
			{return __storage->insert(valuesMap, __columnsMap, record::inserting);}
		unsigned long load (std::unordered_map<std::string, std::string>& valuesMap) throw (basic_exception&)
			{return __storage->insert(valuesMap, __columnsMap, record::loaded);}

		/* La funzione update consente di marcare i valori di un record affinchè siano aggiornati correttamente dal database remoto. Prende i seguenti parametri:
		 * consente di creare una tupla in modo corretto, inserendo opportunamente i dati. I paramentri sono:
		 * 	- valueMap : mappa il cui primo campo è il nome della colonna in cui inserire il valore contenuto nel secondo campo. Se non esiste nessuna colonna con il nome
		 * 				 specificato, viene generata una eccezione di tipo column_not_exists, derivata da access_exception. Se uno dei valori contenuti nel secondo campo
		 * 				 non fosse valido per il tipo di colonna al quale deve corrispondere, viene generata una eccezione derivata da data_exception. Vedi l'header
		 * 				 sqlType.hpp per i dettagli.
		 * 	- columnsMap : mappa delle colonne che compongono una tabella. Questo parametro viene utilizzato per la validazione dei valori contenuti in valueMap.
		 * 	Quando si aggiorna un oggetto record possono essere generate le seguenti tipologie di eccezione:
		 *  - record_not_exists : se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel parametro ID specifico;
		 *  - column_not_exists : se una delle corrispondenze colonna-valore in valuesMap non è valida, cioè la colonna non esiste in columnsMap;
		 *  - data_exception : viene generata una eccezione di tipo derivato da data_exception (vedi header 'exception.hpp') quando la corrispondenza colonna-valore non è
		 * 					  valida a causa di un errore dovuto ad un valore non compatibile con il tipo della colonna.
		 *  - file_open : eccezione derivata da storage_exception, viene generata se, a causa di un errore qualsiasi genere, non fosse possibile aprire il file dove sono memorizzati
		 *		i record;
		 *  - io_error : eccezione derivata da storage_exception, viene generata se la dimensione dei dati scritti-letti non coincide con la dimensione del record.
		 */
		void update (unsigned long ID, std::unordered_map<std::string, std::string>& valuesMap) throw (basic_exception&)
			{__storage->update(ID, valuesMap, __columnsMap);}

		/* La funzione cancel marca un record affinchè sia rimosso dal database remoto all'atto del commit.
		 * La funzione può generare una eccezione di tipo :
		 *  - record_not_exists: se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel parametro ID specifico
		 *  - file_open : eccezione derivata da storage_exception, viene generata se, a causa di un errore qualsiasi genere, non fosse possibile aprire il file dove sono memorizzati
		 *		          i record;
		 *  - io_error : eccezione derivata da storage_exception, viene generata se la dimensione dei dati scritti-letti non coincide con la dimensione del record.
 		 */
		void cancel (unsigned long ID) throw (storage_exception&)
			{__storage->cancel(ID);}

		/* La funzione erase rimuove un record dal gestore.
		 * La funzione può generare una eccezione di tipo :
		 *  - record_not_exists: se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel parametro ID specifico
		 */
		void erase (unsigned long ID) throw (storage_exception&)
			{__storage->erase(ID);}

		/* La funzione state restituisce lo stato di un record.
		 * Lo stato del record può essere:
		 *  - record::empty : una tupla viene creata emty quando bisogna, ad esempio, leggerla da file oppure da memoria. Costruire una tupla vuota non ha molto senso se non
		 * 					  in questo contesto;
		 *  - record::loaded : una tupla DEVE essere creata loaded nel caso in cui viene caricata dal database gestito;
		 *  - record::inserting : una tupla viene creata inserting quando i dati che contiene devono essere inseriti nel database remoto;
		 *  - record::updating : una tupla con stato updating è una tupla, già esistente nel database, i cui valori devono essere aggiornati
		 *  - record::deleting: una tupla con stato deleting deve essere rimossa dal database;
		 * La funzione può generare una eccezione di tipo :
		 *  - record_not_exists: se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel parametro ID specifico
		 *  - file_open : eccezione derivata da storage_exception, viene generata se, a causa di un errore qualsiasi genere, non fosse possibile aprire il file dove sono memorizzati
		 *		          i record;
		 *  - io_error : eccezione derivata da storage_exception, viene generata se la dimensione dei dati scritti-letti non coincide con la dimensione del record.
		 */
		enum record::state state (unsigned long ID) const throw (storage_exception&)
			{return __storage->state(ID);}

		/* La funzione visible restituisce true se i valori del record sono 'visibili' ossia se ha senso che siano visibili in una interfaccia. Potrebbe essere privo di
		 * senso visualizzare i valori di un record che sta per essere cancellato...
		 * La funzione può generare una eccezione di tipo :
		 *  - record_not_exists: se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel parametro ID specifico
		 *  - file_open : eccezione derivata da storage_exception, viene generata se, a causa di un errore qualsiasi genere, non fosse possibile aprire il file dove sono memorizzati
		 *		i record;
		 *  - io_error : eccezione derivata da storage_exception, viene generata se la dimensione dei dati scritti-letti non coincide con la dimensione del record.
		 */
		bool visible (unsigned long ID)	const throw (storage_exception&)
			{return __storage->visible(ID);}

		/* La funzione current restituisce un oggetto unordered_map il cui primo campo contiene il nome di una colonna mentre il secondo campo contiene il corrispettivo
		 * valore memorizzato dalla tupla.
		 * La funzione old restituisce un oggetto unordered_map il cui primo campo contiene il nome di una colonna mentre il secondo campo contiene il corrispettivo
		 * valore precedentemente memorizzato dalla tupla.
		 * La funzione può generare una eccezione di tipo :
		 *  - record_not_exists: se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel parametro ID specifico
		 *  - file_open : eccezione derivata da storage_exception, viene generata se, a causa di un errore qualsiasi genere, non fosse possibile aprire il file dove sono memorizzati
		 *		i record;
		 *  - io_error : eccezione derivata da storage_exception, viene generata se la dimensione dei dati scritti-letti non coincide con la dimensione del record.
		 */
		std::unique_ptr<std::unordered_map<std::string, std::string>> current(unsigned long ID) const throw (storage_exception&)
			{return __storage->current(ID);}
		std::unique_ptr<std::unordered_map<std::string, std::string>> old(unsigned long ID) const throw (storage_exception&)
			{return __storage->old(ID);}

		/* La funzione to_html genera una pagina html molto minimalista, contenente tutte le informazioni gestite dall'oggetto table, organizzate per righe e per colonne.
		 * La funzione prende tre parametri:
		 * 	- fileName: nome del file di output;
		 *	- print_row: se true le tabelle verranno evidenziate con un colore di sfondo alternato, in modo da facilitare la lettura;
		 *	- bgcolor: codice esadecimale del colore di sfondo. se print_row è false, questo parametro viene ignorato. Il codice esadecimane dello sfondo
		 *		   deve essere preceduto dal carattere '#'. es. "#e6e6e6". 
		 * La funzione può generare una eccezione di tipo :
		 *  - file_creation: nel caso non sia possibile creare fileName
		 *  - record_not_exists: se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel parametro ID specifico
		 *  - file_open : eccezione derivata da storage_exception, viene generata se, a causa di un errore qualsiasi genere, non fosse possibile aprire il file dove sono memorizzati
		 *		i record;
		 *  - io_error : eccezione derivata da storage_exception, viene generata se la dimensione dei dati scritti-letti non coincide con la dimensione del record.
		 */
		void to_html(std::string fileName, bool print_row = true, std::string bgcolor="#e6e6e6") const throw (storage_exception&);
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

		/*
	     */
	    std::unique_ptr<storage> __storage;

};
};
#endif
