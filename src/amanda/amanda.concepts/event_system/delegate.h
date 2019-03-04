#pragma once

template <typename Tout = void, typename... Args> using executable = Tout(*)(Args...);
template <typename T, typename Tout = void, typename... Args> using callback = Tout(T::*)(Args...);

template <typename Tout = void, typename... Args>
class IDelegate
{
	public: inline Tout operator()(Args... args) const { return invoke(std::forward<Args>(args)...); }
	public: virtual Tout invoke(Args... args) const = 0;
	public: virtual IDelegate* clone() const { return nullptr; }
};

template <typename Tout = void, typename... Args>
class Executable : public IDelegate<Tout, Args...>
{
	protected: executable<Tout, Args...> ptr;
	public: Executable(executable<Tout, Args...> executable) : ptr(executable) { }
	public: virtual Tout invoke(Args... args) const override { return (*ptr)(std::forward<Args>(args)...); }
	public: virtual Executable* clone() const override { return new Executable(*this); }
};

template <typename T, typename Tout = void, typename... Args>
class Callback : public IDelegate<Tout, Args...>
{
	protected: T& obj;
	protected: callback<T, Tout, Args...> ptr;
	public: Callback(T& obj, callback<T, Tout, Args...> callback) : obj(obj), ptr(callback) { }
	public: virtual Tout invoke(Args... args) const override { return (obj.*ptr)(std::forward<Args>(args)...); }
	public: virtual Callback* clone() const override { return new Callback(*this); }
};
