#include <iostream>
#include <cstdlib>
#include <string>
#include <list>
#include "def.h"
#include "assert.h"

extern void setup();

std::list<std::pair<std::string, testcase>> tests;
namespace suit { void add(std::string name, testcase test) { tests.push_back(std::make_pair(name, test)); } }

int main(void)
{
	setup();

	for (auto i = tests.begin(); i != tests.end(); ++i)
	{
		try
		{
			(*(i->second))();
			std::cout << "[SUCCESS] Test \"" << i->first << "\"" << std::endl;
		}
		catch (const AssertException& ex) { std::cout << "[FAILED] Test \"" << i->first << "\": " << ex.what() << std::endl; }
		catch (const AssertError& ex) { std::cout << "[ERROR] Test \"" << i->first << "\": " << ex.what() << std::endl; }
		catch (const TestNotImplemented& ex) { std::cout << "[ERROR] Test \"" << i->first << "\": " << ex.what() << std::endl; }
		catch (const std::exception& ex) { std::cout << "[ERROR] Test \"" << i->first << "\": " << ex.what() << std::endl; }
		catch (...) { std::cout << "[FATAL ERROR] Test \"" << i->first << "\"" << std::endl; }
	}

	std::cout << "Tests finished" << std::endl;
	system("pause");
	return 0;
}
