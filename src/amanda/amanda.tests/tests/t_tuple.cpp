#include <suit/assert.h>
#include <suit/outputcomparer.h>
#include <units/coord.h>

#include <structures/tuple.h>

#include <string>

void eval_tuple()
{
	Coord c1 = Coord(8, 1);

	Tuple<int, std::string, bool, int, Coord, Coord&, Coord> tuple(3, "hello", true, 5, Coord(3, 2), c1, c1);

	assert::areEqual(tuple.e<0>(), 3, "[A1] First element not equal to 3");
	assert::areEqual(tuple.e<1>(), "hello", "[A2] Second element not equal to \"hello\"");
	assert::areEqual(tuple.e<2>(), true, "[A3] Third element not equal to true");
	assert::areEqual(tuple.e<3>(), 5, "[A4] Fourth element not equal to 5");
	assert::isTrue(tuple.e<4>().x == 3 && tuple.e<4>().y == 2, "[A5] Fifth element not equal to (3, 2)");
	assert::areEqual(tuple.e<5>(), c1, "[A6] Sixth element not equal to c1");
	assert::areEqual(&tuple.e<5>(), &c1, "[A7] Sixth element not equal to c1 by reference");
	assert::areEqual(tuple.e<6>(), c1, "[A8] Seventh element not equal to c1");
	assert::areNotEqual(&tuple.e<6>(), &c1, "[A9] Seventh element equal to c1 by reference");

	tuple.e<4>().x++;
	tuple.e<5>().y += 2;
	
	assert::isTrue(tuple.e<4>().x == 4 && tuple.e<4>().y == 2, "[A10] Fifth element not equal to (4, 2)");
	assert::areEqual(tuple.e<5>(), c1, "[A11] Sixth element not equal to c1");
	assert::areNotEqual(tuple.e<6>(), c1, "[A12] Sixth element still equal to c1");
}

void _tuple()
{
	OutputComparer<std::string> o;
	Coord::Subscription sub = o.appender();
	Coord::reset_ids();

	eval_tuple();

	assert::areEqual(o.size(), 8, "[13] Size not equal to 8");

	o.next("Coord<0>(8, 1)::ctor", "[A14]");
	o.next("Coord<1>(3, 2)::ctor", "[A15]");
	o.next("Coord<2>(3, 2)::move", "[A16]");
	o.next("Coord<3>(8, 1)::copy", "[A17]");
	o.next("Coord<1>(3, 2)::dtor", "[A18]");
	o.next("Coord<3>(8, 1)::dtor", "[A19]");
	o.next("Coord<2>(4, 2)::dtor", "[A20]");
	o.next("Coord<0>(8, 3)::dtor", "[A21]");
}
