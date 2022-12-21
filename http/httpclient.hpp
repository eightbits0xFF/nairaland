#pragma once

#include <string>


#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>

#include <boost/certify/extensions.hpp>
#include <boost/certify/https_verification.hpp>



#include <iostream>

namespace beast = boost::beast;
namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace http = boost::beast::http;

using tcp = boost::asio::ip::tcp;

std::string getName();

class HttpClient {
public:
	HttpClient(ssl::context& ssl_ctx, asio::io_context& ctx, std::string host);

	int get(std::string target, std::string& out);


private:
	tcp::resolver::results_type resolve(std::string const& hostname);

	tcp::socket connect(std::string const& hostname);

	std::unique_ptr<ssl::stream<tcp::socket>>& HttpClient::connect_ssl(std::string const& hostname);

	http::response<http::string_body> get_stream(std::string uri);

	std::unique_ptr<ssl::stream<tcp::socket>> stream_ptr;
	std::string host_;
	asio::io_context& context;
	ssl::context& ssl_context;
};