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
		pairs[i] = memory.alloc()->init(7 - i, 'a' + i);
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
	
	auto fetch = [&](int index) -> SimplePair*
	{
		if (index == 0) return obj1;
		if (index == 1) return obj3;
		return pairs[index - 2];
	};

	message = "[Axx] Pointers are the same [i == j]";
	for (int i = 0; i < 8; ++i)
	{
		SimplePair* pi = fetch(i);

		for (int j = 0; j < 8; ++j)
		{
			if (i != j)
			{
				SimplePair* pj = fetch(j);

				int t = 16 + i * 8 + j;
				message[2] = '0' + (t / 10);
				message[3] = '0' + (t % 10);
				message[29] = '0' + i;
				message[34] = '0' + j;
				assert::areNotEqual(pi, pj, message.c_str());
			}
		}
	}

	SimplePair* p0 = pairs[0];
	SimplePair* p2 = pairs[2];
	SimplePair* p3 = pairs[3];
	SimplePair* p4 = pairs[4];

	memory.dealloc(pairs[2]);
	memory.dealloc(pairs[4]);
	memory.dealloc(pairs[0]);
	memory.dealloc(pairs[3]);
	pairs[2] = memory.alloc();
	pairs[4] = memory.alloc();
	pairs[0] = memory.alloc();
	pairs[3] = memory.alloc();

	assert::areEqual(pairs[2], p3, "[A79] Memory address invalid");
	assert::areEqual(pairs[4], p0, "[A80] Memory address invalid");
	assert::areEqual(pairs[0], p4, "[A81] Memory address invalid");
	assert::areEqual(pairs[3], p2, "[A82] Memory address invalid");
}
