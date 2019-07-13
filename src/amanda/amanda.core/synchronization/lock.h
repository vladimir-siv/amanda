#pragma once

#include "mutex.h"

class lock final
{
	public: volatile mutex& mtx;
	public: lock(volatile mutex& mtx, bool lock = true) : mtx(mtx) { if (lock) mtx.lock(); }
	public: ~lock() { mtx.unlock(); }
};
