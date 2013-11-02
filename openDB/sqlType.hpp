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
#ifndef __OPENDB_SQLTYPE_HEADER__
#define __OPENDB_SQLTYPE_HEADER__

#include <string>
#include <limits>
#include "exception.hpp"


/* Il namespace openDB è il namespace del modulo che si occupa dell'astrazione della struttura di un database, della memorizzazione temporanea delle tuple e della generazione di
 * comandi sql da inviare al dbms remoto.
 */
namespace openDB {

/* sqlType è il namespace a cui appartengono le classi che fungono da trasposizione, in linguaggio c++, dei principali tipi di dato sql	come boolean per valori booleani, varchar
 * per le stringhe, eccetera.
 *
 * Ultima modifica:			2 settembre 2013
 * Ultimo test di modulo:	2 settembre 2013
 * Esito del test:			negativo, nessun errore riscontrato
 */
namespace sqlType {

/* type_base è la classe base da cui derivano concettualmente tutti i tipi di dato della trasposizione c++ dei tipi sql. Essa è una classe astratta senza attributi e con soltanto
 * due membri virtuali astratti, validate_value e prepare_value, le quali definiscono la firma delle funzioni per la validazione di un valore e la sua preparazione precedente alla
 * generazione di un comando sql.
 */
class type_base {
public:
		virtual ~type_base() {}

		/* Il compito della funzione validate_value è quello di verificare che un valore, rappresentato dalla stringa value, possa essere tradotto senza errori da un tipo di dato
		 * sql specifico al suo omologo nella trasposizione in linguaggio c++. Nel caso in cui durante la "traduzione" si verifichi un errore oppure nel caso in cui tale traduzione
		 * non sia possibile, viene generata una eccezione di tipo data_exception (vedi header exception.hpp) o derivati.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&) = 0;

		/* Il compito della funzione prepare_value è quello di preparare un valore in modo che esso possa essere parte di un comando sql di inserimento, modifica, cancellazione o
		 * selezione. Ad esempio, per una stringa di tipo varchar, vengono aggiunti gli apici ad inizio e fine del file e vengono "escaped" i caratteri che devono esserlo.
		 * La funzione prepare_value viene richiamata solo in fase di creazione di comandi sql, ciò vuol dire che il valore contenuto in value è corretto per ipotesi.
		 */
		virtual std::string prepare_value(std::string value) const throw () = 0;
};



/* Il tipo sql boolean è la trasposizione in sql del tipo bool in c++. Si tratta di un tipo di dato che può assumere solo due valori: true e false.
 */
class boolean : public type_base {
public:
		/* Per il tipo sql boolean, che viene trasposto come tipo bool in c++, la funzione validate_value controlla che il valore contenuto nella stringa value corrisponda ad uno dei
		 * 14 valori ammessi per rappresentare uno dei due stati logici booleani. Se il valore contenuto in value non corrisponde a nessuno di essi, viene generata una eccezione di
		 * tipo "invalid_argument", derivata di data_exception.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&);

		/* La funzione prepare_value restituisce una stringa contenente "TRUE" nel caso in cui il valore logico booleano sia true, "FALSE" nel caso contrario.
		 */
		virtual std::string prepare_value(std::string value) const throw ();

private :
		static const std::string true_value[];	/*Le stringhe ammesse per rappresentare il valore booleano true sono "TRUE", "t", "true", "y", "yes", "on", "1"*/
		static const std::string false_value[]; /*Le stringhe ammesse per rappresentare il valore booleano false sono, invece, "FALSE", "f", "false", "n", "no", "off", "0"*/
		static const unsigned value_number = 7; /*Ciascuno dei due valori può essere riconosciuto attraverso 7 diverse stringhe*/
};



/* Per il tipo sql date non esiste una trasposizione in c++. Si tratta di un tipo di dato pensato per contenere date nel formato anno/mese/giorno. Usare lo standard risulta scomodo
 * per la maggior parte delle persone, per cui è prevista la possibilità di inserire una data sia in formato anno/mese/giorno che giorno/mese/anno.
 */
class date : public type_base {
public:
		/* Per il tipo date, la funzione validate_value verifica che la stringa contenuta in value contenga una data in formato yyyy/mm/dd, oppure dd/mm/yyyy.
		 * Nel caso in cui value non contenga una stringa il cui significato è una data viene generata una eccezione. Il tipo dell'eccezione, derivato da data_exception, dipende dalla
		 * tipologia di errore. Nel caso in cui non sia stato possibile scomporre la stringa viene generata una eccezione di tipo invalid_argument.
		 * Nel caso in cui la stringa sia scomponibile ma la sua scomposizione risulta ambigua (si pensi a yy/mm/dd o dd/mm/yy) viene generata una eccezione di tipo ambiguous_value
		 * mentre nel caso in cui la scomposizione sia
		 */
		virtual void validate_value(std::string value) const throw(data_exception&);

		/* La funzione prepare_value restituisce una data in formato 'yyyy-mm-dd'. La funzione viene chiamata solo dopo la validazione della data, per cui la stringa contenuta in
		 * value viene supposta corretta e semanticamente corrispondente ad una data.
		 */
		virtual std::string prepare_value(std::string value) const throw ();
private:
	/* Per rendere il codice più leggibile, le operazioni di conversione e verifica di una data vengono demandate alle due funzioni seguenti.
	 * La struttura date_integer serve a contenere una data convertita in formato numerico.
	 */
	struct date_integer {
		unsigned day;
		unsigned month;
		unsigned year;
		date_integer() : day(0), month(0), year(0) {}
	};

	/* La funzione convert cerca di convertire una stringa di caratteri in una data in formato numerico. Se non dovesse riuscirci viene generata una eccezione di tipo invalid_argument
	 * mentre nel caso in cui la conversione dovesse risultare ambigua, genera una eccezione del tipo ambiguous_value.
	 */
	date_integer convert (std::string __value) const throw (data_exception&);

	/* La funzione validate non genera eccezione, ma restituisce true nel caso in cui la data scomposta in formato intero sia corretta, false altrimenti
	 */
	bool validate (date_integer __date) const throw ();
};

/* Per il tipo sql time non esiste una trasposizione in c++. Si tratta di un tipo di dato pensato per contenere tempo nel formato hh:mm:ss. Usare lo standard risulta scomodo
 * per la maggior parte delle persone, per cui è prevista la possibilità di inserire un tempo sia in formato hh:mm che hh:mm:ss.
 */
class time : public type_base {
public:
		/* La funzione validate_value controlla che nell'oggetto value sia contenuta una stringa in formato hh:mm:ss o hh:mm. Nel caso in cui la conversione non fosse possibile
		 * viene generata una eccezione di tipo invalid_argument, derivata di data_exception. Nel caso in cui la conversione fosse possibile, ma il tempo che si ottiene non è valido
		 * allora viene generata una eccezione di tipo invalid_time, sempre derivata da data_exception
		 */
		virtual void validate_value(std::string value) const throw(data_exception&);

		/* La funzione prepare_value per il tipo time, chiamata su un oggetto value il cui contenuto è già stato validato, restituisce una stringa contenente il tempo in formato
		 * hh:mm:ss
		 */
		virtual std::string prepare_value(std::string value) const throw ();
private:
	/* Per rendere il codice più leggibile, le operazioni di conversione e verifica di un tempo vengono demandate alle due funzioni seguenti.
	 * La struttura time_integer serve a contenere un tempo convertito in formato numerico.
	 */
	struct time_integer {
		unsigned hour;
		unsigned minute;
		unsigned second;
		time_integer() : hour(0), minute(0), second(0) {}
	};

	/* La funzione convert cerca di convertire una stringa di caratteri in un tempo in formato numerico. Se non dovesse riuscirci viene generata una eccezione di tipo invalid_argument.
	 */
	time_integer convert (std::string __value) const throw (data_exception&);

	/* La funzione validate non genera eccezione, ma restituisce true nel caso in cui il tempo scomposto in formato intero sia corretto, false altrimenti
	 */
	bool validate (time_integer __time) const throw ();
};

/* L'equivalente c++ del tipo character è la stringa ci caratteri char[]. Il tipo sql charachar prevede che se una stringa ha lunghezza inferiore alla lunghezza massima consentita,
 * i caratteri rimanenti vengano riempiti con uno spazio. questa operazione non viene effettuata in questa sede perchè eseguita automaticamente dal DBMS all'atto dell'inserimento.
 */
class character : public type_base {
public:
		/* Il tipo character va costruito usando il paramentro __length che ne indica la lunghezza. La lunghezza di default per lo standard è 1 mentre la lunghezza massima è 1048576.
		 */
		character (unsigned __length = default_length)
			{(__length <= max_length ? length = __length : length = max_length);}

		/* Nel caso in cui la lunghezza della stringa contenuta in value ecceda la lunghezza consentita, la funzione validate_value genera una eccezione di tipo value_too_long,
		 * derivata da data_exception.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&)
			{if (value.size() > length) throw value_too_long(value + " is too long for character(" + std::to_string(length) + ").");}

		/* L'unica operazione effettuata da prepare_value è aggiungere i single-quote all'inizio ed alla fine della stringa. Come nel caso degli altri tipi, questa funzione va chiamata
		 * soltanto dopo aver validato il valore.
		 */
		virtual std::string prepare_value(std::string value) const throw ()
			{return "'" + value + "'";}

		/*	*/
		unsigned get_length() const throw ()
			{return length;}

private:
		static const unsigned max_length = 1048576;
		static const unsigned default_length = 1;
		unsigned length;
};

/* Varchar è l'equivalente sql del tipo char*. Questo tipo non prevede che i caratteri rimanenti siano riempiti con spazi bianchi.
 */
class varchar : public type_base {
public:
		/* La funzione allocator ha il compito di allocare opportunamente una nuova copia dell'oggetto, inizializzandolo correttamente. Questa funzione torna utile nel
		 * momento in cui è necessario allocare, in maniera polimorfa, nuovi oggetti derivati da type_base evitando l'utilizzo dei puntatori e dell'operatore new.
		 */
		varchar (unsigned __length = default_length)
			{(__length <= max_length ? length = __length : length = max_length);}

		/* Nel caso in cui la lunghezza della stringa contenuta in value ecceda la lunghezza consentita, la funzione validate_value genera una eccezione di tipo value_too_long,
		 * derivata da data_exception.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&)
			{if (value.size() > length) throw value_too_long(value + " is too long for varchar(" + std::to_string(length) + ").");}

		/* L'unica operazione effettuata da prepare_value è aggiungere i single-quote all'inizio ed alla fine della stringa. Come nel caso degli altri tipi, questa funzione va chiamata
		 * soltanto dopo aver validato il valore.
		 */
		virtual std::string prepare_value(std::string value) const throw ()
			{return "'" + value + "'";}

		/*	*/
		unsigned get_length() const throw ()
			{return length;}

private:
		static const unsigned max_length = 1048576;
		static const unsigned default_length = 1048576;
		unsigned length;
};

/* Per il tipo smallint non esiste un corrispettivo in c++. Si tratta, comunque, di un tipo numerico intero con segno codificato su 16 bit, per cui il suo range di valori si estende
 * da -32768 a 32767
 */
class smallint : public type_base {
public:
		/* La funzione validate_value si occupa di verificare che la stringa contenuta in value sia convertibile in un numero intero. Nel caso non sia possibile viene generata una
		 * eccezione del tipo invalid_argument. Nel caso la conversione sia possibile ma il valore che si ottiene è fuori dal range di valori per questo tipo, viene generata una
		 * eccezione del tipo out_of_bound. Entrambi i tipi di eccezione sono derivati dal tipo data_exception.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&);

		/* La funzione prepare_value chiamata su un oggetto di questo tipo, sempre successivamente alla validazione, restituisce la stringa così com'è in quanto non è necessaria
		 * nessuna operazione di preparazione
		 */
		virtual std::string prepare_value(std::string value) const throw ()
			{return value;}

private:
		static const int min = -32768;	/*	limite superiore del bound dei valori	*/
		static const int max = 32767;	/*	limite inferiore del bound dei valori	*/
};



/*	*/
class integer : public type_base {
public:
		/* La funzione validate_value si occupa di verificare che la stringa contenuta in value sia convertibile in un numero intero. Nel caso non sia possibile viene generata una
		 * eccezione del tipo invalid_argument. Nel caso la conversione sia possibile ma il valore che si ottiene è fuori dal range di valori per questo tipo, viene generata una
		 * eccezione del tipo out_of_bound. Entrambi i tipi di eccezione sono derivati dal tipo data_exception.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&);

		/*  */
		virtual std::string prepare_value(std::string value) const throw ()
			{return value;}
private:
		static const long int min;	/*	limite superiore del bound dei valori	*/
		static const long int max;	/*	limite inferiore del bound dei valori	*/
};



/*	*/
class bigint : public type_base {
public:
		/* La funzione validate_value si occupa di verificare che la stringa contenuta in value sia convertibile in un numero intero. Nel caso non sia possibile viene generata una
		 * eccezione del tipo invalid_argument. Nel caso la conversione sia possibile ma il valore che si ottiene è fuori dal range di valori per questo tipo, viene generata una
		 * eccezione del tipo out_of_bound. Entrambi i tipi di eccezione sono derivati dal tipo data_exception.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&);

		/* La funzione prepare_value chiamata su un oggetto di questo tipo, sempre successivamente alla validazione, restituisce la stringa così com'è in quanto non è necessaria
		 * nessuna operazione di preparazione
		 */
		virtual std::string prepare_value(std::string value) const throw ()
			{return value;}

private:
		static const long long min;	/*	limite superiore del bound dei valori	*/
		static const long long max;	/*	limite inferiore del bound dei valori	*/
};



/* Il tipo sql real è l'equivalente del float c++. Si tratta di un numero reale in virgola mobile in singola precisione, vale a dire con 6 cifre significative dopo il separatore dei
 * decimali.
 * Si tratta di un tipo di dato "inesatto", nel senso che un numero di questo tipo non sempre viene espresso esattamente: potrebbe accadere, infatti, che un numero venga approssimato
 * al numero reale che minimizza la differenza ed è rappresentabile con lo stesso numero di bit per mantissa ed esponente.
 * Il massimo errore che si commette durante l'approssimazione varia da implementazione ad implementazione ma può sempre essere ottenuto mediante la funzione
 * std::numeric_limits<long double>::epsilon(), definita in <limits>. 
 */
class real : public type_base {
public:
		/* La funzione validate_value si occupa di verificare che la stringa contenuta in value sia convertibile in un numero reale. Nel caso non sia possibile viene generata una
		 * eccezione del tipo invalid_argument. Nel caso la conversione sia possibile ma il valore che si ottiene è fuori dal range di valori per questo tipo, viene generata una
		 * eccezione del tipo out_of_bound. Entrambi i tipi di eccezione sono derivati dal tipo data_exception.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&);

		/* La funzione prepare_value chiamata su un oggetto di questo tipo, sempre successivamente alla validazione, restituisce la stringa così com'è in quanto non è necessaria
		 * nessuna operazione di preparazione
		 */
		virtual std::string prepare_value(std::string value) const throw ()
			{return value;}

private:
		static const float min;	/*	limite superiore del bound dei valori	*/
		static const float max;	/*	limite inferiore del bound dei valori	*/
};

/* Il tipo sql double precision è l'equivalente del long double c++. Si tratta di un numero reale in virgola mobile in doppia precisione, vale a dire con 15 cifre significative
 * dopo il separatore dei decimali.
 * Si tratta di un tipo di dato "inesatto", nel senso che un numero di questo tipo non sempre viene espresso esattamente: potrebbe accadere, infatti, che un numero venga approssimato
 * al numero reale che minimizza la differenza ed è rappresentabile con lo stesso numero di bit per mantissa ed esponente.
 * Il massimo errore che si commette durante l'approssimazione varia da implementazione ad implementazione ma può sempre essere ottenuto mediante la funzione
 * std::numeric_limits<long double>::epsilon(), definita in <limits>.
 */
class double_precision : public type_base {
public:
		/* La funzione validate_value si occupa di verificare che la stringa contenuta in value sia convertibile in un numero reale. Nel caso non sia possibile viene generata una
		 * eccezione del tipo invalid_argument. Nel caso la conversione sia possibile ma il valore che si ottiene è fuori dal range di valori per questo tipo, viene generata una
		 * eccezione del tipo out_of_bound. Entrambi i tipi di eccezione sono derivati dal tipo data_exception.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&);

		/* La funzione prepare_value chiamata su un oggetto di questo tipo, sempre successivamente alla validazione, restituisce la stringa così com'è in quanto non è necessaria
		 * nessuna operazione di preparazione
		 */
		virtual std::string prepare_value(std::string value) const throw ()
			{return value;}

private:
		static const long double min;	/*	limite superiore del bound dei valori	*/
		static const long double max;	/*	limite inferiore del bound dei valori	*/
};

/* Per il tipo di dato numeric non esiste un equivalente c++. Si tratta di un tipo di dato numerico intero o reale definito dall'utente mediante due parametri, precision e scale,
 * il cui significato viene spiegato nel prossimo blocco di commento.
 * Il tipo numeric è un tipo di dato "esatto", vale a dire che non vi è nessun tipo di approssimazione. 
 */
class numeric : public type_base {
public:
		/* Il tipo numeric viene definito mediante due paramentri:
		 * 		- precision: è il numero totale di cifre significative dell'intero numero ossia il numero di cifre totali a destra e a sinistra del punto "." che separa la parte
		 * 		  intera dalla parte frazionaria.
		 * 		- scale: è il numero di cifre significative della parte frazionaria, vale a dire il numero di cifre a destra del punto.
		 * Quindi, ad esempio, il numero 123.4567 ha una precisione di 7 cifre e una scala di 4 cifre.
		 */
		numeric (unsigned __precision = default_precision, unsigned __scale = default_scale)
			{(__precision <= max_precision ? precision = __precision : precision = max_precision);
			(__scale <= max_scale ? scale = __scale : scale = max_scale);}

		/* La funzione validate_value si occupa di verificare che la stringa contenuta in value sia convertibile in un numero intero o reale a seconda della scala impostata e che
		 * siano rispettati i vincoli su precisione e scala. Se il numero di cifre o il numero di cifre significative eccedesse il limite, viene generata una eccezione del tipo
		 * out_of_bound.
		 * Nel caso non sia possibile convertire la stringa in un tipo numerico, viene generata una eccezione del tipo invalid_argument.
		 * Nel caso la conversione sia possibile ma il valore che si ottiene è fuori dal range di valori per double precision, viene generata una eccezione del tipo out_of_bound.
		 * Entrambi i tipi di eccezione sono derivati dal tipo data_exception.
		 */
		virtual void validate_value(std::string value) const throw(data_exception&);

		/* La funzione prepare_value chiamata su un oggetto di questo tipo, sempre successivamente alla validazione, restituisce la stringa così com'è in quanto non è necessaria
		 * nessuna operazione di preparazione
		 */
		virtual std::string prepare_value(std::string value) const throw ()
			{return value;}

private:
		static const unsigned max_precision = 1000;				/*	massimo numero di cifre		*/
		static const unsigned default_precision = 1000;			/*	numero di cifre di default	*/
		static const unsigned max_scale = 1000;					/*	numero massimo di cifre significative	*/
		static const unsigned default_scale = 0;				/*	numero di cifre significative di default	*/
		unsigned precision;										/*	massimo numero di cifre	impostato	*/
		unsigned scale;											/*	massimo numero di cifre	significative impostato	*/
};

};	/*	end of sqlType namespace */
};	/*	end of openDB namespace */
#endif
