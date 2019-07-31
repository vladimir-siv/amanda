#include "process.h"

#include "processes/event/create.h"

Process* Process::resolve(const char* name, HTTPClientRequest request)
{
	Process* proc;

	if (strcmp_P(name, PSTR("")) == 0) proc = nullptr;
	else if (strcmp_P(name, PSTR("Event/create")) == 0) proc = EventCreateProcess::instance();
	// ...
	else proc = nullptr;

	if (proc != nullptr) proc->client = &request;
	return proc;
}
