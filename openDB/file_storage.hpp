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

#ifndef __OPENDB_FILE_STORAGE_HEADER__
#define __OPENDB_FILE_STORAGE_HEADER__

#include "storage.hpp"

namespace openDB{
/*
 */
class file_storage : public storage {
public:
		/*
		 */
		file_storage(std::string fileName) throw (file_creation&);

		/* La funzione membro 'internalID' restituisce un oggetto std::list di unsigned long, più precisamente un oggetto unique_ptr contenente un puntatore ad un oggetto
		 * std::list<unsigned long>, che contiene l'elenco delle chiavi generate che sono ancora valide.
		 * Una chiave è valida quando la corrispondenza chiave-valore è valida, cioè quando ne chiave ne valore sono oggetto di cancellazione.
		 */
		virtual std::unique_ptr<std::list<unsigned long>> internalID () const throw ();

		/* La funzione numRecords restituisce il numero di record gestiti. Corrisponde al numero di chiavi valide.
		 */
		virtual unsigned long numRecords () const throw ()
		{return __recordMap.size();}

		/* La funzione clear svuota il gestore, liberando lo spazio occupato dai record e riportando il gestore allo stato in cui si troverebbe se fosse stato appena
		 * creato.
		 */
		virtual void clear () throw ();

		/* La funzione insert consente di creare un nuovo record e di inserirlo tra quelli gestiti dal gestore.
		 * I paramentri sono:
		 * 	- valueMap : mappa il cui primo campo è il nome della colonna in cui inserire il valore contenuto nel secondo campo. Se non esiste nessuna colonna con il nome
		 * 				 specificato, viene generata una eccezione di tipo column_not_exists, derivata da access_exception. Se uno dei valori contenuti nel secondo campo
		 * 				 non fosse valido per il tipo di colonna al quale deve corrispondere, viene generata una eccezione derivata da data_exception. Vedi l'header
		 * 				 sqlType.hpp per i dettagli.
		 * 	- columnsMap : mappa delle colonne che compongono una tabella. Questo parametro viene utilizzato per la validazione dei valori contenuti in valueMap.
		 * 	- _state : rappresenta lo stato del record.
		 * 	Lo stato del record può essere:
		 *  - record::empty : una tupla viene creata emty quando bisogna, ad esempio, leggerla da file oppure da memoria. Costruire una tupla vuota non ha molto senso se
		 * 					  non in questo contesto;
		 *  - record::loaded : una tupla DEVE essere creata loaded nel caso in cui viene caricata dal database gestito;
		 *  - record::inserting : una tupla viene creata inserting quando i dati che contiene devono essere inseriti nel database remoto;
		 *  - record::updating : una tupla con stato updating è una tupla, già esistente nel database, i cui valori devono essere aggiornati
		 *  - record::deleting: una tupla con stato deleting deve essere rimossa dal database;
		 *  * Possono essere generate le seguanti tipologie di eccezione:
		 * - key_empty : se ad una colonna chiave, o che compone la chiave, è associato un valore nullo.
		 * - column_not_exists : se una delle corrispondenze colonna-valore in valuesMap non è valida, cioè la colonna non esiste in columnsMap;
		 * - data_exception : viene generata una eccezione di tipo derivato da data_exception (vedi header 'exception.hpp') quando la corrispondenza colonna-valore non è
		 * 					  valida a causa di un errore dovuto ad un valore non compatibile con il tipo della colonna.
		 */
		virtual unsigned long insert (std::unordered_map<std::string, std::string>& valuesMap, std::unordered_map<std::string, column>& columnsMap, enum record::state _state) throw (basic_exception&);

		/* La funzione update consente di marcare i valori di un record affinchè siano aggiornati correttamente dal database remoto. Prende i seguenti parametri:
		 * consente di creare una tupla in modo corretto, inserendo opportunamente i dati. I paramentri sono:
		 * 	- valueMap : mappa il cui primo campo è il nome della colonna in cui inserire il valore contenuto nel secondo campo. Se non esiste nessuna colonna con il nome
		 * 				 specificato, viene generata una eccezione di tipo column_not_exists, derivata da access_exception. Se uno dei valori contenuti nel secondo campo
		 * 				 non fosse valido per il tipo di colonna al quale deve corrispondere, viene generata una eccezione derivata da data_exception. Vedi l'header
		 * 				 sqlType.hpp per i dettagli.
		 * 	- columnsMap : mappa delle colonne che compongono una tabella. Questo parametro viene utilizzato per la validazione dei valori contenuti in valueMap.
		 * 	Quando si aggiorna un oggetto record possono essere generate le seguenti tipologie di eccezione:
		 * 	- record_not_exists : se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel parametro ID specifico;
		 *  - column_not_exists : se una delle corrispondenze colonna-valore in valuesMap non è valida, cioè la colonna non esiste in columnsMap;
		 *  - data_exception : viene generata una eccezione di tipo derivato da data_exception (vedi header 'exception.hpp') quando la corrispondenza colonna-valore non è
		 * 					  valida a causa di un errore dovuto ad un valore non compatibile con il tipo della colonna.
		 */
		virtual void update (unsigned long ID, std::unordered_map<std::string, std::string>& valuesMap, std::unordered_map<std::string, column>& columnsMap) throw (basic_exception&);

		/* La funzione cancel marca un record affinchè sia rimosso dal database remoto all'atto del commit.
		 * La funzione può generare una eccezione di tipo 'record_not_exists' se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel
		 * parametro ID specifico
		 */
		virtual void cancel (unsigned long ID) throw (storage_exception&);

		/* La funzione erase rimuove un record dal gestore.
		 * La funzione può generare una eccezione di tipo 'record_not_exists' se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel
		 * parametro ID specifico
		 */
		virtual void erase (unsigned long ID) throw (storage_exception&);

		/* La funzione state restituisce lo stato di un record.
		 * Lo stato del record può essere:
		 *  - record::empty : una tupla viene creata emty quando bisogna, ad esempio, leggerla da file oppure da memoria. Costruire una tupla vuota non ha molto senso se non
		 * 					  in questo contesto;
		 *  - record::loaded : una tupla DEVE essere creata loaded nel caso in cui viene caricata dal database gestito;
		 *  - record::inserting : una tupla viene creata inserting quando i dati che contiene devono essere inseriti nel database remoto;
		 *  - record::updating : una tupla con stato updating è una tupla, già esistente nel database, i cui valori devono essere aggiornati
		 *  - record::deleting: una tupla con stato deleting deve essere rimossa dal database;
		 * La funzione può generare una eccezione di tipo 'record_not_exists' se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel
		 * parametro ID specifico
		 */
		virtual enum record::state state (unsigned long ID) const throw (storage_exception&)
				{return get_record(ID)->state();}

		/* La funzione visible restituisce true se i valori del record sono 'visibili' ossia se ha senso che siano visibili in una interfaccia. Potrebbe essere privo di
		 * senso visualizzare i valori di un record che sta per essere cancellato...
		 * La funzione può generare una eccezione di tipo 'record_not_exists' se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel
		 * parametro ID specifico
		 */
		virtual bool visible (unsigned long ID)	const throw (storage_exception&)
				{return get_record(ID)->visible();}

		/* La funzione current restituisce un oggetto unordered_map il cui primo campo contiene il nome di una colonna mentre il secondo campo contiene il corrispettivo
		 * valore memorizzato dalla tupla.
		 * La funzione old restituisce un oggetto unordered_map il cui primo campo contiene il nome di una colonna mentre il secondo campo contiene il corrispettivo
		 * valore precedentemente memorizzato dalla tupla.
		 * La funzione può generare una eccezione di tipo 'record_not_exists' se non esiste nessun record che sia in corrispondenza valida con la chiave contenuta nel
		 * parametro ID specifico
		 */
		virtual std::unique_ptr<std::unordered_map<std::string, std::string>> current(unsigned long ID) const throw (storage_exception&)
				{return get_record(ID)->current();}
		virtual std::unique_ptr<std::unordered_map<std::string, std::string>> old(unsigned long ID) const throw (storage_exception&)
				{return get_record(ID)->old();}

private:
		/*
		 */
		std::string	__fileName;

		/*
		 */
		struct segment {
				segment() : begin(0), end(0) {}
				std::streampos begin;
				std::streampos end;
				std::streamoff size() const {return end - begin + (std::streamoff)1;}
		};
		std::unordered_map<unsigned long, segment>	__recordMap;

		/*
 		 */
		std::unique_ptr<record>	get_record (unsigned long ID) const throw (storage_exception&);

		/*
		 */
		void write (const record& _record, const segment _segment) const throw (storage_exception&);
		void append (const record& _record, segment& _segment) throw (storage_exception&);
		void read (record& _record, const segment _segment) const throw (storage_exception&);

		/*
		 */
		std::unordered_map<unsigned long, segment> __trash;
		unsigned long __trashID;

		/*
		 */
		void pushTrash	(segment _segment);
		unsigned long recycle (std::streamoff byte) const;
		segment popTrash (unsigned long id, std::streamoff byte);




};
}; /*	end of openDB namespace	*/
#endif
