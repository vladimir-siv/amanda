#pragma once

#include "../../types/def.h"
#include "xml_event_dispatcher.h"
#include "constant_parser.h"

class XmlParser final
{
	private: enum State { DECLARATION_DETECT, START, TAG_OPEN_CLOSE, TAG_OPENING, AWAIT_SPACE, ATTRIBUTE_DETECT, ATTRIBUTE_VALUE_START, ATTRIBUTE_VALUE_DETECT, ATTRIBUTE_END, IMMEDIATE_CLOSE, TAG_OPENED, TEXT_DETECT, TAG_CLOSE, TAG_CLOSING };
	
	private: ConstantParser _header;
	private: State _state = DECLARATION_DETECT;
	private: char _name[65] = { };
	private: byte _ncurrent = 0;
	private: char _value[65] = { };
	private: byte _vcurrent = 0;
	private: unsigned int _level = 0;
	private: bool _hasRoot = false;
	private: State _afterSpace = ATTRIBUTE_DETECT;
	
	public: XmlEventDispatcher events;
	
	public: explicit XmlParser(ConstantParser::CStream& header) : _header(header) { }
	
	public: void reset()
	{
		_header.reset(false);
		_state = DECLARATION_DETECT;
		_name[64] = 0;
		_value[64] = 0;
		_level = 0;
		_hasRoot = false;
		_afterSpace = ATTRIBUTE_DETECT;
	}
	
	public: bool nextchar(char c)
	{
		switch (_state)
		{
			case DECLARATION_DETECT:
			{
				ConstantParser::Result res = _header.next(c);
				if (res == ConstantParser::ERROR) return false;
				if (res == ConstantParser::DONE) _state = START;
			} break;
			case START:
			{
				if (is_whitespace(c)) return true;
				if (c != '<') return false;

				if (_level == 0)
				{
					if (_hasRoot) return false;
					_ncurrent = 0;
					_state = TAG_OPENING;
					_hasRoot = true;
				}
				else _state = TAG_OPEN_CLOSE;
			} break;
			case TAG_OPEN_CLOSE:
			{
				if (c != '/')
				{
					_ncurrent = 0;
					_state = TAG_OPENING;
					return nextchar(c);
				}
				else
				{
					_state = TAG_CLOSE;
					return nextchar(c);
				}
			} break;
			case TAG_OPENING:
			{
				if (is_alphanumeric(c))
				{
					if (_ncurrent >= 64) return false;
					_name[_ncurrent++] = c;
				}
				else if (c == ' ' || c == '/' || c == '>')
				{
					if (_ncurrent == 0) return false;

					_name[_ncurrent] = 0;
					_ncurrent = 0;

					events.DispatchTagOpenedEvent(_name);

					if (c == ' ')
					{
						_state = AWAIT_SPACE;
						_afterSpace = ATTRIBUTE_DETECT;
					}
					else if (c == '/')
					{
						events.DispatchAttributeSpecEndEvent();
						_state = IMMEDIATE_CLOSE;
					}
					else if (c == '>')
					{
						events.DispatchAttributeSpecEndEvent();
						_state = TAG_OPENED;
					}
					else return false; // should be possible
				}
				else return false;
			} break;
			case AWAIT_SPACE:
			{
				if (c != ' ')
				{
					_state = _afterSpace;
					return nextchar(c);
				}
			} break;
			case ATTRIBUTE_DETECT:
			{
				if (is_alphanumeric(c))
				{
					if (_ncurrent >= 64) return false;
					_name[_ncurrent++] = c;
				}
				else if (c == '=')
				{
					_name[_ncurrent] = 0;
					_ncurrent = 0;
					_state = ATTRIBUTE_VALUE_START;
				}
				else return false;
			} break;
			case ATTRIBUTE_VALUE_START:
			{
				if (c != '"') return false;
				_vcurrent = 0;
				_state = ATTRIBUTE_VALUE_DETECT;
			} break;
			case ATTRIBUTE_VALUE_DETECT:
			{
				if (c != '"') // &quote; inside " "
				{
					if (_vcurrent >= 64) return false;
					_value[_vcurrent++] = c;
				}
				else
				{
					_value[_vcurrent] = 0;
					_vcurrent = 0;
					_state = ATTRIBUTE_END;

					events.DispatchAttributeSpecEvent(_name, _value);
				}
			} break;
			case ATTRIBUTE_END:
			{
				if (c == ' ')
				{
					_state = AWAIT_SPACE;
					_afterSpace = ATTRIBUTE_DETECT;
				}
				else if (c == '/')
				{
					events.DispatchAttributeSpecEndEvent();
					_state = IMMEDIATE_CLOSE;
				}
				else if (c == '>')
				{
					events.DispatchAttributeSpecEndEvent();
					_state = TAG_OPENED;
				}
				else return false;
			} break;
			case IMMEDIATE_CLOSE:
			{
				if (c != '>') return false;
				events.DispatchTagClosedEvent(nullptr);
				_state = START;
			} break;
			case TAG_OPENED:
			{
				++_level;

				if (c != '<')
				{
					_state = TEXT_DETECT;
					_vcurrent = 0;
					return nextchar(c);
				}
				else
				{
					_state = START;
					return nextchar(c);
				}
			} break;
			case TEXT_DETECT:
			{
				if (c != '<') // &lt; inside text
				{
					if (_vcurrent >= 64) return false;
					_value[_vcurrent++] = c;
				}
				else
				{
					_value[_vcurrent] = 0;
					_vcurrent = 0;

					events.DispatchTextValueEvent(_value);

					_state = TAG_CLOSE;
				}
			} break;
			case TAG_CLOSE:
			{
				if (c != '/') return false;
				_ncurrent = 0;
				_state = TAG_CLOSING;
			} break;
			case TAG_CLOSING:
			{
				if (is_alphanumeric(c))
				{
					if (_ncurrent >= 64) return false;
					_name[_ncurrent++] = c;
				}
				else if (c == '>')
				{
					if (_ncurrent == 0) return false;

					_name[_ncurrent] = 0;
					_ncurrent = 0;

					events.DispatchTagClosedEvent(_name);

					--_level;
					_state = START;
				}
				else return false;
			} break;
		}

		return true;
	}
};
