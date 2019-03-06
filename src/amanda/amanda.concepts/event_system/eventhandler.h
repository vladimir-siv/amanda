#pragma once

#include "delegate.h"
#include "eventargs.h"

template <typename T> class Event;

template <class T>
class EventHandler
{
	private: static inline void __constraint__() { EventArgs* e = static_cast<T*>(nullptr); }
	
	friend class Event<EventHandler<T>>;
	
	protected: IDelegate<void, void*, T>* ptr;
	
	public: EventHandler(IDelegate<void, void*, T>* ptr) : ptr(ptr) { __constraint__(); }
};
