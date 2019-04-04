#include "lmp.h"

ThreadDelegate BlinkingLMP::_delegate = []() -> void
{
	BlinkingLMP* THIS = Thread::current_context<BlinkingLMP*>();

	while (true)
	{
		while (THIS->_freq == 0) THIS->_sync.wait();
		THIS->write(HIGH);
		Thread::sleep(THIS->_freq / 2);
		THIS->write(LOW);
		Thread::sleep(THIS->_freq / 2);
	}
};
