#pragma once

#include "../ethernet.h"
#include "../../xml/api.h"

class Process
{
	public: static Process* resolve(const char* name, HTTPClientRequest request);
	
	protected: ethernet::HTTPClient* client;
	
	public: virtual ~Process() { }
	public: virtual xml::SAXParser& parser() const = 0;
	public: virtual void invoke(bool success = true, const char* name = nullptr) = 0;
};
