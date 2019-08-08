#pragma once

#include "../abstraction/component.h"

#include <system.h>
#include <thread.h>
#include <synchronization/lock.h>
#include <synchronization/mutex.h>

#include <structures/specialized/vlist.h>
#include <dependency.h>

class Toggleable
{
	friend class Toggler;

	private: Time freq = 0;
	private: Time point = 0;
	
	public: virtual ~Toggleable() { }
	
	protected: virtual void _toggling_begin() { }
	protected: virtual void _toggle() = 0;
	protected: virtual void _toggling_end() { }
};

class Toggler final
{
	private: static ThreadDelegate _delegate;
	
	public: static Toggler& instance() { static Toggler toggler; return toggler; }
	
	private: vlist<Toggleable> lst;
	
	private: Thread _toggler;
	private: mutex mtx;
	
	private: Toggler() : lst(D::nodes), _toggler(_delegate, this) { }
	
	private: void rotate(Toggleable* toggleable)
	{
		if (toggleable->freq < 2) return;

		toggleable->point = System::millis() + toggleable->freq / 2;

		auto prev = lst.end();

		for (auto i = lst.begin(); i != lst.end(); prev = i, ++i)
		{
			if (i->point >= toggleable->point) break;
		}

		if (prev != lst.end()) prev.insertAfter(toggleable);
		else lst.push_front(toggleable);
	}
	private: Time try_wakeup()
	{
		lock lck(mtx);

		while (true)
		{
			if (lst.size() == 0) return 0;
			Toggleable* first = lst.peek_front();
			Time current = System::millis();
			if (current < first->point) return first->point - current;
			lst.remove_front();
			rotate(first);

			lck.mtx.unlock();
			first->_toggle();
			lck.mtx.lock();
		}
	}
	
	public: void toggle(Toggleable* toggleable, Time freq)
	{
		if (freq < 2) return;

		lock lck(mtx);

		if (toggleable->freq != 0)
		{
			toggleable->freq = freq;
			return;
		}

		toggleable->freq = freq;
		toggleable->point = 0;

		toggleable->_toggling_begin();
		rotate(toggleable);
		return;
	}
	public: void stop(Toggleable* toggleable)
	{
		lock lck(mtx);

		auto prev = lst.end();
		auto i = lst.begin();

		for (; i != lst.end(); prev = i, ++i)
		{
			if (*i == toggleable) break;
		}

		if (i != lst.end())
		{
			if (prev == lst.end()) lst.remove_front();
			else prev.removeNext();
			toggleable->_toggling_end();
		}

		toggleable->freq = 0;
		toggleable->point = 0;
	}
};
