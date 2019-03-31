#include <suit/assert.h>

#include <structures/list.h>

#include <string>

void _list_enumerator()
{
	list<int> lst;
	std::string message;

	const int N1 = 7;
	int data1[N1] = { 3, 7, 4, 1, 9, 0, 2 };
	message = "[Ax] Data not equal";

	for (int i = 0; i < N1; ++i)
	{
		lst.push_back(data1[i]);
	}

	auto i = lst.begin();

	assert::areNotEqual(i, lst.end(), "[A1] Enumerator is on end");

	for (int c = 0; i != lst.end(); ++i, ++c)
	{
		message[2] = '0' + c + 2;
		assert::areEqual(*i, data1[c], message.c_str());
	}

	assert::areEqual(i, lst.end(), "[A9] Enumerator is not on end");

	i.begin();

	assert::areEqual(i, lst.begin(), "[A10] Enumerator is not on beginning");

	++i; ++i;

	i.insertAfter(-3);
	i.insertAfter(-7);

	++i; ++i; ++i; ++i;

	i.insertAfter(-5);

	++i; ++i;

	i.insertAfter(-1);

	++i; ++i;

	assert::areNotEqual(i, lst.end(), "[A11] Enumerator is on end");

	i.insertAfter(-9);
	i.insertAfter(-2);
	lst.push_back(-4);

	++i; ++i; ++i;

	i.insertAfter(-8);

	i.begin();

	const int N2 = 15;
	int data2[N2] = { 3, 7, 4, -7, -3, 1, 9, -5, 0, -1, 2, -2, -9, -4, -8 };
	message = "[Axx] Data not equal";

	// lst: { 3, 7, 4, -7, -3, 1, 9, -5, 0, -1, 2, -2, -9, -4, -8 }
	assert::areEqual(lst.size(), N2, "[A12] Size not equal to 15");

	for (int c = 0; i != lst.end(); ++i, ++c)
	{
		message[2] = '0' + (c + 3) / 10 + 1;
		message[3] = '0' + (c + 3) % 10;
		assert::areEqual(*i, data2[c], message.c_str());
	}

	assert::areEqual(i, lst.end(), "[A28] Enumerator is not on end");
}