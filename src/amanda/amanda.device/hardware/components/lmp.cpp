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

Command* BlinkingLMP::resolve_cmd(const char* name) const
{
	Command* cmd = nullptr;

	if (strcmp_P(name, PSTR("")) == 0) cmd = nullptr;
	else if (strcmp_P(name, PSTR("blink")) == 0) cmd = lmp_commands::Blink::instance();
	else if (strcmp_P(name, PSTR("stop")) == 0) cmd = lmp_commands::Stop::instance();
	else cmd = nullptr;

	bind_to(cmd);
	return cmd;
};
