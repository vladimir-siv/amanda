#include <suit/assert.h>
#include <suit/outputcomparer.h>
#include <units/coord.h>

#include <exceptions.h>
#include <structures/queue.h>

#include <string>

void eval_queue()
{
	queue<Coord> que1(7);
	queue<Coord*> que2(5);

	Coord c0(8, 2);					// ctor
	Coord& c0r = c0;				// ...
	Coord&& c1 = Coord(7, 5);		// ctor
	Coord c2 = Coord(4, 6);			// ctor

	que1.enqueue(Coord(3, 2));		// ctor -> move= -> dtor
	que1.enqueue(c0);				// copy=
	que1.enqueue(c0r);				// copy=
	que1.enqueue(c1);				// copy=
	que1.enqueue(c2);				// copy=
	que1[5] = c0r;					// copy=
	que1[6] = Coord(0, 1);			// ctor -> move= -> dtor

	que2.enqueue(&c0);
	que2.enqueue(&c0r);
	que2.enqueue(&c1);
	que2.enqueue(&c2);

	queue<Coord*> que3 = que2;
	assert::areEqual(que3.size(), 4, "[A1] Size not equal to 4");

	assert::areEqual(que3.dequeue()->id, 7, "[A2] Pop is not equal to 7");
	assert::areEqual(que3.dequeue()->id, 7, "[A3] Pop is not equal to 7");
	assert::areEqual(que3.dequeue()->id, 8, "[A4] Pop is not equal to 8");
	assert::areEqual(que3.dequeue()->id, 9, "[A5] Pop is not equal to 9");

	que2.clear();
	que1.clear();
}

void _queue_coord()
{
	OutputComparer<std::string> o;
	Coord::Subscription sub = o.appender();
	Coord::reset_ids();

	eval_queue();

	assert::areEqual(o.size(), 31, "[A6] Output size not equal to 31");

	// local queue creation
	o.next("Coord<0>(0, 0)::ctor", "[A7]");
	o.next("Coord<1>(0, 0)::ctor", "[A8]");
	o.next("Coord<2>(0, 0)::ctor", "[A9]");
	o.next("Coord<3>(0, 0)::ctor", "[A10]");
	o.next("Coord<4>(0, 0)::ctor", "[A11]");
	o.next("Coord<5>(0, 0)::ctor", "[A12]");
	o.next("Coord<6>(0, 0)::ctor", "[A13]");

	// local creation
	o.next("Coord<7>(8, 2)::ctor", "[A14]");
	o.next("Coord<8>(7, 5)::ctor", "[A15]");
	o.next("Coord<9>(4, 6)::ctor", "[A16]");

	// rvalue move
	o.next("Coord<10>(3, 2)::ctor", "[A17]");
	o.next("Coord<0>(3, 2)::move=", "[A18]");
	o.next("Coord<10>(3, 2)::dtor", "[A19]");

	// copy on enqueue
	o.next("Coord<1>(8, 2)::copy=", "[A20]");
	o.next("Coord<2>(8, 2)::copy=", "[A21]");
	o.next("Coord<3>(7, 5)::copy=", "[A22]");
	o.next("Coord<4>(4, 6)::copy=", "[A23]");

	// copy on set
	o.next("Coord<5>(8, 2)::copy=", "[A24]");

	// move on set
	o.next("Coord<11>(0, 1)::ctor", "[A25]");
	o.next("Coord<6>(0, 1)::move=", "[A26]");
	o.next("Coord<11>(0, 1)::dtor", "[A27]");

	// local destruction
	o.next("Coord<9>(4, 6)::dtor", "[A28]");
	o.next("Coord<8>(7, 5)::dtor", "[A29]");
	o.next("Coord<7>(8, 2)::dtor", "[A30]");

	// local queue destruction
	o.next("Coord<6>(0, 1)::dtor", "[A31]");
	o.next("Coord<5>(8, 2)::dtor", "[A32]");
	o.next("Coord<4>(4, 6)::dtor", "[A33]");
	o.next("Coord<3>(7, 5)::dtor", "[A34]");
	o.next("Coord<2>(8, 2)::dtor", "[A35]");
	o.next("Coord<1>(8, 2)::dtor", "[A36]");
	o.next("Coord<0>(3, 2)::dtor", "[A37]");
}
