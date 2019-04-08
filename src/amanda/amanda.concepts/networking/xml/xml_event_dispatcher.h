#pragma once

class XmlEventDispatcher final
{
	friend class XmlParser;

	public: using TagOpenedEventHandler = void (*)(const char* tagvalue);
	public: using AttributeSpecEventHandler = void (*)(const char* attrname, const char* attrvalue);
	public: using AttributeSpecEndEventHandler = void (*)();
	public: using TextValueEventHandler = void (*)(const char* text);
	public: using TagClosedEventHandler = void (*)(const char* tagvalue);
	
	private: TagOpenedEventHandler _tagopened = nullptr;
	private: AttributeSpecEventHandler _attrspec = nullptr;
	private: AttributeSpecEndEventHandler _attrspecend = nullptr;
	private: TextValueEventHandler _textvalue = nullptr;
	private: TagClosedEventHandler _tagclosed = nullptr;
	
	public: void SetTagOpenedEventHandler(TagOpenedEventHandler tagopened) { _tagopened = tagopened; }
	public: void SetAttributeSpecEventHandler(AttributeSpecEventHandler attrspec) { _attrspec = attrspec; }
	public: void SetAttributeSpecEventEndHandler(AttributeSpecEndEventHandler attrspecend) { _attrspecend = attrspecend; }
	public: void SetTextValueEventHandler(TextValueEventHandler textvalue) { _textvalue = textvalue; }
	public: void SetTagClosedEventHandler(TagClosedEventHandler tagclosed) { _tagclosed = tagclosed; }
	
	private: void DispatchTagOpenedEvent(const char* tagvalue) const { if (_tagopened) (*_tagopened)(tagvalue); }
	private: void DispatchAttributeSpecEvent(const char* attrname, const char* attrvalue) const { if (_attrspec) (*_attrspec)(attrname, attrvalue); }
	private: void DispatchAttributeSpecEndEvent() const { if (_attrspecend) (*_attrspecend)(); }
	private: void DispatchTextValueEvent(const char* text) const { if (_textvalue) (*_textvalue)(text); }
	private: void DispatchTagClosedEvent(const char* tagvalue) const { if (_tagclosed) (*_tagclosed)(tagvalue); }
};
