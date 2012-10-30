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

#include <cstdio>
#include <cstring>
#include <cctype>
#include <unistd.h>
#include "mxgui/display.h"
#include "mxgui/misc_inst.h"
#include "mxgui/entry.h"
#include "misc_drivers.h"
#include "img/logo2.h"

using namespace std;
using namespace miosix;
using namespace mxgui;

volatile bool status;

typedef Gpio<GPIOA_BASE,0> button;

void* keyboardThread(void*)
{
	Display& display=Display::instance();
	button::mode(Mode::INPUT);
	bool oldx=false;
	for(;;)
	{
		bool newx=button::value();
		if(newx && !oldx)
		{
			status=!status;
			if(status)
			{
				printf("\n-->A\n");
				DrawingContext dc(display);
				dc.clear(Point(0,0),Point(dc.getWidth()-1,tahoma.getHeight()),white);
				dc.write(Point(0,0),"Sede aperta");
			} else {
				printf("\n-->C\n");
				DrawingContext dc(display);
				dc.clear(Point(0,0),Point(dc.getWidth()-1,tahoma.getHeight()),white);
				dc.write(Point(0,0),"Sede chiusa");
			}
		}
		oldx=newx;
		Thread::sleep(200);
	}
	//for(;;) printf("\n-->%c\n",Keyboard::instance().getKey());
}

void* tempThread(void*)
{
	for(;;)
	{
		sleep(120);
		printf("\n-->T %4.1f\n",Temperature::instance().get());
	}
}

ENTRY()
{
    Display& display=Display::instance();
	{
		DrawingContext dc(display);
		dc.clear(white);
		dc.setFont(tahoma);
		dc.drawImage(Point(0,13),logo2);
		dc.write(Point(0,13+1+logo2.getHeight()),"** Message line TODO");
	}

	pthread_t t1, t2;
	pthread_create(&t1,0,keyboardThread,0);
	pthread_create(&t2,0,tempThread,0);

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
		if(strcmp(line,"open")==0)
		{
			status=true;
			DrawingContext dc(display);
			dc.clear(Point(0,0),Point(dc.getWidth()-1,tahoma.getHeight()),white);
			dc.write(Point(0,0),"Sede aperta");
		} else if(strcmp(line,"closed")==0)
		{
			status=false;
			DrawingContext dc(display);
			dc.clear(Point(0,0),Point(dc.getWidth()-1,tahoma.getHeight()),white);
			dc.write(Point(0,0),"Sede chiusa");
		}
	}
}
