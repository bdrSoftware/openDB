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

#ifndef __OPENDB_RECORD_HEADER__
#define __OPENDB_RECORD_HEADER__

#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include "exception.hpp"

namespace openDB{
/*
 */
class record {
public:
	/* Stato della tupla:
	 * -empty : una tupla viene creata emty quando bisogna, ad esempio, leggerla da file oppure da memoria. Costruire una tupla vuota non ha molto senso se non
	 * 			in questo contesto;
	 * - loaded : una tupla DEVE essere creata loaded nel caso in cui viene caricata dal database gestito;
	 * - inserting : una tupla viene creata inserting quando i dati che contiene devono essere inseriti nel database remoto;
	 * - updating : una tupla con stato updating è una tupla, già esistente nel database, i cui valori devono essere aggiornati
	 * - deleting: una tupla con stato deleting deve essere rimossa dal database;
	 */
	enum state {empty, loaded, inserting, updating, deleting};

	/* Costruttore
	 * Il costruttore senza argomenti costruisce una tupla vuota, utile nel caso in cui si debba leggere tuple da file;
	 * Il costruttore con argomenti consente di creare una tupla in modo corretto, inserendo opportunamente i dati. I paramentri sono:
	 * 	- valueMap : mappa il cui primo campo è il nome della colonna in cui inserire il valore contenuto nel secondo campo. Se non esiste nessuna colonna con il nome
	 * 				 specificato, viene generata una eccezione di tipo column_not_exists, derivata da access_exception. Se uno dei valori contenuti nel secondo campo
	 * 				 non fosse valido per il tipo di colonna al quale deve corrispondere, viene generata una eccezione derivata da data_exception. Vedi l'header
	 * 				 sqlType.hpp per i dettagli.
	 * 	- columnsMap : mappa delle colonne che compongono una tabella. Questo parametro viene utilizzato per la validazione dei valori contenuti in valueMap.
	 * 	- _state : rappresenta lo stato della tupla.
	 */
	record () throw () : __state(empty), __visible(false) {}
	record (std::unordered_map<std::string, std::string>& valuesMap, std::unordered_map<std::string, column> columnsMap, enum state _state) throw (basic_exception&);

	/* La funzione update consente di marcare i valori di una tupla affinchè siano aggiornati correttamente. Prende i seguenti parametri:
	 * consente di creare una tupla in modo corretto, inserendo opportunamente i dati. I paramentri sono:
	 * 	- valueMap : mappa il cui primo campo è il nome della colonna in cui inserire il valore contenuto nel secondo campo. Se non esiste nessuna colonna con il nome
	 * 				 specificato, viene generata una eccezione di tipo column_not_exists, derivata da access_exception. Se uno dei valori contenuti nel secondo campo
	 * 				 non fosse valido per il tipo di colonna al quale deve corrispondere, viene generata una eccezione derivata da data_exception. Vedi l'header
	 * 				 sqlType.hpp per i dettagli.
	 * 	- columnsMap : mappa delle colonne che compongono una tabella. Questo parametro viene utilizzato per la validazione dei valori contenuti in valueMap.
	 */
	void update (std::unordered_map<std::string, std::string>& valueMap, std::unordered_map<std::string, column> columnsMap) throw (basic_exception&);

	/* La funzione cancel marca una tupla affinchè sia rimossa dal database remoto all'atto del commit.
	 */
	void cancel () throw ();

	/* La funzione erase cancella il contenuto di una tupla, portandola nello stato in cui si fosse trovata se creata col costruttore senza argomenti.
	 */
	void erase () throw ();

	/*	*/
	enum state state () const throw ()
			{return __state;}

	/*	*/
	bool visible () const throw ()
			{return __visible;}

	/* La funzione current restituisce un oggetto unordered_map il cui primo campo contiene il nome di una colonna mentre il secondo campo contiene il corrispettivo
	 * valore memorizzato dalla tupla.
	 * La funzione old restituisce un oggetto unordered_map il cui primo campo contiene il nome di una colonna mentre il secondo campo contiene il corrispettivo
	 * valore precedentemente memorizzato dalla tupla.
	 */
	std::unique_ptr<std::unordered_map<std::string, std::string>> current() const throw ();
	std::unique_ptr<std::unordered_map<std::string, std::string>> old() const throw ();


	/* La funzione size restituisce il numero di byte necessari alla memorizzazione su file della tupla.
	 * La funzione write consente la scrittura della tupla su stream binario. Viene generata una eccezione di tipo storage_exception nel caso in cui la scrittura non
	 * fosse possibile.
	 * La funzione read consente la lettura della tupla su stream binario. Viene generata una eccezione di tipo storage_exception nel caso in cui la lettura non
	 * fosse possibile.
	 */
	std::streamoff size() const throw ();
	void write (std::fstream& stream) const	throw (storage_exception&);
	void read (std::fstream& stream) throw (storage_exception&);

private:
	enum state											__state;
	struct {std::string current;	std::string old;}	value;
	std::unordered_map<std::string, value>				__valueMap;
	bool												__visible;

};	/*	end of record declaration	*/
}; /*	end of openDB namespace	*/
#endif /* TUPLE_HPP_ */
