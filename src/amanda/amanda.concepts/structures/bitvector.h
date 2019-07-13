#pragma once

#include "../types/def.h"
#include "../types/type_traits.h"
#include "../exceptions.h"

template <size_t size>
class bitvector final
{
	private: volatile byte vector[size / (sizeof(byte) * 8) + (size % (sizeof(byte) * 8) > 0 ? 1 : 0)] = { };
	
	public: bool isset(unsigned int i) volatile const
	{
		return (vector[i / (sizeof(byte) * 8)] & ((byte)1 << (i % (sizeof(byte) * 8)))) > 0;
	}
	public: void set(unsigned int i) volatile
	{
		vector[i / (sizeof(byte) * 8)] |= ((byte)1 << (i % (sizeof(byte) * 8)));
	}
	public: void unset(unsigned int i) volatile
	{
		vector[i / (sizeof(byte) * 8)] &= ~((byte)1 << (i % (sizeof(byte) * 8)));
	}
	public: void value(unsigned int i, bool value) volatile
	{
		if (value) set(i);
		else unset(i);
	}
};
