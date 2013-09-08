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
#ifndef __OPENDB_QUERY_ATTRIBUTE_HEADER__
#define __OPENDB_QUERY_ATTRIBUTE_HEADER__

#include <string>

namespace openDB {

/* La classe query_attribute contiene le informazioni relative all'utilizzo di una colonna nell'ambito di una operazione di interrogazione al DBMS.
 * In particolare conserva informazioni relative all'uso della colonna in operazioni di proiezione, selezione (con relativo valore da utilizzare
 * per a selezione), l'operatore di confrontoi da usare, informazioni relative all'uso della colonna in operazioni di ordinamento r modalita' di
 * ordinamento.
 * 
 * Ultima modifica:			2 settembre 2013
 * Ultimo test di modulo:	2 settembre 2013
 * Esito del test:			negativo, nessun errore riscontrato
 */
class query_attribute {
public:

		/* Il tipo enumerativo sqlComOdefinisce una forma di corrispondenza con gli operatori di confronto sql. Gli operatori supportati 
		 * sono:
		 * 	- more: corrisponde all'operatore maggiore ">";
		 * 	- moreEqual: corrisponde all'operatore maggiore o uguale ">=";
		 * 	- equal: corrisponde all'operatore di eguaglianza "=";
		 * 	- disequal: corrisponde all'operatore diverso, non uguale, "!=";
		 * 	- like: corrisponde all'operatore di confronto "like"  per stringhe;
		 * 	- notLike: duale all'operatroe like per il confronto tra stringhe;
		 * 	- lessEqual: corrisponde all'operatore minore o uguale, "<=";
		 * 	- less: corrisponde all'operatore minore "<";
		 * 	- in: operatore di "uguaglianza multipla", consente di verificare se un valore appartiene ad un certo insieme di valori;
		 * 	- notIn: duale dell'operatore "in".
		 */
		enum sqlCompOp	{more, moreEqual, equal, disequal, like, notLike, lessEqual, less, in, notIn};

		/* Il tipo enumerativo sqlOrder definiscluna modita' di ordinamentoto da utilizzare per una colonna. La modalita' "asc" ordina
		 * i valori di una colonna in modo crescente, la modalita' "desc" fa in modo che i valori della colonna vengano ordinati in modo
		 * decrescente.
		 */
		enum sqlOrder	{asc, desc};

		/* Gli oggetti di questo tipo sono concepiti percontenere le informazioni drelative all'utilizzo delle colonne nelle query di
		 * interrogazione al DBMS. Per questo motivo viene fornito un solo costruttore, il quale costruisce un oggetto in modo che la
		 * colonna a cui venisse assegnato non venga utilizzata in nessuna operazione riguardo interrogazioni al DBMS.
		 */
		query_attribute() :
			__project(false),
			__select(false),
			__select_value(""),
			__compare_operator(equal),
			__order_by(false),
			__order_mode(asc)
			{}

		/* La funzione project con parametro booleano value, consente di settare le impostazioni per l'utilizzo della query nelle operazioni
		 * di proiezione. La sua versione sovracaricata restituisce il valore booleano "true" se la colonna deve essere usata per operazioni
		 * di proiezione, "false" altrimenti.
		 */
		void project (bool value) throw ()
				{__project = value;}
		bool project () const throw ()
				{return __project;}

		/* La funzione project con parametro booleano value, consente di settare le impostazioni per l'utilizzo della query nelle operazioni
		 * di selezione. La sua versione sovracaricata restituisce il valore booleano "true" se la colonna deve essere usata per operazioni
		 * di selezione, "false" altrimenti.
		 */
		void select (bool value) throw ()
				{__select = value;}
		bool select () const throw ()
				{return __select;}

		/* LA funzione selectValue con parametre value di tipo stringa consente di impostare il valore da usare come discriminante durante le
		 * operazioni di selezione. E' possibile inserire anche piu' di un valore conil seguente formato: (valore1, valore2, valore3, eccetera).
		 * Nel caso in cui si inserisca piu' di un valore nel formato sovraillustrato, e' necessario utilizzare solo gli operatori "in" e
		 * "notIn". Nessuna delle funzioni pone limitazioni in questo senso, ma un mancato utilizzo degli operatori "in" e "notIn" potrebbe
		 * causare errori nell'esecuzione di query su alcuni DBMS.
		 * La versione sovraccaricata, senza argomenti, restituisce il valore, o i valori, impostati.
		 */
		void selectValue (std::string value) throw ()
				{__select_value = value;}
		std::string selectValue () const throw ()
				{return __select_value;}

		/* La funzione compareOperator, con paramentro op di tipo enumerativo sqlCOmpOp, consente di impostare l'operatore di confronto. Quando
		 * viene impostato piu' di un valore di selezione (vedi funzione selectValue) e' necessario utilizzare gli operatori "in" e notIn". La
		 * funzione non pone nessun limite in questo senso, ma un mancato utilizzo di tali operatori potrebbe causare errori nell'esecuzione di
		 * query su alcuni DBMS.
		 * La funzione compareOperator senza argomenti restituisce una stringa contenente la traduzione in caratteri dell'operatore di confronto
		 * impostato. Questo in ottica dell'utilizzo delle informazioni contenute nell'oggetto durante la generazione di comandi sql da inviare
		 * al DBMS. Per ottenere il valore enumerativo dell'operatore di confronto si utilizzi compareOperator_enum.
		 */
		void compareOperator (sqlCompOp op) throw ()
				{__compare_operator = op;}
		std::string compareOperator() const throw ();
		enum sqlCompOp compareOperator_enum () const throw ()
				{return __compare_operator;}

		/* La funzione orderBy con argomento booleano value, consente di settare le impostazioni riguardo all'uso della colonna nelle operazioni
		 * di ordinamento. La sua versione sovraccaricata, senza argomenti, restituisce "true" se la colonna verra' usata nelle operazioni di
		 * ordinamento, "false" altrimenti.
		 */
		void orderBy (bool value) throw ()
				{__order_by = value;}
		bool orderBy () const throw ()
				{return __order_by;}

		/* La funzione orderMode consente di impostare le modalita' con cui viene effettuato l'ordinamento.  E' possibile ordinare i valori di una
		 * colonna sia in modo crescente, passando come parametro il valore "asc", sia in maniera decrescente, passando come valore "desc".
		 * La funzione orderMode senza argomenti restituisce una stringa contenente la traduzione della modalita di ordinamento. Per ottenere
		 * la modalita' di ordinamento in tipo enumerativo sqlOrder si usi orderMode_enum.
		 */
		void orderMode (enum sqlOrder order) throw ()
				{__order_mode = order;}
		std::string orderMode () const throw ();
		enum sqlOrder orderMode_enum () const throw ()
				{return __order_mode;}

private:
		bool 			__project;			/*	true se la colonna viene usata in proiezione	*/
		bool 			__select;			/*	true se la colonna viene usata in selezione	*/
		std::string 	__select_value;		/*	valore usato per la selezione	*/
		enum sqlCompOp	__compare_operator;	/*	operatore di confronto usato in selezione	*/
		bool 			__order_by;			/*	true se si deve ordinare i risultati in base ai valori della colonna	*/
		enum sqlOrder	__order_mode;		/*	modo in cui devino essere ordinati i risultati	*/
};	/*	class query_attribute	*/
};	/*	end of openDB namespace	*/
#endif
