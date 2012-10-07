/***************************************************************************
 *   Copyright (C) 2011 by Yury Kuchura                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#ifndef MXGUI_LIBRARY
#error "This is header is private, it can be used only within mxgui."
#error "If your code depends on a private header, it IS broken."
#endif //MXGUI_LIBRARY

#ifndef EVENT_WIN_H
#define	EVENT_WIN_H

#if !defined(_MIOSIX) && defined(_WINDOWS)

#include "mxgui/level2/input.h"

namespace mxgui {

/**
 * This function is only available in this backend because the Qt GUI app
 * requires a function to add event to the queue.
 */
void addEvent(Event e);

/**
 * Implementation class to handle events in the Windows backend
 */
class InputHandlerImpl
{
public:
    /**
     * \return an event, blocking
     */
    Event getEvent();

    /**
     * \return an event, nonblocking. A default constructed event is returned
     * if there are no events.
     */
    Event popEvent();
};

} //namespace mxgui

#endif //!defined(_MIOSIX) && defined(_WINDOWS)

#endif //EVENT_WIN_H 