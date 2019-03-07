#include <suit/assert.h>
#include <suit/outputcomparer.h>

#include <event_system/eventargs.h>
#include <event_system/eventhandler.h>
#include <event_system/event.h>

class PositionEventArgs : public EventArgs
{
	public: int x, y;
	public: PositionEventArgs(int x, int y) : x(x), y(y) { }
};

Event<void*, EventArgs*> event1;
Event<void*, PositionEventArgs*> event2;

OutputComparer<int> eventOutput;

void handler1(void* sender, EventArgs* e)
{
	assert::areEqual(e, EventArgs::empty(), "[A1] EventArgs not equal to empty");
	eventOutput.append(1);
	++(*((int*)sender));
}

void handler2(void* sender, EventArgs* e)
{
	assert::areEqual(e, EventArgs::empty(), "[A2] EventArgs not equal to empty");
	eventOutput.append(2);
	++(*((int*)sender));
}

void handler3(void* sender, PositionEventArgs* e)
{
	assert::areEqual(sender, EventArgs::empty(), "[A3] Sender not equal to EventArgs::empty()");
	assert::areNotEqual(e, nullptr, "[A4] Event arguments are null");

	eventOutput.append(e->x);
}

void handler4(void* sender, PositionEventArgs* e)
{
	assert::areEqual(sender, EventArgs::empty(), "[A5] Sender not equal to EventArgs::empty()");
	assert::areNotEqual(e, nullptr, "[A6] Event arguments are null");

	eventOutput.append(e->y++);
}

void _events()
{
	event1 += new EventHandler<EventArgs*>(&handler1);
	event1 += new EventHandler<EventArgs*>(&handler2);

	event2 += new EventHandler<PositionEventArgs*>(&handler3);
	event2 += new EventHandler<PositionEventArgs*>(&handler4);

	int sender = 10;
	event1.fire(&sender, EventArgs::empty());

	PositionEventArgs args(9, -3);
	event2.fire(EventArgs::empty(), &args);

	assert::areEqual(sender, 12, "[A7] Sender not equal to 12");
	assert::areEqual(eventOutput.size(), 4, "[A8] Output size not equal to 4");

	eventOutput.next(1, "[A9]");
	eventOutput.next(2, "[A10]");
	eventOutput.next(9, "[A11]");
	eventOutput.next(-3, "[A12]");

	assert::areEqual(args.y, -2, "[A13] PositionEventArgs::y was not changed on args");
}
