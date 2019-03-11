#pragma once

#include "mutex.h"

class lock final
{
	public: mutex& mtx;
	public: lock(mutex& mtx, bool lock = true) : mtx(mtx) { if (lock) mtx.lock(); }
	public: ~lock() { mtx.unlock(); }
};
