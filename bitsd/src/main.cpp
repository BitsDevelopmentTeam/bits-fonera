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

#include <sstream>
#include <cassert>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include "bitsd.h"
#include "configfile.h"
#include "logger.h"

using namespace std;
using namespace boost;

#ifndef DEBUG_MODE
const string configName="/etc/config/bitsd.conf";
#else //DEBUG_MODE
const string configName="../bitsd-debug.conf";
#endif //DEBUG_MODE

int main()
{
	ConfigFile config(configName);
	string address=config.getString("serveraddr");
	int port=lexical_cast<int>(config.getString("serverport"));
	string serial=config.getString("serialport");
	int baudrate=lexical_cast<int>(config.getString("baudrate"));
	if(config.hasOption("logfile"))
		Logger::instance().setLogFile(config.getString("logfile"));
	Logger::instance().append("Bitsd 1.01 starting...");

	const int retryPeriods[]={5,10,15,30,60,2*60,5*60,10*60,15*60};
	const int retryMax=sizeof(retryPeriods)/sizeof(int);
	int r=0;
	for(;;)
	{
		try {
			Bitsd (address,port,serial,baudrate).run();
			assert(false); //Should never reach here
		} catch(std::exception& e)
		{
			if(r<retryMax)
			{
				stringstream ss;
				ss<<"Disconnected because of \""<<e.what()<<"\". ";
				ss<<"Retrying connection in "<<retryPeriods[r]<<"s.";
				if(r==retryMax-1)
					ss<<" (further disconnections won't be logged)";
				Logger::instance().append(ss.str());
			}
			sleep(r<retryMax ? retryPeriods[r++] : retryPeriods[retryMax-1]);
		}
	}
}
