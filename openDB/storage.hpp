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

#ifndef __OPENDB_STORAGE_HEADER__
#define __OPENDB_STORAGE_HEADER__

#include "record.hpp"
#include "exception.hpp"

namespace openDB {
class storage {
public :
		storage() throw () : __lastKey(0) {};
		virtual ~storage () {}

		virtual void internalID	(std::unique_ptr<std::list<unsigned long>>& ptr) const throw () = 0;
		virtual std::unique_ptr<std::list<unsigned long>> internalID () const throw () = 0;
		virtual unsigned long numTuples	() const throw () = 0;
		virtual void clear () throw () = 0;

		virtual unsigned long insert (const std::list<std::string>& values, const std::list<column>& structure, enum record::state _status)	throw (storage_exception&) = 0;
		virtual void update (unsigned long ID, const std::list<std::string>& values, const std::list<column>& structure) throw (storage_exception&)	= 0;
		virtual void cancel (unsigned long ID) throw (storage_exception&) = 0;
		virtual void erase (unsigned long ID) throw (storage_exception&)	= 0;

		virtual enum record::state state (unsigned long ID)	const	throw (storage_exception&)	= 0;
		virtual bool visible (unsigned long ID)	const throw (storage_exception&)	= 0;

		std::unique_ptr<std::unordered_map<std::string, std::string>> current(unsigned long ID) const throw ();
		std::unique_ptr<std::unordered_map<std::string, std::string>> old(unsigned long ID) const throw ();

	protected :
		unsigned long 		__lastKey;
}; /* end of storage class definition */

}; /*	end of openDB namespace	*/
#endif
