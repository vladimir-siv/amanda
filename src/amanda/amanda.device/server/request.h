#pragma once

#include "ethernet.h"

class Host final
{
	// [WARNING] Shallow copy WILL lead to undefined behaviour. This implementation is temporary anyway.
	// TODO: Parse possible Host names.
	private: const char* _name = nullptr;
	public: Host& operator=(const char* host_name) { _name = host_name; return *this; }
	public: operator bool() { return _name != nullptr; }
};

class RequestHandler : public ethernet::HTTPRequestParser
{
	protected: Host host;
	protected: unsigned long content_length;
	protected: bool expect100continue;
	protected: bool keepAlive;
	
	public: virtual ~RequestHandler() { }
	
	protected: virtual void reset() override
	{
		host = nullptr;
		content_length = 0;
		expect100continue = false;
		keepAlive = false;
	}
	protected: virtual void request_uri(const char* uri) override
	{
		if (strcmp(uri, "POST / HTTP/1.1") != 0) cancel();
	}
	protected: virtual void request_header(const char* hname, const char* hvalue) override
	{
		if (strcmp(hname, "Host") == 0)
		{
			if (host) cancel();
			else host = hvalue;
		}
		else if (strcmp(hname, "Content-Length") == 0)
		{
			if (content_length > 0) cancel();
			else content_length = strtoul(hvalue, nullptr, 0);
		}
		else if (strcmp(hname, "Expect") == 0)
		{
			if (strcmp(hvalue, "100-continue") == 0)
			{
				if (expect100continue) cancel();
				else expect100continue = true;
			}
			else cancel();
		}
		else if (strcmp(hname, "Connection") == 0)
		{
			if (strcmp(hvalue, "Keep-Alive") == 0)
			{
				if (keepAlive) cancel();
				else keepAlive = true;
			}
			else cancel();
		}
		else cancel();
	}
	protected: virtual void request_header_end() override
	{
		if (!host || !content_length || !expect100continue) cancel();
	}
	protected: virtual bool request_body_end() const override
	{
		return content_length == 0;
	}
	protected: virtual bool request_body(char chr) override
	{
		--content_length;
		return true;
	}
};
