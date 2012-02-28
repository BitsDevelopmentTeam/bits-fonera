
#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "bitsd.h"
#include "base64.h"
#include "logger.h"

using namespace std;

Bitsd::Bitsd(const string& server, int port,
		  const string& serialPort, int baudrate)
		  : BitsdCore(server, port, serialPort, baudrate),
		    oldStatus(false), firstTime(true), ignore(true), counter(0),
		    alive(true) {}

Bitsd::~Bitsd() {}

void Bitsd::onConnect()
{
	Logger::instance().append("Connected");
	serialWrite("poul lcd1  Bits 2.0 alpha\n");
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
		firstTime=false;
		ignore=true;
		if(arg=="1") serialWrite("poul lcd0Sede aperta     \n");
		else serialWrite("poul lcd0Sede chiusa     \n");
		oldStatus= arg=="1";
	} else if(command=="sound")
	{
		serialWrite("poul sound\n");
	} else Logger::instance().append(string("unknown data from socket: ")+message);
}

void Bitsd::onSerialMessage(const string& message)
{
	try {
		int i=boost::lexical_cast<int>(message);
		//Try to auto-guess if this is a temperature or stao sede, doesn't
		//work if temperature falls to less then 2 Kelvin
		if(i==0)
		{
			if(firstTime) return; //Avoid changing status before the server
			if(oldStatus==false) return;
			oldStatus=false;
			if(ignore) { ignore=false; return; }
			serialWrite("poul lcd0Sede chiusa     \n");
			Logger::instance().append("status 0");
			tcpWrite("status 0\n");
		} else if(i==1) {
			if(firstTime) return; //Avoid changing status before the server
			if(oldStatus==true) return;
			oldStatus=true;
			if(ignore) { ignore=false; return; }
			serialWrite("poul lcd0Sede aperta     \n");
			Logger::instance().append("status 1");
			tcpWrite("status 1\n");
		} else {
			stringstream ss;
			ss<<"temperature 0 ";
			ss<<((static_cast<float>(i)/2.048f)-273.0f)<<endl;
			tcpWrite(ss.str());
		}
	} catch(...)
	{
		Logger::instance().append(string("unknown data from serial: ")+message);
	}
}

void Bitsd::onTimer()
{
	if(alive) serialWrite("poul lcd1.\n"); else serialWrite("poul lcd1 \n");
	alive=!alive;
	//TODO
	#ifndef DEBUG_MODE
	if(++counter>=600) //Every 600 seconds = 10 minutes
	{
		counter=0;
		serialWrite("poul temp\n");
	} else serialWrite("poul key\n");
	#endif //DEBUG_MODE
}
