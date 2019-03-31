#include <suit/assert.h>
#include <suit/outputcomparer.h>
#include <units/coord.h>

#include <exceptions.h>
#include <structures/specialized/vlist.h>

#include <string>

void eval_vlist()
{
	vlist<Coord> list1;
	vlist<Coord> list2;

	Coord c0(8, 2);					// ctor
	Coord& c0r = c0;				// ...
	Coord&& c1 = Coord(7, 5);		// ctor
	Coord c2 = Coord(4, 6);			// ctor
	Coord c3 = Coord(3, 2);			// ctor

	list1.push_back(&c3);
	list1.push_front(&c0);
	list1.push_back(&c0r);
	list1.push_front(&c1);
	list1.push_back(&c2);

	list2.push_back(&c0);
	list2.push_front(&c0r);
	list2.push_back(&c1);
	list2.push_front(&c2);

	vlist<Coord> list3 = list2;
	assert::areEqual(list3.size(), 4, "[A1] Size not equal to 4");

	assert::areEqual(list3.pop_front()->id, 2, "[A2] Front pop is not equal to 2");
	assert::areEqual(list3.pop_front()->id, 0, "[A3] Front pop is not equal to 0");
	assert::areEqual(list3.pop_front()->id, 0, "[A4] Front pop is not equal to 0");
	assert::areEqual(list3.pop_front()->id, 1, "[A5] Front pop is not equal to 1");

	Executable<void, Coord*> exec([](Coord* coord) -> void { });
	list1.traverse(&exec);

	list2.clear();
	list1.clear();
}

void _vlist_coord()
{
	OutputComparer<std::string> o;
	Coord::Subscription sub = o.appender();
	Coord::reset_ids();

	eval_vlist();

	assert::areEqual(o.size(), 8, "[A6] Output size not equal to 8");

	// local creation
	o.next("Coord<0>(8, 2)::ctor", "[A7]");
	o.next("Coord<1>(7, 5)::ctor", "[A8]");
	o.next("Coord<2>(4, 6)::ctor", "[A9]");
	o.next("Coord<3>(3, 2)::ctor", "[A10]");

	// local destruction
	o.next("Coord<3>(3, 2)::dtor", "[A11]");
	o.next("Coord<2>(4, 6)::dtor", "[A12]");
	o.next("Coord<1>(7, 5)::dtor", "[A13]");
	o.next("Coord<0>(8, 2)::dtor", "[A14]");
}
