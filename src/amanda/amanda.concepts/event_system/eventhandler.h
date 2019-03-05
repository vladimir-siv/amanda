#pragma once

#include "delegate.h"
#include "eventargs.h"

template <class T>
class EventHandler
{
	//friend class Event<EventHandler<T>>;

	private: static inline void __constraint__() { EventArgs* e = static_cast<T*>(nullptr); }
	
	protected: IDelegate<void, void*, T>* ptr;
	
	public: EventHandler(IDelegate<void, void*, T>* ptr) : ptr(ptr) { __constraint__(); }
};
