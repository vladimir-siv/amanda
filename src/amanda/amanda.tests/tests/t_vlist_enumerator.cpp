#include <suit/assert.h>

#include <structures/specialized/vlist.h>

#include <string>

void _vlist_enumerator()
{
	vlist<int> lst;
	std::string message;

	const int N1 = 7;
	int data1[N1] = { 3, 7, 4, 1, 9, 0, 2 };
	message = "[Ax] Data not equal";

	for (int i = 0; i < N1; ++i)
	{
		lst.push_back((int*)data1[i]);
	}

	auto i = lst.begin();

	assert::areNotEqual(i, lst.end(), "[A1] Enumerator is on end");

	for (int c = 0; i != lst.end(); ++i, ++c)
	{
		message[2] = '0' + c + 2;
		assert::areEqual(*i, (int*)data1[c], message.c_str());
	}

	assert::areEqual(i, lst.end(), "[A9] Enumerator is not on end");

	i.begin();

	assert::areEqual(i, lst.begin(), "[A10] Enumerator is not on beginning");

	++i; ++i;

	i.insertAfter((int*)-3);
	i.insertAfter((int*)-7);

	++i; ++i; ++i; ++i;

	i.insertAfter((int*)-5);

	++i; ++i;

	i.insertAfter((int*)-1);

	++i; ++i;

	assert::areNotEqual(i, lst.end(), "[A11] Enumerator is on end");

	i.insertAfter((int*)-9);
	i.insertAfter((int*)-2);
	lst.push_back((int*)-4);

	++i; ++i; ++i;

	i.insertAfter((int*)-8);

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
		assert::areEqual(*i, (int*)data2[c], message.c_str());
	}

	assert::areEqual(i, lst.end(), "[A28] Enumerator is not on end");

	i.begin();

	++i; ++i; ++i;

	i.removeNext();

	++i;

	i.removeNext();

	++i; ++i;

	i.removeNext();

	++i; ++i; ++i; ++i;

	i.removeNext();

	i.begin();

	const int N3 = 11;
	int data3[N3] = { 3, 7, 4, -7, 1, -5, 0, 2, -2, -9, -4 };
	message = "[Axx] Data not equal";

	// lst: { 3, 7, 4, -7, 1, -5, 0, 2, -2, -9, -4 }
	assert::areEqual(lst.size(), N3, "[A29] Size not equal to 11");

	assert::areEqual(lst.peek_back(), (int*)-4, "[A30] Last element not equal to -4");

	for (int c = 0; i != lst.end(); ++i, ++c)
	{
		message[2] = '0' + (c + 1) / 10 + 3;
		message[3] = '0' + (c + 1) % 10;
		assert::areEqual(*i, (int*)data3[c], message.c_str());
	}

	assert::areEqual(i, lst.end(), "[A42] Enumerator is not on end");
}
