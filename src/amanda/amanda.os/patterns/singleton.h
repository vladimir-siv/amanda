#pragma once

template <class T>
class S final
{
	private: static T instance;
	public: static T& I() { return instance; }
};

template<class T> T S<T>::instance;
