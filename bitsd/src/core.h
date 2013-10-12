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

#ifndef CORE_H
#define CORE_H

#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

/**
 * Base class for handling events, derive from this to specify the behaviour
 */
class BitsdCore
{
public:
	/**
	 * Constructor
	 * \param server IP address or url of server where to connect
	 * \param port TCP port of server where to connect
	 * \param serialPort serial port where to connect, like "/dev/ttyS0"
	 * \param baudrate serial port baudrate
	 */
	BitsdCore(const std::string& server, int port,
			  const std::string& serialPort, int baudrate);

	/**
	 * Start the daemon. This member function throws an exception if
	 * one of the event handler throw an exception.
	 */
	void run();

	virtual ~BitsdCore();

protected:
	/**
	 * Callback called when the socket connection is established, and
	 * after each successful reconnection in case the network goes down
	 */
	virtual void onConnect()=0;

	/**
	 * Callback called when a message is received from the TCP connection
	 * \param message message sent
	 */
	virtual void onTcpMessage(const std::string& message)=0;

	/**
	 * Callback called when a message is received from the serial port
	 * \param message sent message
	 */
	virtual void onSerialMessage(const std::string& message)=0;

	/**
	 * Called periodically, the period is specified by timerPeriod in core.cpp
	 */
	virtual void onTimer()=0;

	/**
	 * Write a message to the TCP connection
	 * \param message message to send
	 */
	void tcpWrite(const std::string& message);

	/**
	 * Write a message to the serial port
	 * \param message message to send
	 */
	void serialWrite(const std::string& message);

private:
	BitsdCore(const BitsdCore&);
	const BitsdCore& operator= (const BitsdCore&);

	/**
	 * Asynchronous callback when something arrives from the socket
	 * \param ec error code
	 */
	void socketRead(const boost::system::error_code& ec);

	/**
	 * Asynchronous callback when something arrives from the serial port
	 * \param ec error code
	 */
	void serialRead(const boost::system::error_code& ec);

	/**
	 * Asynchronous callback when timer expires
	 * \param ec error code
	 */
	void timerExpired(const boost::system::error_code& ec);

	/**
	 * Asynchronous callback when a write operation is completed,
	 * either on the serial port or TCP connection
	 * \param ec error code
	 * \param data pointer to string whith the message that was sent
	 */
	void writeCompleted(const boost::system::error_code& ec,
			boost::shared_ptr<std::string> data);

	const std::string server;
	int port;
	const std::string serialPort;
	int baudrate;
	boost::asio::io_service io;
	boost::asio::ip::tcp::socket socket;
	boost::asio::deadline_timer timer;
	#ifndef DEBUG_MODE
	boost::asio::serial_port serial;
	#else //DEBUG_MODE
	boost::asio::posix::stream_descriptor serial;
	#endif //DEBUG_MODE
	boost::asio::streambuf tcpReadData, serialReadData;
};

#endif //BOTTOM_H
