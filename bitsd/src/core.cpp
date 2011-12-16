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

#include <iostream>
#include <stdexcept>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "core.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::asio::ip;

/**
 * Period at which the onTimer member function is called.
 */
static const time_duration timerPeriod=seconds(10);

BitsdCore::BitsdCore(const string& server, int port,
		const string& serialPort, int baudrate)
		: server(server), port(port),
		serialPort(serialPort), baudrate(baudrate), socket(io), timer(io),
		#ifndef DEBUG_MODE
		serial(io)
		#else //DEBUG_MODE
		serial(io, ::dup(STDIN_FILENO)) //If debugging read from stdin
		#endif //DEBUG_MODE
{
	if(port<=0 || port>65535) throw range_error("Invalid TCP port");
	if(baudrate<=0 || baudrate>115200) throw range_error("Invalid baudrate");
}

void BitsdCore::run()
{
	io.reset();
	socket.close();
	#ifndef DEBUG_MODE
	serial.close();
	try {
		serial.open(serialPort);
		serial.set_option(asio::serial_port_base::baud_rate(baudrate));
	} catch(...) {
		throw runtime_error("Serial port open failure");
	}
	#endif //DEBUG_MODE

	tcp::resolver resolver(io);
	tcp::resolver::query query(server,lexical_cast<string>(port));
	tcp::resolver::iterator endpointIterator=resolver.resolve(query);
	tcp::resolver::iterator end;
	system::error_code error=asio::error::host_not_found;
	while (error && endpointIterator!=end)
	{
		socket.close();
		socket.connect(*endpointIterator++,error);
	}
	if(error) throw runtime_error(error.message());

	async_read_until(socket,tcpReadData,'\n',
			boost::bind(&BitsdCore::socketRead,this,
			asio::placeholders::error));
	async_read_until(serial,serialReadData,'\n',
			boost::bind(&BitsdCore::serialRead,this,
			asio::placeholders::error));
	timer.expires_from_now(timerPeriod);
	timer.async_wait(boost::bind(&BitsdCore::timerExpired,this,
			asio::placeholders::error));
	io.post(boost::bind(&BitsdCore::onConnect,this));

	io.run();
	assert(false); //Should never reach here
}

BitsdCore::~BitsdCore() {}

void BitsdCore::tcpWrite(const string& message)
{
	shared_ptr<string> data(new string(message));
	async_write(socket,asio::buffer(*data),
			boost::bind(&BitsdCore::writeCompleted,this,
			asio::placeholders::error,data));
}

void BitsdCore::serialWrite(const string& message)
{
	#ifndef DEBUG_MODE
	shared_ptr<string> data(new string(message));
	async_write(serial,asio::buffer(*data),
			boost::bind(&BitsdCore::writeCompleted,this,
			asio::placeholders::error,data));
	#else //DEBUG_MODE
	cout<<message; //If debugging write to stdout
	#endif //DEBUG_MODE
}

void BitsdCore::socketRead(const system::error_code& ec)
{
	if(ec.value()==125) return; //Operation canceled
	if(ec) throw runtime_error(ec.message());
	istream is(&tcpReadData);
	string line;
	getline(is,line);
	if(!line.empty() && line[line.length()-1]=='\r')
		line=line.substr(0,line.length()-1);

	onTcpMessage(line);

	async_read_until(socket,tcpReadData,'\n',
			boost::bind(&BitsdCore::socketRead,this,
			asio::placeholders::error));
}

void BitsdCore::serialRead(const system::error_code& ec)
{
	if(ec.value()==125) return; //Operation canceled
	if(ec) throw runtime_error(ec.message());
	istream is(&serialReadData);
	string line;
	getline(is,line);
	if(!line.empty() && line[line.length()-1]=='\r')
		line=line.substr(0,line.length()-1);

	onSerialMessage(line);

	async_read_until(serial,serialReadData,'\n',
			boost::bind(&BitsdCore::serialRead,this,
			asio::placeholders::error));
}

void BitsdCore::timerExpired(const system::error_code& ec)
{
	if(ec.value()==125) return; //Operation canceled
	if(ec) throw runtime_error(ec.message());

	onTimer();

	timer.expires_from_now(timerPeriod);
	timer.async_wait(boost::bind(&BitsdCore::timerExpired,this,
			asio::placeholders::error));
}

void BitsdCore::writeCompleted(const system::error_code& ec,
		shared_ptr<string> data)
{
	if(ec.value()==125) return; //Operation canceled
	if(ec) throw runtime_error(ec.message());
	//Note: while the shared_ptr<string> isn't used, it's still important as it
	//guarantees the string isn't deallocated while data write is in progress
}
