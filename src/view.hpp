/* Copyright 2013-2014 Salvatore Barone <salvator.barone@gmail.com>
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
#ifndef __OPENDB_VIEW_HEADER__
#define __OPENDB_VIEW_HEADER__

#include "table.hpp"
#include <string>
#include <list>

namespace openDB {
/* Un oggetto view non appartiene ad uno schema di una base di dati in senso stretto, ma contiene informazioni che, invece, vi appartengono. Rappresenta una "vista"
 * un modo per mostrare i dati di un database con una struttura diversa da quella che hanno effettivamente sulla base dati.
 * L'idea è permettere di interagire con essi in maniera più articolata. Il motivo principale dell'introduzione di questo oggetto è dare la possibilità di "collegare"
 * tra loro le colonne di più tabelle, rendendo possibile interagire con essi più facilmente. Si pensi a tabelle collegate tra loro mediante integrità referenziale:
 * attraverso un oggetto view contenente le colonne delle due tabelle, è possibile effettuare operazioni di inserimento/modifica delle informazioni in maniera avanzata.
 * Un utilizzo alternativo può restringere l'insieme delle colonne "visibili" ad un utente.
 */
class view {
public:


private:

};
};
#endif
