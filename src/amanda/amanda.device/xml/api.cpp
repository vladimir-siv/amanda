#include "api.h"

namespace xml
{
	TiXmlDocument to_document(String&& str) { return to_document(str); }
	TiXmlDocument to_document(const String& str)
	{
		TiXmlDocument doc;
		doc.Parse(str.c_str(), 0, TIXML_ENCODING_UTF8);
		return doc;
	}
	String to_string(const TiXmlDocument& doc)
	{
		TiXmlPrinter printer;
		printer.SetStreamPrinting();
		doc.Accept(&printer);
		String str = printer.CStr();
		return str;
	}
}
