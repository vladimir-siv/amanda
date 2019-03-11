#include <suit/assert.h>

#include <structures/list.h>

#include <string>

void _enumerators()
{
	const int N = 7;
	int data[N] = { 3, 7, 4, 1, 9, 0, 2 };

	list<int> lst;

	std::string message = "[Ax] Data not equal";

	for (int i = 0; i < N; ++i)
	{
		lst.push_back(data[i]);
	}

	auto i = lst.begin();

	assert::areNotEqual(i, lst.end(), "[A0] Enumerator is on end");

	for (int c = 0; i != lst.end(); ++i, ++c)
	{
		message[2] = '0' + c + 1;
		assert::areEqual(*i, data[c], message.c_str());
	}

	assert::areEqual(i, lst.end(), "[A8] Enumerator is not on end");
}
