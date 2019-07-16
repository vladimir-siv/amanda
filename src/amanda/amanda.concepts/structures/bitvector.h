#pragma once

#include "../types/def.h"
#include "../types/type_traits.h"
#include "../exceptions.h"

template <size_t size>
class bitvector final
{
	private: byte vector[size / (sizeof(byte) * 8) + (size % (sizeof(byte) * 8) > 0 ? 1 : 0)] = { };
	
	public: inline bool isset(unsigned int i) const
	{
		return (vector[i / (sizeof(byte) * 8)] & ((byte)1 << (i % (sizeof(byte) * 8)))) > 0;
	}
	public: inline void set(unsigned int i)
	{
		vector[i / (sizeof(byte) * 8)] |= ((byte)1 << (i % (sizeof(byte) * 8)));
	}
	public: inline void unset(unsigned int i)
	{
		vector[i / (sizeof(byte) * 8)] &= ~((byte)1 << (i % (sizeof(byte) * 8)));
	}
	public: inline void value(unsigned int i, bool value)
	{
		if (value) set(i);
		else unset(i);
	}
};
