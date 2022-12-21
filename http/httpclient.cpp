

#include "httpclient.hpp"

std::string getName()
{
	return std::string("Pius");
}




tcp::resolver::results_type HttpClient::resolve(std::string const& hostname)
{
    tcp::resolver resolver{ this->context };
    return resolver.resolve(hostname, "https");
}

tcp::socket HttpClient::connect(std::string const& hostname)
{
    tcp::socket socket{ this->context};
    asio::connect(socket, resolve(hostname));
    return socket;
}

std::unique_ptr<ssl::stream<tcp::socket>>& HttpClient::connect_ssl(std::string const& hostname)
{
    stream_ptr = boost::make_unique<ssl::stream<tcp::socket>>(connect(hostname), ssl_context);

    boost::certify::set_server_hostname(*stream_ptr, hostname);
    boost::certify::sni_hostname(*stream_ptr, hostname);


    stream_ptr->handshake(ssl::stream_base::handshake_type::client);
    
    return stream_ptr;
}

http::response<http::string_body> HttpClient::get_stream(std::string uri)
{
    http::request<http::empty_body> request;
    request.method(http::verb::get);
    request.target(uri);
    std::cout << uri << "\n";
    request.keep_alive(false);
    request.set(http::field::host, host_);
    http::write(*stream_ptr, request);

    http::response<http::string_body> response;
    
    beast::flat_buffer buffer;
    http::read(*stream_ptr, buffer, response);

    return response;
}

int HttpClient::get(std::string target, std::string& out)
{
  
    connect_ssl(this->host_);
    std::cout << this->host_;
    auto response = get_stream(target);
    out = std::move(response.body());

    boost::system::error_code ec;
    stream_ptr->shutdown(ec);
    stream_ptr->next_layer().close(ec);

    return response.result_int();
}

HttpClient::HttpClient(ssl::context& ssl_ctx, asio::io_context& ctx, std::string host):
    host_{host},
    context{ctx},
    ssl_context{ ssl_ctx }
{
}



