
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <boost/lexical_cast.hpp>
#include "bitsd.h"
#include "base64.h"

using namespace std;

Bitsd::Bitsd(const string& server, int port,
		  const string& serialPort, int baudrate, const string& logfile)
		  : BitsdCore(server, port, serialPort, baudrate), log(logfile),
		    oldStatus(false), firstTime(true), counter(0) {}

void Bitsd::appendToLog(const string& message)
{
	if(log.empty()) return;
	ofstream logFile(log.c_str(),ios::app);
	time_t t=time(NULL);
	tm tt;
	localtime_r(&t,&tt);
	char timestring[1024];
	strftime(timestring,1024,"%c",&tt);
	logFile<<timestring<<": "<<message<<endl;
}

Bitsd::~Bitsd() {}

void Bitsd::onConnect()
{
	appendToLog("Connected");
	serialWrite("poul lcd1Bits 2.0 alpha  \n");
}

void Bitsd::onTcpMessage(const string& message)
{
	stringstream ss(message);
	string command,arg;
	ss>>command>>arg;
	if(command=="message")
	{
		//TODO
		cout<<"Received a message: "<<base64_decode(arg)<<endl;
	} else if(command=="status")
	{
		if(arg=="1") serialWrite("poul lcd0Sede aperta     \n");
		else serialWrite("poul lcd0Sede chiusa     \n");
		if(firstTime)
		{
			firstTime=false;
			oldStatus= arg=="1";
		}
	} else if(command=="sound")
	{
		serialWrite("poul sound\n");
	} else appendToLog(string("unknown data from socket: ")+message);
}

void Bitsd::onSerialMessage(const string& message)
{
	try {
		int i=boost::lexical_cast<int>(message);
		//Try to auto-guess if this is a temperature or stao sede, doesn't
		//work if temperature falls to less then 2 Kelvin
		if(i==0)
		{
			if(oldStatus==false) return;
			oldStatus=false;
			serialWrite("poul lcd0Sede chiusa     \n");
			tcpWrite("status 0\n");
		} else if(i==1) {
			if(oldStatus==true) return;
			oldStatus=true;
			serialWrite("poul lcd0Sede aperta     \n");
			tcpWrite("status 1\n");
		} else {
			stringstream ss;
			ss<<"temperature 0 ";
			ss<<((static_cast<float>(i)/2.048f)-273.0f)<<endl;
			tcpWrite(ss.str());
		}
	} catch(...)
	{
		appendToLog(string("unknown data from serial: ")+message);
	}
}

void Bitsd::onTimer()
{
	//TODO
	#ifndef DEBUG_MODE
	if(++counter>=60) //Every 600 seconds = 10 minutes
	{
		counter=0;
		serialWrite("poul temp\n");
	} else serialWrite("poul key\n");
	#endif //DEBUG_MODE
}
