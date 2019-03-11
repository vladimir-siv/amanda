#pragma once

#include "../types/type_traits.h"
#include "delegate.h"
#include "../structures/list.h"

template <typename... Args>
class Event final
{
	private: list<IDelegate<void, Args...>*> subscriptions;
	
	public: Event& operator+=(IDelegate<void, Args...>* del) { subscriptions.push_back(del); return *this; }
	public: Event& operator-=(IDelegate<void, Args...>* del) { subscriptions.remove_last(del); return *this; }
	
	public: void dispose()
	{
		Executable<void, IDelegate<void, Args...>*> exec([](IDelegate<void, Args...>* del) -> void { delete del; });
		subscriptions.traverse(&exec);
		subscriptions.clear();
	}
	public: void clear(bool deep = false)
	{
		if (deep) { dispose(); }
		else subscriptions.clear();
	}
	
	public: template <typename... EArgs>
	void fire(EArgs&&... arg) const
	{
		for (auto i = subscriptions.begin(); i != subscriptions.end(); ++i)
		{
			i->invoke(std::forward<EArgs>(arg)...);
		}
	}
};
