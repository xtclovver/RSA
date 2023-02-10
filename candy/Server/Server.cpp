#include <iostream>
#include <random>
#include <chrono>
#include <cstdio>
#include <string>
#include "httplib.h"


bool isPrime(long int prime);
long int calculateE(long int t);
long int greatestCommonDivisor(long int e, long int t);
long int calculateD(long int e, long int t);

using namespace httplib;



int main()
{
	httplib::Client cli("localhost", 8080);
	int e, n, d;
	long long int p, q = 2, t;
	std::cout << "Generating secret and public key..." << std::endl << std::endl;

	// Cоздание открытого и секретного ключей

	// 1. Выбираются два различных случайных простых числа p и q заданного размера

	do {
		std::mt19937 mt;
		std::random_device rd;
		mt.seed(rd() ^ (
			(std::mt19937::result_type)
			std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::system_clock::now().time_since_epoch()
				).count() +
			(std::mt19937::result_type)
			std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				).count()));
		std::uniform_int_distribution<int> dist(50, 200);
		int rand = dist(mt);
		p = mt() % rand;
	} while (isPrime(p) == false);
	std::cout << "p: " << p << std::endl;
	do {
		std::mt19937 mt;
		std::random_device rd;
		mt.seed(rd() ^ (
			(std::mt19937::result_type)
			std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::system_clock::now().time_since_epoch()
				).count() +
			(std::mt19937::result_type)
			std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
				).count()));
		std::uniform_int_distribution<int> dist(50, 200);
		int rand = dist(mt);
		q = mt() % rand;
	} while (isPrime(q) == false);
	std::cout << "q: " << q << std::endl;

	// 2. Вычисляется их произведение n = p ⋅ q, которое называется модулем.
	n = p * q;
	std::cout << "\nResult of computing n = p*q = " << n << std::endl;

	// 3. Вычисляется значение функции Эйлера от числа n: φ(n) = (p−1)⋅(q−1)
	t = (p - 1) * (q - 1);
	std::cout << "Result of computing Euler's totient function:\t t = " << t << std::endl;

	// 4. Выбирается целое число e ( 1 < e < φ(n) ), взаимно простое со значением функции Эйлера (t)
	//	  Число e называется открытой экспонентой
	e = calculateE(t);

	// 5. Вычисляется число d, мультипликативно обратное к числу e по модулю φ(n), то есть число, удовлетворяющее сравнению:
	//    d ⋅ e ≡ 1 (mod φ(n))
	d = calculateD(e, t);

	// 6. Пара {e, n} публикуется в качестве открытого ключа RSA
	std::cout << "\nRSA public key is (n = " << n << ", e = " << e << ")" << std::endl;

	// 7. Пара {d, n} играет роль закрытого ключа RSA и держится в секрете
	std::cout << "RSA private key is (n = " << n << ", d = " << d << ")" << std::endl;

	if (auto res = cli.Post("/n", std::to_string(n), "text/plain")) {
		std::cout << res->status << std::endl;
		std::cout << res->get_header_value("Content-Type") << std::endl;
		std::cout << res->body << std::endl;
	}
	else {
		std::cout << "error code: " << res.error() << std::endl;
	}
	if (auto res = cli.Post("/e", std::to_string(e), "text/plain")) {
		std::cout << res->status << std::endl;
		std::cout << res->get_header_value("Content-Type") << std::endl;
		std::cout << res->body << std::endl;
	}
	else {
		std::cout << "error code: " << res.error() << std::endl;
	}
	auto res = cli.Get("/stop", { {"Accept-Encoding", "gzip, deflate"} });
	std::cout << "Press enter after you open decryptor...";
	std::cin.ignore();
	if (auto res = cli.Post("/n", std::to_string(n), "text/plain")) {
		std::cout << res->status << std::endl;
		std::cout << res->get_header_value("Content-Type") << std::endl;
		std::cout << res->body << std::endl;
	}
	else {
		std::cout << "error code: " << res.error() << std::endl;
	}
	if (auto res = cli.Post("/d", std::to_string(d), "text/plain")) {
		std::cout << res->status << std::endl;
		std::cout << res->get_header_value("Content-Type") << std::endl;
		std::cout << res->body << std::endl;
	}
	else {
		std::cout << "error code: " << res.error() << std::endl;
	}
	res = cli.Get("/stop", { {"Accept-Encoding", "gzip, deflate"} });
}


bool isPrime(long int prime)
{
	long int i, j;

	j = (long int)sqrt((long double)prime);
	if (prime <= 1)
	{
		return false;
	}
	for (i = 2; i <= j; i++)
	{
		if (prime % i == 0)
		{
			return false;
		}
	}

	return true;
}

long int calculateE(long int t)
{
	// Выбирается целое число e ( 1 < e < t ) // взаимно простое со значением функции Эйлера (t)

	long int e;

	for (e = 2; e < t; e++)
	{
		if (greatestCommonDivisor(e, t) == 1)
		{
			return e;
		}
	}

	return -1;
}

long int greatestCommonDivisor(long int e, long int t)
{
	while (e > 0)
	{
		long int myTemp;

		myTemp = e;
		e = t % e;
		t = myTemp;
	}

	return t;
}

long int calculateD(long int e, long int t)
{
	// Вычисляется число d, мультипликативно обратное к числу e по модулю φ(n), то есть число, удовлетворяющее сравнению:
	//    d ⋅ e ≡ 1 (mod φ(n))

	long int d;
	long int k = 1;

	while (1)
	{
		k = k + t;

		if (k % e == 0)
		{
			d = (k / e);
			return d;
		}
	}

}