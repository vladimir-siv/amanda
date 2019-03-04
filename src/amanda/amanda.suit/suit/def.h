#pragma once

#include <string>

using testcase = void (*)();

namespace suit
{
	void add(std::string name, testcase test);
}
