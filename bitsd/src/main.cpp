
#include <sstream>
#include <cassert>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include "bitsd.h"
#include "configfile.h"

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
	string logfile;
	if(config.hasOption("logfile")) logfile=config.getString("logfile");
	Bitsd bitsd(address,port,serial,baudrate,logfile);

	const int retryPeriods[]={5,10,15,30,60,2*60,5*60,10*60,15*60};
	const int retryMax=sizeof(retryPeriods)/sizeof(int);
	int r=0;
	for(;;)
	{
		try {
			bitsd.run();
			assert(false); //Should never happen
		} catch(std::exception& e)
		{
			if(r<retryMax)
			{
				stringstream ss;
				ss<<"Disconnected because of \""<<e.what()<<"\". ";
				ss<<"Retrying connection in "<<retryPeriods[r]<<"s.";
				if(r==retryMax-1)
					ss<<" (further disconnections won't be logged)";
				bitsd.appendToLog(ss.str());
			}
			sleep(r<retryMax ? retryPeriods[r++] : retryPeriods[retryMax-1]);
		}
	}
}
