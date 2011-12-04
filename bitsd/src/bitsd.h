
#ifndef BITSD_H
#define BITSD_H

#include "core.h"

class Bitsd : public BitsdCore
{
public:
	/**
	 * Constructor
	 * \param server IP address or url of server where to connect
	 * \param port TCP port of server where to connect
	 * \param serialPort serial port where to connect, like "/dev/ttyS0"
	 * \param baudrate serial port baudrate
	 * \param logfile file for logging or empty string if logging disabled
	 */
	Bitsd(const std::string& server, int port,
		  const std::string& serialPort, int baudrate,
		  const std::string& logfile);

	/**
	 *\param message message that will be appended to the log file
	 */
	void appendToLog(const std::string& message);

	virtual ~Bitsd();
private:
	virtual void onConnect();

	virtual void onTcpMessage(const std::string& message);

	virtual void onSerialMessage(const std::string& message);

	virtual void onTimer();

	std::string log;
	bool oldStatus;
	bool firstTime;
	int counter;
};

#endif //BITSD_H
