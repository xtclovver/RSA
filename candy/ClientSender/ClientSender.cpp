#include <iostream>
#include <sstream>
#include <cstdio>
#include <string>
#include "httplib.h"


using namespace httplib;

long int encrypt(long int i, long int e, long int n);

int main()
{
	std::string msg;
	long long int encryptedText[500];
	memset(encryptedText, 0, sizeof(encryptedText));
	Server svr;
	//svr.set_keep_alive_max_count(2); // Default is 5
	//svr.set_keep_alive_timeout(10);  // Default is 5
	svr.set_read_timeout(15, 0); // 5 seconds
	svr.set_write_timeout(15, 0); // 5 seconds
	svr.set_idle_interval(0, 100000); // 100 milliseconds
	auto n = 0;
	auto e = 0;
	if (!svr.is_valid()) {
		printf("server has an error...\n");
		return -1;
	}
	svr.Post("/n",
		[&](const Request& req, Response& res, const ContentReader& content_reader) {
			std::string body;
	content_reader([&](const char* data, size_t data_length) {
		body.append(data, data_length);
	n = std::stoi(body);
	return true;
		});
		}
	);
	svr.Post("/e",
		[&](const Request& req, Response& res, const ContentReader& content_reader) {
			std::string body;
	content_reader([&](const char* data, size_t data_length) {
		body.append(data, data_length);
	e = std::stoi(body);
	return true;
		});
		}
	);
	svr.Get("/stop",
		[&](const Request& /*req*/, Response& /*res*/) { svr.stop(); });

	svr.set_error_handler([](const Request& /*req*/, Response& res) {
		const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
	char buf[BUFSIZ];
	snprintf(buf, sizeof(buf), fmt, res.status);
	res.set_content(buf, "text/html");
		});

	svr.listen("localhost", 8080);
	system("cls");
	std::cout << "e: " << e << "\n";
	std::cout << "n: " << n << "\n";
	std::cout << "\nEnter Message to be encryped:" << std::endl;

	// Во входном потоке остался символ новой строки, поэтому я использую ignore()
//	std::cin.ignore();

	std::getline(std::cin, msg);

	std::cout << "\nThe message is: " << msg << std::endl;
	for (long int i = 0; i < msg.length(); i++)
	{
		encryptedText[i] = encrypt(msg[i], e, n);
	}

	std::cout << "\nTHE ENCRYPTED MESSAGE IS:" << std::endl;

	for (long int i = 0; i < msg.length(); i++)
	{
		std::cout << (char)encryptedText[i];
	}
	std::string enctext = "";
	for (auto i = 0; i < msg.length(); i++)
	{
		int temp = encryptedText[i];
		enctext = enctext + std::to_string(temp) + ",";
	}

	httplib::Client cli("localhost", 8080);

	if (auto res = cli.Post("/encrypted", enctext, "text/plain")) {
		std::cout << res->status << std::endl;
		std::cout << res->get_header_value("Content-Type") << std::endl;
		std::cout << res->body << std::endl;
	}
	else {
		std::cout << "error code: " << res.error() << std::endl;
	}
	auto res = cli.Get("/stop", { {"Accept-Encoding", "gzip, deflate"} });
}

long int encrypt(long int i, long int e, long int n)
{
	long int current, result;

	current = i - 97;
	result = 1;

	for (long int j = 0; j < e; j++)
	{
		result = result * current;
		result = result % n;
	}

	return result;
}

