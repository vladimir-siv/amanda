#include "task.h"

#include "tasks/echo/hello.h"
#include "tasks/echo/reply.h"

Task* Task::resolve(const char* name, HTTPClientRequest request)
{
	Task* task;

	if (strcmp(name, "") == 0) task = nullptr;
	else if (strcmp(name, "Echo/hello") == 0) task = HelloTask::instance();
	else if (strcmp(name, "Echo/reply") == 0) task = ReplyTask::instance();
	// ...
	else task = nullptr;

	if (task != nullptr) task->client = &request;
	return task;
}
