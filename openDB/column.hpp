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
#ifndef __OPENDB_COLUMN_HEADER__
#define __OPENDB_COLUMN_HEADER__

#include <string>
#include <memory>
#include "sqlType.hpp"
#include "queryAttribute.hpp"
#include "exception.hpp"

namespace openDB {
/* L'oggetto column astrae il concetto di colonna. Il punto di vista che bisogna tenere guardando questo oggetto noe' quello di colonna visto come
 * insieme di valori, ma come insiame di valori con le stesse proprieta'. Questo oggetto, infatti, non conserva nessun valore relativo alla colonna,
 * ma soltanto le sue proprieta', come il nome, il tipo, e l'uso che si fa deli valori contenuti nela colonna quando vengono generate query sql di
 * interrogazione al database.
 *
 * Ultima modifica:			4 settembre 2013
 * Ultimo test di modulo:	5 settembre 2013
 * Esito del test:			negativo, nessun errore riscontrato
 */
class column {
public:
		/* Per costruire un oggetto di tipo column sono necessari tre parametri:
		 * 	- columnName: il nome della colonna. Dovrebbe essere univoco all'interno di una stessa tabella (vedi oggetto table, definito in table.hpp)
		 *  - columnType: il tipo della colonna. Per creare una colonna di tipo varchar(20), ad esempio, si può procedere come segue:
		 * 			column _column("colonna", new sqlType::varchar(10));
		 * 	 per costruire una colonna di tipo boolean, invece
		 * 			column _column("colonna", new sqlType::boolean);
		 * - key : se questo paramentro assume valore true, allora vuol dire che la colonna compone la chiave per la tabella a cui appartiene. È previsto
		 * 		   un controllo sulle colonne chiave: quando si valida un valore con la funzione validate_value (vedi sotto), essa genera una eccezione di
		 * 		   tipo key_empty, derivata di data_exception, se la stringa che si passa alla funzione è vuota.
		 *
		 *  LA SEGUENTE SITUAZIONE È DA EVITARE
		 * 		openDB::sqlType::type_base* base_ptr = new openDB::sqlType::boolean;
		 *		{
		 *		openDB::column _column("colonna", base_ptr, true);
		 *		try {_column.validate_value("");}
		 *		catch(openDB::data_exception& e) {cout <<e.what() <<endl;}
		 *		}
		 *		try {base_ptr -> validate_value("scemo");}
		 *		catch(openDB::data_exception& e) {cout <<e.what() <<endl;}
		 * 	ESEGUENDO QUESTO CODICE VIENE GENERATO ERRORE DI SEGMENTAZIONE. È UNA COSA VOLUTA. IL DISTRUTTORE DI COLUMN DEALLOCA AUTOMATICAMENTE LO SPAZIO
		 * OCCUPATO DALL'OGGETTO PUNTATO DAL MEMBRO __columnType;
		 */
		column (std::string columnName,	sqlType::type_base* columnType, bool key = false, std::string defaultValue = "")	throw () :
			__columnName(columnName),
			__columnType(columnType),
			__isKey(key),
			__defaultValue(defaultValue)
			{}

		/* Questa funzione restituisce il nome di una colonna. Il nome di ogni colonna all'interno di una stessa tabella dovrebbe essere univoco (vedi oggetto
		 * table, definito in table.hpp) perché usato per l'accesso ad esse.
		 */
		std::string name() const throw ()
			{return __columnName;}
 
		/* Il compito della funzione validate_value è quello di verificare che un valore, rappresentato dalla stringa value, possa essere tradotto senza errori da un tipo di dato
		 * sql specifico al suo omologo nella trasposizione in linguaggio c++. Nel caso in cui durante la "traduzione" si verifichi un errore oppure nel caso in cui tale traduzione
		 * non sia possibile, viene generata una eccezione di tipo data_exception (vedi header exception.hpp) o derivati.
		 * In primis, se la colonna è chiave, di qualunque tipo essa sia, e la stringa che viene passata alla funzione è una stringa vuota, viene generata una eccezione di tipo
		 * key_empty, sempre derivata da data_exception.
		 * La funzione validate_value può generare uno dei seguenti tipi di eccezione:
		 * - invalid_argument : se
		 * 			a) la colonna è di tipo booleano e il parametro value contiene una stringa non ammessa per questo tipo di dato. Per i dettagli, consultare "sqlType.hpp";
		 * 			b) la colonna è di tipo date o time e non è stato possibile convertire correttamente
		 * 			c) la colonna è di un tipo numerico intero (smallint, integer o bigint) o in virgola mobile (float o double precision) o numeric e non è possibile convertire
		 * 			   correttamente la stringa contenuta in value nel corrispondente numero;
		 * - ambiguous_value : se la colonna è di tipo date e non è possibile stabilire il formato da cui convertire la data;
		 * - invalid_date : se la colonna è di tipo date e, traducendo da stringa a data, la data che si ottiene non è valida;
		 * - invalid_time : se la colonna è di tipo time e, traducendo da stringa a time, l'orario che si ottiene non è valido;
		 * - out_of_bound : se la colonna è di un tipo numerico intero (smallint, integer o bigint) o in virgola mobile (float o double precision) o numeric e il numero che si
		 * 					ottiene non rientra nel range di valori del tipo della colonna;
		 * - value_too_long: se la colonna è di tipo character o varchar e la lunghezza della stringa è più lunga della dimensione massima consentita.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&)
			{((__isKey && value=="") ? throw key_empty("'" + __columnName + "' is key! Empty strings aren't allowed!") : __columnType->validate_value(value));}

		/* Il compito della funzione prepare_value è quello di preparare un valore in modo che esso possa essere parte di un comando sql di inserimento, modifica, cancellazione o
		 * selezione. Ad esempio, per una stringa di tipo varchar, vengono aggiunti gli apici ad inizio e fine del file e vengono "escaped" i caratteri che devono esserlo.
		 * La funzione prepare_value viene richiamata solo in fase di creazione di comandi sql, ciò vuol dire che il valore contenuto in value è corretto per ipotesi.
		 * Per i dettagli, si consulti l'header "sqlType.hpp".
		 */
		std::string prepare_value(std::string value) const throw ()
			{return __columnType->prepare_value(value);}

		/* La funzione is_key, con argomento booleano key, consente di impostare una colonna affinchè venga considerata chiave o parte della chiave di una tabella.
		 * Non vi sono limitazioni sul suo uso, ma la si usi con cautela poichè è previsto un controllo sulle colonne chiave: quando si valida un valore con la
		 * funzione validate_value (vedi sotto), essa genera una eccezione di tipo key_empty, derivata di data_exception, se la stringa che si passa alla funzione è vuota.
		 * La versione sovraccarica, senza argomenti, restituisce true se la colonna compone la chiave per la tabella alla quale appartiene.
		 */
		void is_key(bool key) throw ()
			{__isKey = key;}
		bool is_key() const throw ()
			{return __isKey;}

		/* La funzione set_attribute con un argomento consente di impostare i parametri per l'utilizzo della colonna nelle operazioni di interrogazione a DBMS.
		 * Per i dettagli si consulti l'header "queryAttribute.hpp"
		 * La funzione get_attribute, senza argomenti, consente di recuperare le impostazioni settate riguardo l'utilizzo della colonna nelle operazioni di interrogazione a DBMS.
		 */
		void set_attribute(const query_attribute& _attr) throw ()
			{__query_attribute = _attr;}
		const query_attribute& get_attribute () const throw ()
			{return __query_attribute;}

		/* La funzione default value con argomendi tipo stringa, consente di impostare il valore di default. Il valore di default viene utilizzato in fase di inserimento di una
		 * riga in una tabella (vedi oggetto di tipo table, header table.hpp).
		 * La versione senza argomenti restituisce il valore di default precedentemente impostato.
		 */
		void default_value(std::string defaultValue) throw ()
			{__defaultValue = defaultValue;}
		std::string default_value() const throw ()
			{return __defaultValue;}

private:
		std::string								__columnName;			/*	nome della colonna	*/
		std::unique_ptr<sqlType::type_base>		__columnType;			/*	tipo della colonna	*/
		bool									__isKey;				/*	se la colonna è chiave, o concorre alla formazione della chiave, questo attributo è true	*/
		std::string								__defaultValue;			/*	valore di default per la colonna	*/
		query_attribute							__query_attribute;		/*	attributi aggiuntivi relativi all'utilizzo della colonna durante le query di interrogazione al DBMS	*/

};	/*	end of column class	*/
};	/*	end of openDB namespace	*/
#endif
