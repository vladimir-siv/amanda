#pragma once

#include "../ethernet.h"

class Task
{
	public: static Task* resolve(const char* name, HTTPClientRequest request);
	
	protected: ethernet::HTTPClient* client;
	
	public: virtual ~Task() { }
	public: virtual void configure(const char* name = nullptr) { }
	public: virtual void accept(const char* arg) = 0;
	public: virtual void invoke() = 0;
};
