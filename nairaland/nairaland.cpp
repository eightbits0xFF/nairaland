// nairaland.cpp : Defines the entry point for the application.
//

#include "nairaland.h"
#include "httpclient.hpp"
#include "gumbo.h"

#include <map>

#include <Poco/URI.h>
#include <queue>
#include <stack>

#define NAIRALAND "www.nairaland.com"

using namespace std;

std::map<std::string, int> visited;

static void search_for_links(GumboNode* node, std::vector<std::string>& urls) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        std::cout << href->value << std::endl;
        urls.push_back(std::string(href->value));
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        search_for_links(static_cast<GumboNode*>(children->data[i]), urls);
    }
}

std::stack<std::string> linkQueue;

void httpProcessor(HttpClient& client, std::string &target) {
    
    
    

    linkQueue.push(target);
    
   
    
    std::cout << "Targeted " << target << "\n";
   
    

    while (!linkQueue.empty()) {

        std::string out;
        std::string t = linkQueue.top(); linkQueue.pop();
       
        std::cout << client.get(t, out);

        std::vector<std::string> urls;
        GumboOutput* output = gumbo_parse(out.c_str());
        search_for_links(output->root, urls);

        visited[t]++;
        for (auto url : urls) {
            
            Poco::URI link(url);
            // std::cout << link.getHost() << "\n";
            std::string host = link.getHost();
            if (host == NAIRALAND || host == "") {
                // std::cout << url << "\n";
                std::string path = link.getPath();
                if (visited[path] < 1) {
                    std::cout << " Detected " << path << "\n";
                    linkQueue.push(path);
                }
            }

           

    }
    
     gumbo_destroy_output(&kGumboDefaultOptions, output);
    
    }
    

}

int main()
{

    asio::io_context ctx;
    ssl::context ssl_ctx{ ssl::context::tls_client };
    ssl_ctx.set_verify_mode(ssl::context::verify_peer | ssl::context::verify_fail_if_no_peer_cert);
    ssl_ctx.set_default_verify_paths();

    boost::certify::enable_native_https_server_verification(ssl_ctx);

    HttpClient client{ ssl_ctx, ctx, "www.nairaland.com" };
    visited["/register"]++;
    visited["/login"]++;
    std::string target = "/";
    httpProcessor(client, target);

	return 0;
}
