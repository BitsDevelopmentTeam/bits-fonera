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
#include <miosix.h>
#include "misc_drivers.h"
#include "logo.h"
#include "logo-open.h"
#include "logo-closed.h"
#include "mxgui/display.h"
#include "mxgui/misc_inst.h"

using namespace std;
using namespace miosix;
using namespace mxgui;

typedef Gpio<GPIOD_BASE,14> redLed;
typedef Gpio<GPIOD_BASE,12> greenLed;

void keyboardThread(void*)
{
	for(;;) printf("\n-->%c\n",Keyboard::instance().getKey());
}

void tempThread(void*)
{
	for(;;)
	{
		Thread::sleep(120*1000);
		printf("\n-->T %4.1f\n",Temperature::instance().get());
	}
//TODO: incremental temperature
// 	int oldTemp=Temperature::instance().get();
// 	printf("\n-->T %4.1f\n",oldTemp);
// 	for(;;)
// 	{
// 		Thread::sleep(120*1000);
// 		int newTemp=Temperature::instance().get();
// 		if(newTemp==oldTemp) continue;
// 		oldTemp=newTemp;
// 		printf("\n-->T %4.1f\n",oldTemp);
// 	}
}

int main()
{
	Display& display=Display::instance();
	{
		DrawingContext dc(display);
		dc.setFont(miscFixed);
		dc.write(Point(70,20),"Hello world");
		dc.line(Point(0,0),Point(255,127),black);
		dc.clear(Point(10,10),Point(20,20),black);
		for(int i=0;i<10;i++) dc.line(Point(3*i,64),Point(3*i,127),black);
		
	}
	redLed::mode(Mode::OUTPUT);
	greenLed::mode(Mode::OUTPUT);
	redLed::low();
	greenLed::low();
	Thread::create(keyboardThread,2048);
	Thread::create(tempThread,2048);

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
			redLed::low();
			greenLed::high();
// 			memcpy(framebuffer,logo_open,4096);
		} else if(strcmp(line,"closed")==0)
		{
			greenLed::low();
			redLed::high();
// 			memcpy(framebuffer,logo_closed,4096);
		}
	}
}
