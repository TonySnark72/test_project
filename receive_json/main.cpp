#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>

boost::asio::io_service ioService;
boost::asio::ip::tcp::endpoint tcpEndpoint{ boost::asio::ip::tcp::v4(), 80 };
boost::asio::ip::tcp::acceptor tcpAcceptor{ ioService, tcpEndpoint };
boost::asio::ip::tcp::socket tcpSocket{ ioService };
std::array<char, 4096> buf;

void acceptHandler(const boost::system::error_code &ec) {
	if (!ec) {
		std::size_t nBytes = tcpSocket.read_some(boost::asio::buffer(buf, 4096));
		
		std::string sMsg(buf.data(), buf.data() + nBytes);
		std::size_t iJsonPos = sMsg.find("\r\n\r\n") + 4;
		sMsg = sMsg.substr(iJsonPos);

		std::stringstream jsonStream(sMsg);
		boost::property_tree::ptree jsonTree;
		boost::property_tree::read_json(jsonStream, jsonTree);

		boost::posix_time::ptime curTime = boost::posix_time::second_clock::local_time();
		jsonTree.put("date", curTime.date());
		jsonTree.put("time", curTime.time_of_day());

		std::ofstream outStream("json.txt");
		boost::property_tree::write_json(outStream, jsonTree);
		outStream.close();
	}
}

int main(int argc, char** argv) {
	tcpAcceptor.listen();
	tcpAcceptor.async_accept(tcpSocket, acceptHandler);
	ioService.run();
	return 0;
}