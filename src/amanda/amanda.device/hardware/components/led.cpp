#include "led.h"

ThreadDelegate BlinkingLED::_delegate = []() -> void
{
	BlinkingLED* THIS = Thread::current_context<BlinkingLED*>();

	while (true)
	{
		while (THIS->_freq == 0) THIS->_sync.wait();
		THIS->write(HIGH);
		Thread::sleep(THIS->_freq / 2);
		THIS->write(LOW);
		Thread::sleep(THIS->_freq / 2);
	}
};
