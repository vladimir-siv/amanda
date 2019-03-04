#include <suit/assert.h>

#include "../../amanda.os/exceptions.h"
#include "../../amanda.os/structures/list.h"

void _list_int()
{
	int value = 5;		// 5
	int& lref = value;	// 5
	int&& rref = 4;		// 4

	List<int> list;

	list.push_back(3);		// 3
	list.push_back(value);	// 5
	list.push_front(lref);	// 5
	list.push_back(rref);	// 4
	list.push_front(6);		// 6

	// list: { 6, 5, 3, 5, 4 }
	assert::areEqual(list.size(), 5, "[A1] Size is not equal to 5");
	assert::areEqual(list.pop_front(), 6, "[A2] Front pop not equal to 6");
	assert::areEqual(list.peek_front(), 5, "[A3] Front peek not equal to 5");
	assert::areEqual(list.peek_back(), 4, "[A4] Back peek not equal to 5");
	assert::areEqual(list.pop_front(), 5, "[A5] Front pop not equal to 5");
	assert::areEqual(list.pop_front(), 3, "[A6] Front pop not equal to 3");

	// list: { 5, 4 }
	assert::areEqual(list.size(), 2, "[A7] Size is not equal to 2");
	assert::areEqual(list.peek_front(), 5, "[A8] Front peek not equal to 5");
	assert::areEqual(list.peek_back(), 4, "[A9] Back peek not equal to 4");

	list.push_back(int(3));	// 3

	// list: { 5, 4, 3 }
	assert::areEqual(list.size(), 3, "[A10] Size is not equal to 3");
	assert::areEqual(list.pop_front(), 5, "[A11] Front pop not equal to 5");

	list.clear();
	list.push_front(7);		// 7
	list.push_front(4);		// 4
	list.push_back(value);	// 5

	// list: { 4, 7, 5 }
	assert::areEqual(list.size(), 3, "[A12] Size is not equal to 3");
	assert::areEqual(list.peek_back(), 5, "[A13] Back peek not equal to 5");
	assert::areEqual(list.pop_front(), 4, "[A14] Front pop not equal to 4");

	// list: { 7, 5 }
	assert::areEqual(list.size(), 2, "[A15] Size is not equal to 2");
	assert::areEqual(list.pop_front(), 7, "[A16] Front pop not equal to 7");
	assert::areEqual(list.pop_front(), 5, "[A17] Front pop not equal to 5");

	// list: { }
	assert::areEqual(list.size(), 0, "[A18] Size is not equal to 0");

	list.push_back(4);		// 4
	list.push_back(5);		// 5
	list.push_back(6);		// 6

	// list: { 4, 5, 6 }
	assert::areEqual(list.size(), 3, "[A19] Size is not equal to 3");
	assert::areEqual(list.peek_back(), 6, "[A20] Back peek not equal to 6");
	assert::areEqual(list.peek_front(), 4, "[A21] Front peek not equal to 4");
}
