#pragma once

#define _TYPENAME_DISABLE_

template <typename Type> class TypeName final
{
	private: static TypeName* assemble()
	{
#ifndef _MSVC_LANG

		const char* xname = __PRETTY_FUNCTION__ + 63;

		int n = 0;
		while (xname[n]) ++n;
		++n;

		char* name = new char[n + 1];

		name[0] = '[';
		for (int i = 0; i < n - 1; ++i) name[i + 1] = xname[i];
		name[n] = 0;

		return new TypeName(name, n);

#else

		const char* xname = __FUNCSIG__ + 14;

		int n = 1, k = 1;
		while (k > 0)
		{
			if (xname[n] == '<') ++k;
			if (xname[n] == '>') --k;
			++n;
		}

		char* name = new char[n + 1];

		for (int i = 0; i < n; ++i) name[i] = xname[i];
		name[n] = 0;

		return new TypeName(name, n);

#endif
	}
	
	private: static TypeName* instance;
	public: static TypeName& resolve() { return *instance; }
	
	public: const char* const name;
	public: const int size;
	private: TypeName(const char* const name, const int size) : name(name), size(size) { }
	private: TypeName(const TypeName&) = delete;
	private: TypeName(TypeName&&) = delete;
	public: ~TypeName() { delete name; name = nullptr; size = 0; }
	private: TypeName& operator=(const TypeName&) = delete;
	private: TypeName& operator=(TypeName&&) = delete;
};

template <typename Type> TypeName<Type>* TypeName<Type>::instance = TypeName<Type>::assemble();

#ifndef _TYPENAME_DISABLE_
#define type_name(type) (TypeName<type>::resolve().name)
#else
#define type_name(type) ("<TypeName Disabled>")
#endif
