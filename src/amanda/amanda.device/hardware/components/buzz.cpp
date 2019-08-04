#include "buzz.h"

BUZZ* BUZZ::_playing = nullptr;

Command* BUZZ::resolve_cmd(const char* name) const
{
	Command* cmd = nullptr;

	if (strcmp_P(name, PSTR("")) == 0) cmd = nullptr;
	else if (strcmp_P(name, PSTR("play")) == 0) cmd = buzz_commands::Play::instance();
	else if (strcmp_P(name, PSTR("stop")) == 0) cmd = buzz_commands::Stop::instance();
	else cmd = nullptr;

	bind_to(cmd);
	return cmd;
};
