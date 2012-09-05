 /***************************************************************************
 *   Copyright (C) 2011 by Terraneo Federico                               *
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

 // Driving a framebuffer-less LCD with SPI + DMA
 // Code designed for use with the Miosix kernel.

#include <cstdio>
#include <cstring>
#include <cctype>
#include <miosix.h>
#include "bitsboard_display.h"
#include "misc_drivers.h"
#include "logo.h"
#include "logo-open.h"
#include "logo-closed.h"

using namespace std;
using namespace miosix;

void keyboardThread(void*)
{
	for(;;) iprintf("\n-->%c\n",Keyboard::instance().getKey());
}

void tempThread(void*)
{
	int oldTemp=Temperature::instance().get();
	iprintf("\n-->T %d\n",oldTemp);
	for(;;)
	{
		Thread::sleep(120*1000);
		int newTemp=Temperature::instance().get();
		if(newTemp==oldTemp) continue;
		oldTemp=newTemp;
		iprintf("\n-->T %d\n",oldTemp);
	}
}

int main()
{
	Thread::create(keyboardThread,2048);
	Thread::create(tempThread,2048);
	initializeDisplay();
	memcpy(framebuffer,logo,4096);

	for(;;)
	{
		char line[256];
		fgets(line,sizeof(line),stdin);
		for(;;)
		{
			int l=strlen(line);
			if(l==0 || (line[l-1]!='\r' && line[l-1]!='\n')) break;
			line[l-1]='\0';
		}
		if(strcmp(line,"open")==0) memcpy(framebuffer,logo_open,4096);
		else if(strcmp(line,"closed")==0) memcpy(framebuffer,logo_closed,4096);
	}
}
