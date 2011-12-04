
#include <boost/asio.hpp>

using namespace boost::asio::ip;

void handleClient(tcp::iostream& stream)
{
	stream<<"Lol\n";
}

int main()
{
	boost::asio::io_service io;
	// Accept tcp connections only from 127.0.0.1
	tcp::endpoint ep(address::from_string("127.0.0.1"),8080);
	tcp::acceptor ac(io,ep);
	for(;;)
	{
		tcp::iostream stream;
		boost::system::error_code ec;
		ac.accept(*stream.rdbuf(),ec);
		if(!ec) handleClient(stream);
	}
}
