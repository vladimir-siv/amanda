#include "led.h"

Command* LED::resolve_cmd(const char* name) const
{
	Command* cmd = nullptr;

	if (strcmp_P(name, PSTR("")) == 0) cmd = nullptr;
	else if (strcmp_P(name, PSTR("blink")) == 0) cmd = led_commands::Blink::instance();
	else if (strcmp_P(name, PSTR("stop")) == 0) cmd = led_commands::Stop::instance();
	else cmd = nullptr;

	bind_to(cmd);
	return cmd;
};
