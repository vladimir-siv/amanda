#pragma once

namespace math
{
	template <typename T> inline T vmin(T arg1, T arg2) { return arg1 < arg2 ? arg1 : arg2; }
}
