#include <suit/assert.h>
#include <suit/outputcomparer.h>
#include <units/coord.h>

#include <types/type_traits.h>
#include <event_system/delegate.h>
#include <string>

template <typename T> void processor(T arg) { }
template <typename T> void forwarder(T&& arg)
{
	processor(arg);
	processor(type_traits::forward<T>(arg));
}

void eval_type_traits()
{
	Coord c0(3, 4);
	Coord c1 = Coord(6, 3);
	Coord c2 = c1;
	Coord c3 = type_traits::move(c1);

	forwarder(c2);
	forwarder(Coord(8, 5));

	c1 = c0;
	c0 = type_traits::move(c2);
}

void _type_traits()
{
	OutputComparer<std::string> o;
	Coord::Subscription sub = o.appender();
	Coord::reset_ids();

	eval_type_traits();
	
	// size should be 18
	assert::areEqual(o.size(), 20, "[A1] Output size not equal to 20");

	// local creation
	o.next("Coord<0>(3, 4)::ctor", "[A2]");
	o.next("Coord<1>(6, 3)::ctor", "[A3]");
	o.next("Coord<2>(6, 3)::copy", "[A4]");
	o.next("Coord<3>(6, 3)::move", "[A5]");

	// forwarder(c2) -> processor(arg)
	o.next("Coord<4>(6, 3)::copy", "[A6]");
	o.next("Coord<4>(6, 3)::dtor", "[A7]");

	// forwarder(c2) -> processor(type_traits::forward(arg))
	o.next("Coord<5>(6, 3)::copy", "[A8]");
	o.next("Coord<5>(6, 3)::dtor", "[A9]");

	// Coord(8, 5)
	o.next("Coord<6>(8, 5)::ctor", "[A10]");

	// forwarder(Coord(8, 5)) -> processor(arg)
	o.next("Coord<7>(8, 5)::copy", "[A11]");
	o.next("Coord<7>(8, 5)::dtor", "[A12]");

	// forwarder(Coord(8, 5)) -> processor(type_traits::forward(arg))
	o.next("Coord<8>(8, 5)::move", "[A13]");
	o.next("Coord<8>(8, 5)::dtor", "[A14]");

	// ~Coord(8, 5)
	o.next("Coord<6>(8, 5)::dtor", "[A15]");

	//c1 = c2;
	o.next("Coord<1>(3, 4)::copy=", "[A16]");
	//c0 = type_traits::move(c2);
	o.next("Coord<0>(6, 3)::move=", "[A17]");
	
	// local destruction
	o.next("Coord<3>(6, 3)::dtor", "[A18]");
	o.next("Coord<2>(6, 3)::dtor", "[A19]");
	o.next("Coord<1>(3, 4)::dtor", "[A20]");
	o.next("Coord<0>(6, 3)::dtor", "[A21]");
}
