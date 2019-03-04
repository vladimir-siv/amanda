#pragma once

#include "../event_system/delegate.h"

namespace RAII
{
	class Generic
	{
		private: const IDelegate<>* del;
		private: bool shouldDelete = false;
		
		private: void copy(const Generic& gen)
		{
			del = gen.del->clone();
			shouldDelete = gen.shouldDelete;
		}
		private: void move(Generic& gen)
		{
			del = gen.del; gen.del = nullptr;
			shouldDelete = gen.shouldDelete; gen.shouldDelete = false;
		}
		private: void clear() { if (shouldDelete) delete del; }
		
		public: Generic(const IDelegate<>* del, bool shouldDelete = false) : del(del), shouldDelete(shouldDelete) { }
		public: Generic(const IDelegate<>& del) : Generic(&del) { }
		public: Generic(IDelegate<>&& del) : Generic(del.clone(), true) { }
		public: virtual ~Generic() { del->invoke(); clear(); }
		
		public: Generic(const Generic& gen) { copy(gen); }
		public: Generic(Generic&& gen) { move(gen); }
		public: Generic& operator=(const Generic& gen)
		{
			if (&gen != this) { clear(); copy(gen); }
			return *this;
		}
		public: Generic& operator=(Generic&& gen)
		{
			if (&gen != this) { clear(); move(gen); }
			return *this;
		}
	};
}
