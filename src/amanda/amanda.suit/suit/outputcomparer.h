#pragma once

#include "../suit/assert.h"

#include <event_system/delegate.h>
#include <RAII/generic.h>

#include <string>
#include <sstream>
#include <list>

template <typename T>
class OutputComparer
{
	private: std::list<T> output;
	
	public: auto appender() { return new Callback<OutputComparer, void, const T&>(*this, &OutputComparer::append); }
	public: void append(const T& value) { output.push_back(value); }
	public: void append(T&& value) { output.push_back(value); }
	public: void clear() { output.clear(); }
	
	public: int size() const { return output.size(); }
	
	public: const T& current() const { if (output.size() <= 0) throw std::exception("Output comparer is empty"); return output.front(); }
	public: void advance() { if (output.size() > 0) output.pop_front(); }
	public: void next(const T& value, const char* message = "Values are not equal;")
	{
		RAII::Generic afterExecution(Callback<OutputComparer>(*this, &OutputComparer::advance));
		
		if (output.size() <= 0) throw AssertError("Attempt to assert failed due to empty output value list");
		
		T& nextValue = output.front();

		std::stringstream s;
		s << message << " Value is \"" << nextValue << "\" while assert checks for \"" << value << "\"";

		assert::areEqual(nextValue, value, s.str().c_str());
	}
};
