// nairaland.cpp : Defines the entry point for the application.
//

#include "nairaland.h"
#include "httpclient.hpp"
#include "gumbo.h"

#include <map>

#include <Poco/URI.h>
#include <queue>
#include <stack>
#include <ostream>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <regex>
namespace io = boost::iostreams;

#define NAIRALAND "www.nairaland.com"

std::map<std::string, int> visited;


io::stream_buffer<io::file_sink> buf("log.txt", std::ios::app);

void processText(GumboNode* node) {
	
	if (node->type == GUMBO_NODE_TEXT) {
		
		if (node->parent->v.element.tag == GUMBO_TAG_A) {
			auto hrefvalue = gumbo_get_attribute(&node->parent->v.element.attributes, "href")->value;
			if (std::regex_match(hrefvalue, std::regex("/[0-9]+/([A-Za-z0-9]+(-[A-Za-z0-9]+)+)#[0-9]+"))) {
				std::cout << node->v.text.text << "\n";
			}
			
		}
		std::ostream out(&buf, true);
		out << node->v.text.text << "\n";
	}
	
}

static void search_for_links(GumboNode* node, std::vector<std::string>& urls) {
	if (node->type != GUMBO_NODE_ELEMENT) {
		return processText(node);
	}
	GumboAttribute* href;
	GumboAttribute* cls;
	if (node->v.element.tag == GUMBO_TAG_A &&
		(href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
		//  std::cout << href->value << std::endl;
		urls.push_back(std::string(href->value));
	}
	if (node->v.element.tag == GUMBO_TAG_TABLE) {

	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		search_for_links(static_cast<GumboNode*>(children->data[i]), urls);
	}
}

std::stack<std::string> linkQueue;

void httpProcessor(HttpClient& client, std::string& target) {


	// std::cout << "Targeted " << target << "\n";
	visited[target]++;
	std::string out;
	if (client.get(target, out) < 0) {
		return;
	}
	std::vector<std::string> urls;
	GumboOutput* output = gumbo_parse(out.c_str());
	search_for_links(output->root, urls);
	std::string t;

	while (!linkQueue.empty()) {

		// std::cout << "================= " << linkQueue.size() << "========\n";
		t = linkQueue.top(); linkQueue.pop();

		std::cout << t << "\n";


		for (auto url : urls) {

			Poco::URI link(url);
			// std::cout << link.getHost() << "\n";
			std::string host = link.getHost();
			if (host == NAIRALAND || host == "") {
				// std::cout << url << "\n";
				std::string path = link.getPath();
				if (visited[path] < 1) {
					//std::cout << " Detected " << path << "\n";
					linkQueue.push(path);

				}
			}

			urls.pop_back();

		}

		// httpProcessor(client, t);
		if (output != nullptr) {
			gumbo_destroy_output(&kGumboDefaultOptions, output);
			output = nullptr;
		}

		httpProcessor(client, t);

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
	visited[""]++;
	visited[" "]++;
	std::string target = "/";
	linkQueue.push("/");
	httpProcessor(client, target);



	return 0;
}
