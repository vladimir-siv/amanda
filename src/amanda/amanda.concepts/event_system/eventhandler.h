#pragma once

#include "../types/type_traits.h"
#include "delegate.h"
#include "eventargs.h"

template <class T>
class EventHandler : public IDelegate<void, void*, T>
{
	private: static inline void __constraint__() { EventArgs* e = static_cast<T>(nullptr); }
	
	protected: IDelegate<void, void*, T>* ptr;
	
	public: EventHandler(IDelegate<void, void*, T>* ptr) : ptr(ptr) { __constraint__(); }
	public: EventHandler(Executable<void, void*, T>&& exec) : EventHandler(exec.clone()) { }
	public: template <typename R> EventHandler(Callback<R, void, void*, T>&& call) : EventHandler(call.clone()) { }
	public: virtual ~EventHandler() { delete ptr; ptr = nullptr; }
	
	public: virtual void invoke(void* sender, T e) const { ptr->invoke(sender, e); }
	public: virtual IDelegate<void, void*, T>* clone() const { return new EventHandler(ptr->clone()); }
};
