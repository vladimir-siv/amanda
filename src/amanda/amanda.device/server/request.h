#pragma once

/**
 *     Request formats:
 * 
 * <action task="{name}"><arg>{value}</arg>...</action>
 * <action process="{name}">{inner-xml}</action>
 * 
 *     Examples of [client request] -> [server response]:
 * 
 * <action task="Echo/hello"></action> -> <message>Hello World.</message>
 * <action task="Echo/reply"><arg>{message}</arg></action> -> <message>{message}</message>
 * 
 * <action process="Event/create">{event-xml}</action> -> <action>[success|failure]</action>
 */

#include "../xml/api.h"
#include "ethernet.h"
#include "actions/task.h"
#include "actions/process.h"

class Host final
{
	// [WARNING] Shallow copy WILL lead to undefined behaviour. This implementation is temporary anyway.
	// TODO: Parse possible Host names.
	private: const char* _name = nullptr;
	public: Host& operator=(const char* host_name) { _name = host_name; return *this; }
	public: operator bool() { return _name != nullptr; }
};

class RequestBodyParser final : public xml::SAXParser
{
	private: HTTPClientRequest request;
	
	private: unsigned int level;
	private: unsigned int l1;
	private: Task* task;
	private: Process* proc;
	
	public: RequestBodyParser(HTTPClientRequest request) : request(request) { }
	
	public: bool parse(unsigned long content_length = 0)
	{
		request.inquire_request();
		return xml::SAXParser::parse(request, content_length);
	}
	
	protected: virtual void reset() override;
	protected: virtual void oncancel() override;
	protected: virtual void tag_opened(const char* tagname) override;
	protected: virtual void attribute_spec(const char* attrname, const char* attrvalue) override;
	protected: virtual void attribute_spec_end() override;
	protected: virtual void text_value(const char* value) override;
	protected: virtual void tag_closed(const char* tagname) override;
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
	protected: virtual bool request_body(HTTPClientRequest request) override
	{
		if (content_length > 0)
		{
			RequestBodyParser body_parser(request);
			return body_parser.parse(content_length);
		}

		return true;
	}
};
