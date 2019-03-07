#pragma once

#include "../types/type_traits.h"
#include "delegate.h"
#include "../structures/list.h"

template <typename... Args>
class Event final
{
	private: List<IDelegate<void, Args...>*> handlers;
	
	public: Event& operator+=(IDelegate<void, Args...>* del) { handlers.push_back(del); return *this; }
	public: Event& operator-=(IDelegate<void, Args...>* del) { handlers.remove_last(del); return *this; }
	
	public: void dispose()
	{
		Executable<void, IDelegate<void, Args...>*> exec([](IDelegate<void, Args...>* del) -> void { delete del; });
		handlers.traverse(&exec);
		handlers.clear();
	}
	public: void clear(bool deep = false)
	{
		if (deep) { dispose(); }
		else handlers.clear();
	}
	
	public: template <typename... EArgs>
	void fire(EArgs&&... arg) const
	{
		for (ListEnumerator<IDelegate<void, Args...>*> i = handlers.begin(); i != handlers.end(); ++i)
		{
			i->invoke(std::forward<EArgs>(arg)...);
		}
	}
};
