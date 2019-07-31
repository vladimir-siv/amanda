#include <suit/assert.h>
#include <suit/outputcomparer.h>

#include <networking/xml/xml_parser.h>

#include <string>

class XMLHeader final : public ConstantParser::CStream
{
	public: static XMLHeader& instance() { static XMLHeader header; return header; }
	
	private: const char* _header = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>";
	private: const char* _current = _header;
	
	private: XMLHeader() { }
	public: virtual ~XMLHeader() { }
	
	public: virtual char current() const override { return *_current; }
	public: virtual void next() override { if (!eos()) ++_current; }
	public: virtual bool eos() const override { return current() == 0; }
	public: virtual bool reset() override { _current = _header; return true; }
};

const char* nextxmldoc()
{
	static int i = 0;

	const char* xmls[]
	{
		// xml1
		"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>"
		"<simple>"
			"<test attr=\"val\">"
				"hello from xml"
			"</test>"
		"</simple>",

		// xml2
		"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>"
		"<simple>"
			"<test1 attr=\"val\">"
				"hello from xml test 1"
			"</test1>"
			"<test2 attr1=\"val1\" attr2=\"val2\">"
				"hello from xml test 2"
			"</test2>"
			"<test3>"
				"<inner>test3 inner1</inner>"
				"<inner>test3 inner2</inner>"
				"<inner id=\"3\">test3 inner3</inner>"
			"</test3>"
			"<test4 attr1=\"val1\" attr2=\"val2\" attr3=\"val3\">"
			"</test4>"
			"<test5       attr1=\"val1\"     attr2=\"val2\"          attr3=\"val3\"/>"
			" \r\n\t\t\r\n   <test6/>"
		"</simple>",

		// xml3
		"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>"
		"<trivial/>",

		// xml4
		"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>"
		"<trivial/>"
		"<error/>",

		// xml5
		"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>"
		"<simple>"
			"hello from xml"
			"<test>text and node</test>"
		"</simple>",

		nullptr
	};

	return xmls[i++];
}

const char* nextoutput()
{
	static int i = 0;

	const char* outputs[]
	{
		// xml1
		"<TAG_OPENED>:simple",
		"<ATTR_SPEC_END>",
		"<TAG_OPENED>:test",
		"<ATTR_SPEC>:attr=val",
		"<ATTR_SPEC_END>",
		"<TEXT_VALUE>:hello from xml",
		"<TAG_CLOSED>:test",
		"<TAG_CLOSED>:simple",

		// xml2
		"<TAG_OPENED>:simple",
		"<ATTR_SPEC_END>",

		"<TAG_OPENED>:test1",
		"<ATTR_SPEC>:attr=val",
		"<ATTR_SPEC_END>",
		"<TEXT_VALUE>:hello from xml test 1",
		"<TAG_CLOSED>:test1",

		"<TAG_OPENED>:test2",
		"<ATTR_SPEC>:attr1=val1",
		"<ATTR_SPEC>:attr2=val2",
		"<ATTR_SPEC_END>",
		"<TEXT_VALUE>:hello from xml test 2",
		"<TAG_CLOSED>:test2",

		"<TAG_OPENED>:test3",
		"<ATTR_SPEC_END>",
		"<TAG_OPENED>:inner",
		"<ATTR_SPEC_END>",
		"<TEXT_VALUE>:test3 inner1",
		"<TAG_CLOSED>:inner",
		"<TAG_OPENED>:inner",
		"<ATTR_SPEC_END>",
		"<TEXT_VALUE>:test3 inner2",
		"<TAG_CLOSED>:inner",
		"<TAG_OPENED>:inner",
		"<ATTR_SPEC>:id=3",
		"<ATTR_SPEC_END>",
		"<TEXT_VALUE>:test3 inner3",
		"<TAG_CLOSED>:inner",
		"<TAG_CLOSED>:test3",

		"<TAG_OPENED>:test4",
		"<ATTR_SPEC>:attr1=val1",
		"<ATTR_SPEC>:attr2=val2",
		"<ATTR_SPEC>:attr3=val3",
		"<ATTR_SPEC_END>",
		"<TAG_CLOSED>:test4",

		"<TAG_OPENED>:test5",
		"<ATTR_SPEC>:attr1=val1",
		"<ATTR_SPEC>:attr2=val2",
		"<ATTR_SPEC>:attr3=val3",
		"<ATTR_SPEC_END>",
		"<TAG_CLOSED>",

		"<TAG_OPENED>:test6",
		"<ATTR_SPEC_END>",
		"<TAG_CLOSED>",

		"<TAG_CLOSED>:simple",

		// xml3
		"<TAG_OPENED>:trivial",
		"<ATTR_SPEC_END>",
		"<TAG_CLOSED>",

		// xml4
		"<TAG_OPENED>:trivial",
		"<ATTR_SPEC_END>",
		"<TAG_CLOSED>",
		"<PARSING_FAILED>",

		// xml5
		"<TAG_OPENED>:simple",
		"<ATTR_SPEC_END>",
		"<TEXT_VALUE>:hello from xml",
		"<PARSING_FAILED>",

		nullptr
	};

	return outputs[i++];
}

OutputComparer<std::string>& comparer(const char* str = nullptr)
{
	static OutputComparer<std::string> o;
	if (str != nullptr) o.append(str);
	return o;
}

void _tag_opened(const char* tagname) { comparer((std::string("<TAG_OPENED>:") + tagname).c_str()); }
void _attribute_spec(const char* attrname, const char* attrvalue) { comparer((std::string("<ATTR_SPEC>:") + attrname + "=" + attrvalue).c_str()); }
void _attribute_spec_end() { comparer("<ATTR_SPEC_END>"); }
void _text_value(const char* value) { comparer((std::string("<TEXT_VALUE>:") + value).c_str()); }
void _tag_closed(const char* tagname) { if (tagname != nullptr) comparer((std::string("<TAG_CLOSED>:") + tagname).c_str()); else comparer("<TAG_CLOSED>"); }

void _xml_parser()
{
	OutputComparer<std::string>& o = comparer();

	XmlParser parser(XMLHeader::instance());

	Executable<void, const char*> tag_opened(&_tag_opened);
	Executable<void, const char*, const char*> attribute_spec(&_attribute_spec);
	Executable<void> attribute_spec_end(&_attribute_spec_end);
	Executable<void, const char*> text_value(&_text_value);
	Executable<void, const char*> tag_closed(&_tag_closed);
	
	parser.events.SetTagOpenedEventHandler(&tag_opened);
	parser.events.SetAttributeSpecEventHandler(&attribute_spec);
	parser.events.SetAttributeSpecEventEndHandler(&attribute_spec_end);
	parser.events.SetTextValueEventHandler(&text_value);
	parser.events.SetTagClosedEventHandler(&tag_closed);

	const char* current = nullptr;
	std::string message = "[Axx]";

	while (true)
	{
		current = nextxmldoc();
		if (current == nullptr) break;

		parser.reset();
		while (*current != 0)
		{
			if (!parser.nextchar(*current))
			{
				o.append("<PARSING_FAILED>");
				break;
			}

			++current;
		}
	}

	for (int i = 1; true; ++i)
	{
		current = nextoutput();
		if (current == nullptr) break;
		message[2] = '0' + i / 10;
		message[3] = '0' + i % 10;
		o.next(current, message.c_str());
	}

	assert::areEqual(o.size(), 0, "[AF] Output comparer size not equal to 0");
}
