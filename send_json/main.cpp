#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

boost::asio::io_service ioService;
boost::asio::ip::tcp::resolver resolv{ ioService };
boost::asio::ip::tcp::socket tcpSocket{ ioService };

void connectHandler(const boost::system::error_code &ec) {
	if (!ec) {
		boost::property_tree::ptree jsonTree;
		std::ostringstream jsonBuffer;
		boost::property_tree::write_json(jsonBuffer, jsonTree, false);
		std::string json = jsonBuffer.str();

		boost::asio::streambuf request;
		std::ostream requestStream(&request);
		requestStream << "POST / HTTP/1.1\r\n";
		requestStream << "Host: 127.0.0.1 \r\n";
		requestStream << "User-Agent: C/1.0\r\n";
		requestStream << "Content-Type: application/json; charset=utf-8 \r\n";
		requestStream << "Accept: */*\r\n";
		requestStream << "Content-Length: " << json.length() << "\r\n";
		requestStream << "Connection: close\r\n\r\n";
		requestStream << json;

		boost::asio::write(tcpSocket, request);
	}
}

void resolveHandler(const boost::system::error_code &ec,
	boost::asio::ip::tcp::resolver::iterator it) {
	if (!ec)
		tcpSocket.async_connect(*it, connectHandler);
}

int main(int argc, char** argv) {
	boost::asio::ip::tcp::resolver::query q{ "127.0.0.1", "80" };
	resolv.async_resolve(q, resolveHandler);
	ioService.run();
	return 0;
}