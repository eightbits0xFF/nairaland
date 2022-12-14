// nairaland.cpp : Defines the entry point for the application.
//

#include "nairaland.h"
#include "httpclient.hpp"

using namespace std;

int main()
{

    asio::io_context ctx;
    ssl::context ssl_ctx{ ssl::context::tls_client };
    ssl_ctx.set_verify_mode(ssl::context::verify_peer | ssl::context::verify_fail_if_no_peer_cert);
    ssl_ctx.set_default_verify_paths();

    boost::certify::enable_native_https_server_verification(ssl_ctx);

    HttpClient client{ ssl_ctx, ctx, "www.nairaland.com" };

    std::cout << client.get(std::string("/"));

	return 0;
}
