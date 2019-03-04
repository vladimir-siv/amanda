// THIS PIECE OF CODE IS A COPY OF A FILE IN "amanda.os" PROJECT
// THE ONLY PURPOSE OF THIS CODE IS FOR TESTING, AND TESTING "amanda.os" ONLY

#include "../../amanda.os/exceptions.h"

#if MAX_EXCEPTIONS > 1

#include "../../amanda.os/structures/qbuffer.h"
QBuffer<Exception*> Context(MAX_EXCEPTIONS);

namespace Exceptions
{
	void Throw(Exception* ex) { Context.push(ex); }
	Exception* Fetch() { return Context.size() > 0 ? Context.pop() : nullptr; }
	Exception* Check() { return Context.size() > 0 ? Context.peek() : nullptr; }
	unsigned int Count() { return Context.size(); }
}

#else

Exception* Last = nullptr;

namespace Exceptions
{
	void Throw(Exception* ex) { Last = ex; }
	Exception* Fetch() { Exception* ex = Last; Last = nullptr; return ex; }
	Exception* Check() { return Last; }
	unsigned int Count() { return Last != nullptr ? 1 : 0 }
}

#endif

// THE COPY ENDS HERE

#include <suit/assert.h>

#include "../../amanda.os/patterns/singleton.h"
#include "../../amanda.os/exceptions.h"

void _exceptions()
{
	static_assert(MAX_EXCEPTIONS == 3, "The test is only meant for \"MAX_EXCEPTIONS\" to be equal to 3");

	assert::areEqual(Exceptions::Count(), 0, "[A1] Count not equal to 0");
	assert::areEqual(Exceptions::Check(), nullptr, "[A2] Check not equal to nullptr");
	assert::areEqual(Exceptions::Fetch(), nullptr, "[A3] Fetch not equal to nullptr");

	Exceptions::Throw<Exception>();
	Exceptions::Throw<CollectionEmptyException>();
	
	assert::areEqual(Exceptions::Count(), 2, "[A4] Count not equal to 2");
	assert::areEqual(Exceptions::Check(), &S<CollectionEmptyException>::I(), "[A5] Check got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<CollectionEmptyException>::I(), "[A6] Fetch got wrong exception");

	Exceptions::Throw<IndexOutOfBoundsException>();
	Exceptions::Throw<InsufficientStackSizeException>();

	assert::areEqual(Exceptions::Count(), 3, "[A7] Count not equal to 3");
	assert::areEqual(Exceptions::Check(), &S<InsufficientStackSizeException>::I(), "[A8] Check got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<InsufficientStackSizeException>::I(), "[A9] Fetch got wrong exception");
	assert::areEqual(Exceptions::Check(), &S<IndexOutOfBoundsException>::I(), "[A10] Check got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<IndexOutOfBoundsException>::I(), "[A11] Fetch got wrong exception");
	assert::areEqual(Exceptions::Check(), &S<Exception>::I(), "[A12] Check got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<Exception>::I(), "[A13] Fetch got wrong exception");
	assert::areEqual(Exceptions::Count(), 0, "[A14] Count not equal to 0");

	Exceptions::Throw<CollectionEmptyException>();
	Exceptions::Throw<IndexOutOfBoundsException>();
	Exceptions::Throw<CollectionFullException>();
	Exceptions::Throw<Exception>();
	Exceptions::Throw<InsufficientStackSizeException>();

	assert::areEqual(Exceptions::Count(), 3, "[A15] Count not equal to 3");
	assert::areEqual(Exceptions::Fetch(), &S<InsufficientStackSizeException>::I(), "[16] Fetch got wrong exception");
	assert::areEqual(Exceptions::Check(), &S<Exception>::I(), "[A17] Check got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<Exception>::I(), "[A18] Fetch got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<CollectionFullException>::I(), "[A19] Fetch got wrong exception");
	assert::areEqual(Exceptions::Check(), nullptr, "[A20] Check not equal to nullptr");
	assert::areEqual(Exceptions::Count(), 0, "[A21] Count not equal to 0");
}
