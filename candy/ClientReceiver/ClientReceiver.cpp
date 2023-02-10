#include <iostream>
#include "httplib.h"

using namespace httplib;

long int decrypt(long int i, long int d, long int n);

int main()
{
	std::string msg;
	long long int encryptedText[500];
	memset(encryptedText, 0, sizeof(encryptedText));
	long long int decryptedText[500];
	memset(decryptedText, 0, sizeof(decryptedText));
	Server svr;
	svr.set_read_timeout(15, 0); // 5 seconds
	svr.set_write_timeout(15, 0); // 5 seconds
	svr.set_idle_interval(0, 100000); // 100 milliseconds
	auto n = 0;
	auto d = 0;
	std::string enctext;
	if (!svr.is_valid()) {
		printf("server has an error...\n");
		return -1;
	}
	std::cout << "start Encrypter then press enter";
	std::cin.ignore();
	svr.Post("/encrypted",
		[&](const Request& req, Response& res, const ContentReader& content_reader) {
			std::string body;
	content_reader([&](const char* data, size_t data_length) {
		body.append(data, data_length);
	enctext = body;
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
	std::cout << "start Server then press enter";
	std::cin.ignore();
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
	svr.Post("/d",
		[&](const Request& req, Response& res, const ContentReader& content_reader) {
			std::string body;
	content_reader([&](const char* data, size_t data_length) {
		body.append(data, data_length);
	d = std::stoi(body);
	return true;
		});
		}

	);
	svr.Get("/stop",
		[&](const Request& /*req*/, Response& /*res*/) { svr.stop(); });
	svr.listen("localhost", 8080);
	system("cls");
	std::cout << "d: " << d << "\n";
	std::cout << "n: " << n << "\n";

	std::cout << "\n\nenctext: " << enctext << "\n";
	std::string msgtext = enctext; 
	int msgsize = 1;
	for (auto i = 0; i < msgtext.size(); i++)
	{
		if ((std::stoi(msgtext.substr(0, msgtext.find(",")))) != NULL) {
			msgsize++;
			msgtext.erase(0, msgtext.find(",") + 1);
		}
	}
	for (long int i = 0; i < msgsize; i++)
	{
		encryptedText[i] = std::stoll(enctext.substr(0, enctext.find(",")));
		enctext.erase(0, enctext.find(",") + 1);
	}
	std::cout << "encryptedText: ";
	for (long int i = 0; i < msgsize; i++)
	{
		std::cout << encryptedText[i];
	}
	for (long int i = 0; i < msgsize; i++)
	{
		decryptedText[i] = decrypt(encryptedText[i], d, n);
	}

	std::cout << "\n\nTHE DECRYPTED MESSAGE IS:" << std::endl;

	for (long int i = 0; i < msgsize; i++)
	{
		std::cout << (char)decryptedText[i];
	}
	std::cout << std::endl << std::endl;
	return 0;
}

long int decrypt(long int i, long int d, long int n)
{
	long int current, result;

	current = i;
	result = 1;

	for (long int j = 0; j < d; j++)
	{
		result = result * current;
		result = result % n;
	}

	return result + 97;
}
