#include "task.h"

#include "tasks/echo/hello.h"
#include "tasks/echo/reply.h"
#include "tasks/io/basic.h"
#include "tasks/command/execute.h"

Task* Task::resolve(const char* name, HTTPClientRequest request)
{
	Task* task;

	if (strcmp_P(name, PSTR("")) == 0) task = nullptr;
	else if (strcmp_P(name, PSTR("Echo/hello")) == 0) task = HelloTask::instance();
	else if (strcmp_P(name, PSTR("Echo/reply")) == 0) task = ReplyTask::instance();
	else if (strcmp_P(name, PSTR("IO/basic")) == 0) task = BasicIOTask::instance();
	else if (strcmp_P(name, PSTR("Cmd/exec")) == 0) task = CommandExecuteTask::instance();
	// ...
	else task = nullptr;

	if (task != nullptr) task->client = &request;
	return task;
}
