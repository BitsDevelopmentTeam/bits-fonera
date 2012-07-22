
#include <iostream>
#include <sstream>
#include "bitsd.h"
#include "base64.h"
#include "logger.h"

using namespace std;

Bitsd::Bitsd(const string& server, int port,
		  const string& serialPort, int baudrate)
		  : BitsdCore(server, port, serialPort, baudrate) {}

Bitsd::~Bitsd() {}

void Bitsd::onConnect()
{
	Logger::instance().append("Connected");
}

void Bitsd::onTcpMessage(const string& message)
{
	stringstream ss(message);
	string command,arg;
	ss>>command>>arg;
	if(command=="status")
	{
		if(arg=="1") serialWrite("open\n");
		else serialWrite("closed\n");
	} else if(command=="message")
	{
		//TODO
		cout<<"Received a message: "<<base64_decode(arg)<<endl;
	} else if(command=="sound")
	{
		//TODO
		cout<<"Received a sound request"<<endl;
	} else Logger::instance().append(string("unknown data from socket: ")+message);
}

void Bitsd::onSerialMessage(const string& message)
{
	if(message.empty()) return;
	if(message.substr(0,3)=="-->" && message.size()>=4)
	{
		char key=message[3];
		if(key=='A')
		{
			serialWrite("open\n");
			Logger::instance().append("status 1");
			tcpWrite("status 1\n");
		} else if(key=='C')
		{
			serialWrite("closed\n");
			Logger::instance().append("status 0");
			tcpWrite("status 0\n");
		}
	} else Logger::instance().append(string("unknown data from serial: ")+message);
}

void Bitsd::onTimer() {}
