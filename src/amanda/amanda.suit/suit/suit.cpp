#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <string>
#include <list>
#include "def.h"
#include "assert.h"

extern void setup();

std::list<std::pair<std::string, testcase>> tests;
namespace suit { void add(std::string name, testcase test) { tests.push_back(std::make_pair(name, test)); } }

int main(void)
{
	using std::chrono::high_resolution_clock;
	using std::chrono::duration;

	setup();

	high_resolution_clock::time_point start = high_resolution_clock::now();
	unsigned long long ntests = 0, nsuccess = 0;

	for (auto i = tests.begin(); i != tests.end(); ++ntests, ++i)
	{
		try
		{
			(*(i->second))();
			++nsuccess;
			std::cout << "[SUCCESS] Test \"" << i->first << "\"" << std::endl;
		}
		catch (const AssertException& ex) { std::cout << "[FAILED] Test \"" << i->first << "\": " << ex.what() << std::endl; }
		catch (const AssertError& ex) { std::cout << "[ERROR] Test \"" << i->first << "\": " << ex.what() << std::endl; }
		catch (const TestNotImplemented& ex) { std::cout << "[ERROR] Test \"" << i->first << "\": " << ex.what() << std::endl; }
		catch (const std::exception& ex) { std::cout << "[ERROR] Test \"" << i->first << "\": " << ex.what() << std::endl; }
		catch (...) { std::cout << "[FATAL ERROR] Test \"" << i->first << "\"" << std::endl; }
	}

	high_resolution_clock::time_point end = high_resolution_clock::now();

	duration<double, std::milli> time_span = end - start;
	std::cout << ntests << " tests finished in " << time_span.count() << " milliseconds [passed: " << nsuccess << "/" << ntests << " | failed: " << (ntests - nsuccess) << "/" << ntests << "]." << std::endl;
	return 0;
}
