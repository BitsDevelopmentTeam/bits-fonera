/***************************************************************************
 *   Copyright (C) 2011, 2012 by Terraneo Federico                         *
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

 // Drivers for temperature sensor and keyboard
 
#ifndef MISC_DRIVERS
#define MISC_DRIVERS

#include <miosix.h>

class Temperature
{
public:
	static Temperature& instance();

	int get();
private:
	Temperature(const Temperature&);
	Temperature& operator=(const Temperature&);

	Temperature();
};

class Keyboard
{
public:
	static Keyboard& instance();

	char getKey();

	//returns '-' if no char available
	char getKeyNonBlocking();
private:
	Keyboard(const Keyboard&);
	Keyboard& operator= (const Keyboard&);

	static void launcher(void* argv)
	{
		reinterpret_cast<Keyboard*>(argv)->keybThread();
	}

	void keybThread();

	Keyboard();

	miosix::Queue<char,3> keyPressed;
};

#endif //MISC_DRIVERS