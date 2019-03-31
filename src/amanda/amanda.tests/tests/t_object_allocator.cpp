#include <suit/assert.h>

#include <exceptions.h>
#include <memory_management/object_allocator.h>

#include <string>

void _object_allocator()
{
	struct SimplePair
	{
		int i;
		char c;

		SimplePair* init(int i, char c) { this->i = i; this->c = c; return this; }
	};

	ObjectAllocator<SimplePair, 8> memory;
	std::string message;

	SimplePair* obj1 = memory.alloc()->init(10, 'k');
	SimplePair* obj2 = memory.alloc()->init(11, 'p');

	assert::areNotEqual(obj1, nullptr, "[A1] Object 1 is null");
	assert::areNotEqual(obj2, nullptr, "[A2] Object 2 is null");
	
	SimplePair* pairs[6];

	for (int i = 0; i < 6; ++i)
	{
		pairs[i] = memory.alloc()->init(7, 'a');
	}

	message = "[Ax] Element is null";
	for (int i = 0; i < 6; ++i)
	{
		message[2] = '3' + i;
		assert::areNotEqual(pairs[i], nullptr, message.c_str());
	}

	SimplePair* obj3 = memory.alloc();
	SimplePair* obj4 = memory.alloc();

	assert::areEqual(obj3, nullptr, "[A9] Object 3 is not null");
	assert::areEqual(obj4, nullptr, "[A10] Object 4 is not null");

	assert::areEqual(obj1->i, 10, "[A11] Object 1.i is not equal to 10");
	assert::areEqual(obj1->c, 'k', "[A12] Object 1.c is not equal to 'k'");
	assert::areEqual(obj2->i, 11, "[A13] Object 2.i is not equal to 11");
	assert::areEqual(obj2->c, 'p', "[A14] Object 2.c is not equal to 'p'");

	memory.dealloc(obj2);
	obj2 = nullptr;

	obj3 = memory.alloc();

	assert::areNotEqual(obj3, nullptr, "[A15] Object 3 is null");
	assert::areEqual(obj3->i, 11, "[A16] Object 3.i is not equal to 11");
	assert::areEqual(obj3->c, 'p', "[A17] Object 3.c is not equal to 'p'");
}
