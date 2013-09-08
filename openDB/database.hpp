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

namespace openDB {
/*
 */
class database {
public:
		/*
		 */
		explicit database(std::string databaseName) throw (storage_exception&);

		/* Questa funzione restituisce il nome di undatabase.
		 */
		std::string name() const throw ()
			{return __databaseName;}

		/*
		 */
		void add_schema(std::string schemaName) throw (schema_exists&)
			{(find_schema(schemaName) ? throw("Schema '" + schemaName + "' already exists in database '" + __databaseName + "'") : __schemasMap.emplace(schemaName, schema(schemaName, __storageDirectory)));}

		/*
		 */
		unsigned number_of_schemas() const throw ()
			{return __schemasMap.size();}

		/*
		 */
		std::unique_ptr<std::list<std::string>> schemas_name (bool attach_database_name = false) const throw ();

		/* La funzione find_schema restituisce true se uno schema con lo stesso nome di quello indicato compone la struttura del database. Restituisce false altrimenti.
		 */
		bool find_schema(std::string schemaName) const throw ()
			{ if( __schemasMap.find(schemaName) != __schemasMap.end()) return true; return false;}

		/* La funzione drop_schema consente l'eliminazione di un schema dalla struttura del database, unitamente alle colonne che lo compongono. Se lo schema non esiste viene
		 * generata una eccezione di tipo schema_not_exists, derivata da access_exception.
		 */
		void drop_schema(std::string schemaName) throw (schema_not_exists&)
			{__schemasMap.erase(get_iterator(schemaName));}

		/*
		 *
		 */
		schema& get_schema(std::string schemaName) throw (schema_not_exists&)
			{return get_iterator(schemaName)->second;}
		const schema& get_schema(std::string schemaName) const throw (schema_not_exists&)
			{return get_iterator(schemaName)->second;}
		
private:
		std::string 								__databaseName;			/*	nome del database.	*/
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
};	/*	end of	database declaration	*/
};	/*	end of openDB namespace	*/
#endif
