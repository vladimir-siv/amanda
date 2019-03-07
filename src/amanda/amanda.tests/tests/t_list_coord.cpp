#include <suit/assert.h>
#include <suit/outputcomparer.h>
#include <units/coord.h>

#include <exceptions.h>
#include <structures/list.h>

#include <string>

void eval_list()
{
	List<Coord> list1;
	List<Coord*> list2;

	Coord c0(8, 2);					// ctor
	Coord& c0r = c0;				// ...
	Coord&& c1 = Coord(7, 5);		// ctor
	Coord c2 = Coord(4, 6);			// ctor

	list1.push_back(Coord(3, 2));	// ctor -> move -> dtor
	list1.push_front(c0);			// copy
	list1.push_back(c0r);			// copy
	list1.push_front(c1);			// copy
	list1.push_back(c2);			// copy

	list2.push_back(&c0);
	list2.push_front(&c0r);
	list2.push_back(&c1);
	list2.push_front(&c2);

	List<Coord*> list3 = list2;
	assert::areEqual(list3.size(), 4, "[A1] Size not equal to 4");
		
	assert::areEqual(list3.pop_front()->id, 2, "[A2] Front pop is not equal to 3");
	assert::areEqual(list3.pop_front()->id, 0, "[A3] Front pop is not equal to 1");
	assert::areEqual(list3.pop_front()->id, 0, "[A4] Front pop is not equal to 0");
	assert::areEqual(list3.pop_front()->id, 1, "[A5] Front pop is not equal to 2");

	Executable<void, Coord&> exec([](Coord& coord) -> void { });
	list1.traverse(&exec);

	list2.clear();
	list1.clear();
}

void _list_coord()
{
	OutputComparer<std::string> o;
	Coord::Subscription sub = o.appender();
	Coord::reset_ids();
	
	eval_list();

	assert::areEqual(o.size(), 18, "[A6] Output size not equal to 18");
	
	// local creation
	o.next("Coord<0>(8, 2)::ctor", "[A7]");
	o.next("Coord<1>(7, 5)::ctor", "[A8]");
	o.next("Coord<2>(4, 6)::ctor", "[A9]");

	// rvalue move
	o.next("Coord<3>(3, 2)::ctor", "[A10]");
	o.next("Coord<4>(3, 2)::move", "[A11]");
	o.next("Coord<3>(3, 2)::dtor", "[A12]");

	// copy on push
	o.next("Coord<5>(8, 2)::copy", "[A13]");
	o.next("Coord<6>(8, 2)::copy", "[A14]");
	o.next("Coord<7>(7, 5)::copy", "[A15]");
	o.next("Coord<8>(4, 6)::copy", "[A16]");

	// clear
	o.next("Coord<7>(7, 5)::dtor", "[A17]");
	o.next("Coord<5>(8, 2)::dtor", "[A18]");
	o.next("Coord<4>(3, 2)::dtor", "[A19]");
	o.next("Coord<6>(8, 2)::dtor", "[A20]");
	o.next("Coord<8>(4, 6)::dtor", "[A21]");

	// local destruction
	o.next("Coord<2>(4, 6)::dtor", "[A22]");
	o.next("Coord<1>(7, 5)::dtor", "[A23]");
	o.next("Coord<0>(8, 2)::dtor", "[A24]");
}
