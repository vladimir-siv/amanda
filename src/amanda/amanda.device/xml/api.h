#pragma once

#include <networking/xml/xml_parser.h>

#include "stream.h"

namespace xml
{
	class SAXParser
	{
		public: class Context final
		{
			friend class SAXParser;

			private: class Dispatcher final
			{
				friend class Context;

				private: SAXParser* _executing = nullptr;
				
				private: bool isExecuting() const { return _executing != nullptr; }
				private: bool finish(bool success = true) { _executing = nullptr; return success; }
				private: bool set(SAXParser* parser)
				{
					if (_executing != nullptr) return false;
					_executing = parser;
					return true;
				}
				private: SAXParser* swap(SAXParser* parser)
				{
					if (_executing == nullptr) return nullptr;
					SAXParser* last = _executing;
					_executing = parser;
					return last;
				}
				
				private: void tag_opened(const char* tagname) { if (_executing) _executing->tag_opened(tagname); }
				private: void attribute_spec(const char* attrname, const char* attrvalue) { if (_executing) _executing->attribute_spec(attrname, attrvalue); }
				private: void attribute_spec_end() { if (_executing) _executing->attribute_spec_end(); }
				private: void text_value(const char* value) { if (_executing) _executing->text_value(value); }
				private: void tag_closed(const char* tagname) { if (_executing) _executing->tag_closed(tagname); }
			};
			
			private: XmlParser _exec_unit;
			private: Dispatcher _dispatcher;
			private: Stream* _stream;
			private: Callback<Dispatcher, void, const char*> _tag_opened;
			private: Callback<Dispatcher, void, const char*, const char*> _attribute_spec;
			private: Callback<Dispatcher, void> _attribute_spec_end;
			private: Callback<Dispatcher, void, const char*> _text_value;
			private: Callback<Dispatcher, void, const char*> _tag_closed;
			
			private: Context() :
				_tag_opened(_dispatcher, &Dispatcher::tag_opened),
				_attribute_spec(_dispatcher, &Dispatcher::attribute_spec),
				_attribute_spec_end(_dispatcher, &Dispatcher::attribute_spec_end),
				_text_value(_dispatcher, &Dispatcher::text_value),
				_tag_closed(_dispatcher, &Dispatcher::tag_closed)
			{
				_exec_unit.events.SetTagOpenedEventHandler(&_tag_opened);
				_exec_unit.events.SetAttributeSpecEventHandler(&_attribute_spec);
				_exec_unit.events.SetAttributeSpecEventEndHandler(&_attribute_spec_end);
				_exec_unit.events.SetTextValueEventHandler(&_text_value);
				_exec_unit.events.SetTagClosedEventHandler(&_tag_closed);
			}
			
			private: bool isExecuting() const { return _dispatcher.isExecuting(); }
			private: bool set(SAXParser* parser) { return _dispatcher.set(parser); }
			private: void reset(Stream* stream) { _exec_unit.reset(); _stream = stream; }
			private: SAXParser* swap(SAXParser* parser) { return _dispatcher.swap(parser); }
			private: bool eos() const { return _stream == nullptr || _stream->eos(); }
			private: bool nextchar() { return _exec_unit.nextchar(_stream->advance()); }
			private: bool finish(bool success = true) { _stream = nullptr; return _dispatcher.finish(success); }
		};
		
		public: static Context createContext()
		{
			Context _context;
			return _context;
		}
		public: static Context& defaultContext()
		{
			static Context _default;
			return _default;
		}
		
		private: Context& _context;
		private: bool _finish;
		private: bool _cancel;
		
		public: SAXParser(Context& context = defaultContext()) : _context(context) { }
		public: virtual ~SAXParser() { }
		
		private: bool _oncancel(bool finish = true)
		{
			oncancel();
			if (finish) return _context.finish(false);
			else return false;
		}
		
		protected: bool swap(SAXParser& parser)
		{
			if (&parser == this) return false;
			if (&parser._context != &_context) return false;
			if (!_context.isExecuting()) return false;

			SAXParser* prev = _context.swap(&parser);

			parser._finish = false;
			parser._cancel = false;

			parser.reset();

			while (!parser._finish && !_context.eos())
			{
				if (!_context.nextchar()) return false;
				if (parser._cancel) return parser._oncancel(false);
			}

			if (_context.swap(prev) != &parser) return false;

			return true;
		}
		public: bool parse(Stream&& xml) { return parse(xml); }
		public: bool parse(Stream& xml)
		{
			if (!_context.set(this)) return false;

			_context.reset(&xml);

			_finish = false;
			_cancel = false;

			reset();

			while (!_finish && !_context.eos())
			{
				if (!_context.nextchar()) return _context.finish(false);
				if (_cancel) return _oncancel();
			}

			return _context.finish();
		}
		
		protected: void cancel() { _cancel = true; }
		protected: bool canceled() const { return _cancel; }
		protected: void finish() { _finish = true; }
		protected: bool finished() const { return _finish; }
		
		protected: virtual void reset() { }
		protected: virtual void oncancel() { }
		protected: virtual void tag_opened(const char* tagname) = 0;
		protected: virtual void attribute_spec(const char* attrname, const char* attrvalue) = 0;
		protected: virtual void attribute_spec_end() = 0;
		protected: virtual void text_value(const char* value) = 0;
		protected: virtual void tag_closed(const char* tagname) = 0;
	};
}
