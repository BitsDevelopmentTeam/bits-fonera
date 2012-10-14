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

/*
     MXGUI Drawboard example
*/

#include "mxgui/entry.h"
#include "mxgui/display.h"
#include "mxgui/misc_inst.h"
#include "mxgui/level2/input.h"
#include <cstdio>
#include <unistd.h>

using namespace mxgui;

#ifdef _MIOSIX
using namespace miosix;
#else //_MIOSIX
#define siprintf sprintf //On a non MCU environment siprintf doesn't exist
#define ledOn() ;
#define ledOff() ;
#endif //_MIOSIX

static const short int LEGEND0_B = 140;
static const short int LEGEND0_E = 160;
static const short int LEGEND1_B = 165;
static const short int LEGEND1_E = 185;
static const short int LEGEND2_B = 190;
static const short int LEGEND2_E = 210;
static const short int LEGEND3_B = 215;
static const short int LEGEND3_E = 235;
static const short int LEGEND_H  = 15;

static char txt[50];

void OutCoord(DrawingContext& dc, Point& p)
{
    siprintf(txt, "x:%04d y:%04d", p.x(), p.y());
    dc.write(Point(0,0), txt);
} // OutCoord

void DrawLegend(DrawingContext& dc)
{
    dc.drawRectangle( Point(LEGEND0_B, 0),
                      Point(LEGEND0_E, LEGEND_H),
                      black);
    //dc.clear() when passed two points produces a filled rectangle
    dc.clear(
                   Point(LEGEND1_B, 0),
                   Point(LEGEND1_E, LEGEND_H),
                   black);
    dc.clear(
                   Point(LEGEND2_B, 0),
                   Point(LEGEND2_E, LEGEND_H),
                   black);
    dc.clear(
                   Point(LEGEND3_B, 0),
                   Point(LEGEND3_E, LEGEND_H),
                   black);
} // DrawLegend
#include "logo2.h"
ENTRY()
{
    Display& display=Display::instance();
	for(int i=0;;i++)
	{
		{
			DrawingContext dc(display);
			dc.clear(white);
			dc.setFont(tahoma);
			dc.write(Point(0,0),"08:19:49 12/10/2012 | Sede aperta | 23.5 C");
			//dc.drawImage(Point(0,13),logo2);
			//dc.clippedDrawImage(Point(0,13),Point(i,10+i),Point(255-i,127-i),logo2);
			dc.clippedDrawImage(Point(0,13),Point(0,10),Point(255-i,127),logo2);
			dc.write(Point(0,13+1+logo2.getHeight()),"** Message line");
		}
		sleep(1);
	}
    InputHandler& backend=InputHandler::instance();
    Point prev(0, 0);
    Color color = black;

    {
        DrawingContext dc(display);
        dc.setFont(miscFixed);
        dc.setTextColor(black, white);
        DrawLegend(dc);
    }

    for(;;)
    {
        Event e=backend.getEvent();
        Point p = e.getPoint();
        {
            DrawingContext dc(display);
            switch(e.getEvent())
            {
            case EventType::TouchDown:
                ledOn();
                if (p.y() <= LEGEND_H)
                {
                    if (p.x() >= LEGEND0_B && p.x() <= LEGEND0_E)
                    {
                        dc.clear(white);
                        color = black;
                        DrawLegend(dc);
                    }
                    else if (p.x() >= LEGEND1_B && p.x() <= LEGEND1_E)
                        color =  black;
                    else if (p.x() >= LEGEND2_B && p.x() <= LEGEND2_E)
                        color = black;
                    else if (p.x() >= LEGEND3_B && p.x() <= LEGEND3_E)
                        color = black;
                    dc.setTextColor(black, white);
                    OutCoord(dc, p);
                    prev = Point(-1, -1);
                }
                else
                {
                    prev = p;
                    OutCoord(dc, p);
                    dc.beginPixel();
                    dc.setPixel(p, color);
                }
                break;

            case EventType::TouchUp:
                ledOff();
                prev = Point(-1, -1);
                break;

            case EventType::TouchMove:
                if (prev != Point(-1, -1))
                {
                    OutCoord(dc, p);
                    if ( (p.y() <= LEGEND_H) || (p.y() >= (display.getHeight()- 1)) ||
                         (p.x() >= (display.getWidth() - 1)) )
                        break;
                    dc.line(prev, p, color);
                    dc.line(Point(prev.x()+1, prev.y()), Point(p.x()+1, p.y()), color);
                    dc.line(Point(prev.x(), prev.y()+1), Point(p.x(), p.y()+1), color);
                    prev = p;
                }
                break;

            case EventType::ButtonA:
                dc.clear(black);
                color = white;
                DrawLegend(dc);
                break;

            default:
                break;
            } // switch(e.getEvent())
        }
    } //for(;;)
    return 0;
} // ENTRY()
