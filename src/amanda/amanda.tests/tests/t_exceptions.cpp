#include <suit/assert.h>

#include <patterns/singleton.h>
#include <exceptions.h>

void _exceptions()
{
	static_assert(MAX_EXCEPTIONS == 3, "The test is only meant for \"MAX_EXCEPTIONS\" to be equal to 3");

	assert::areEqual(Exceptions::Count(), 0, "[A1] Count not equal to 0");
	assert::areEqual(Exceptions::Check(), nullptr, "[A2] Check not equal to nullptr");
	assert::areEqual(Exceptions::Fetch(), nullptr, "[A3] Fetch not equal to nullptr");

	Exceptions::LThrow<Exception>();
	Exceptions::LThrow<CollectionEmptyException>();
	
	assert::areEqual(Exceptions::Count(), 2, "[A4] Count not equal to 2");
	assert::areEqual(Exceptions::Check(), &S<CollectionEmptyException>::I(), "[A5] Check got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<CollectionEmptyException>::I(), "[A6] Fetch got wrong exception");

	Exceptions::LThrow<IndexOutOfBoundsException>();
	Exceptions::LThrow<InsufficientStackSizeException>();

	assert::areEqual(Exceptions::Count(), 3, "[A7] Count not equal to 3");
	assert::areEqual(Exceptions::Check(), &S<InsufficientStackSizeException>::I(), "[A8] Check got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<InsufficientStackSizeException>::I(), "[A9] Fetch got wrong exception");
	assert::areEqual(Exceptions::Check(), &S<IndexOutOfBoundsException>::I(), "[A10] Check got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<IndexOutOfBoundsException>::I(), "[A11] Fetch got wrong exception");
	assert::areEqual(Exceptions::Check(), &S<Exception>::I(), "[A12] Check got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<Exception>::I(), "[A13] Fetch got wrong exception");
	assert::areEqual(Exceptions::Count(), 0, "[A14] Count not equal to 0");

	Exceptions::Clear();
	Exceptions::LThrow<CollectionEmptyException>();
	Exceptions::LThrow<IndexOutOfBoundsException>();
	Exceptions::LThrow<CollectionFullException>();
	Exceptions::LThrow<Exception>();
	Exceptions::LThrow<InsufficientStackSizeException>();

	assert::areEqual(Exceptions::Count(), 3, "[A15] Count not equal to 3");
	assert::areEqual(Exceptions::Fetch(), &S<InsufficientStackSizeException>::I(), "[A16] Fetch got wrong exception");
	assert::areEqual(Exceptions::Check(), &S<Exception>::I(), "[A17] Check got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<Exception>::I(), "[A18] Fetch got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<CollectionFullException>::I(), "[A19] Fetch got wrong exception");
	assert::areEqual(Exceptions::Check(), nullptr, "[A20] Check not equal to nullptr");
	assert::areEqual(Exceptions::Count(), 0, "[A21] Count not equal to 0");

	assert::doesThrow<IndexOutOfBoundsException>(&Exceptions::Throw<IndexOutOfBoundsException>, "[A22] Exception was not thrown");
	assert::doesNotThrow<IndexOutOfBoundsException>(&Exceptions::Throw<InsufficientStackSizeException>, "[A23] Exception was not thrown");
	assert::areEqual(Exceptions::Count(), 2, "[A24] Count not equal to 2");
	assert::areEqual(Exceptions::Fetch(), &S<InsufficientStackSizeException>::I(), "[A25] Fetch got wrong exception");
	assert::areEqual(Exceptions::Fetch(), &S<IndexOutOfBoundsException>::I(), "[A26] Fetch got wrong exception");
	assert::areEqual(Exceptions::Count(), 0, "[A27] Count not equal to 0");

	Exceptions::LThrow<Exception>();
	Exceptions::LThrow<InsufficientStackSizeException>();
	Exceptions::Clear();
	assert::areEqual(Exceptions::Count(), 0, "[A28] Count not equal to 0");
}
