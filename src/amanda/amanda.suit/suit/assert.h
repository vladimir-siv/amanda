#pragma once

#include <cassert>
#include <exception>
#include <string>

class AssertException : std::exception
{
	//private: const char* _message = "";
	private: const std::string _message;
	public: AssertException() : AssertException("Assertion failed") { }
	public: AssertException(const char* message) : _message(message) { }
	public: virtual const char* what() const override { return _message.c_str(); }
};

class AssertError : std::exception
{
	//private: const char* _message = "";
	private: const std::string _message;
	public: AssertError() : AssertError("Assertion error") { }
	public: AssertError(const char* message) : _message(message) { }
	public: virtual const char* what() const override { return _message.c_str(); }
};

class TestNotImplemented : std::exception
{
	//private: const char* _message = "";
	private: const std::string _message;
	public: TestNotImplemented() : TestNotImplemented("Test not implemented yet") { }
	public: TestNotImplemented(const char* message) : _message(message) { }
	public: virtual const char* what() const override { return _message.c_str(); }
};

namespace assert
{
	using callback = void (*)();

	template <typename T>
	void isTrue(T&& value, const char* message = "Value is not true")
	{
		if (value == true) return;
		throw AssertException(message);
	}
	template <typename T>
	void isFalse(T&& value, const char* message = "Value is not false")
	{
		if (value == false) return;
		throw AssertException(message);
	}
	template <typename T1, typename T2>
	void areEqual(T1&& e1, T2&& e2, const char* message = "Values are not equal")
	{
		if (e1 == e2) return;
		throw AssertException(message);
	}
	template <typename T1, typename T2>
	void areNotEqual(T1&& e1, T2&& e2, const char* message = "Values are equal")
	{
		if (e1 != e2) return;
		throw AssertException(message);
	}
	template <typename T>
	void doesThrow(callback cb, const char* message = "Given exception was not thrown")
	{
		try { cb(); }
		catch (const T& ex) { return; }
		catch (...) {  }
		throw AssertException(message);
	}
	template <typename T>
	void doesNotThrow(callback cb, const char* message = "Given exception was thrown")
	{
		try { cb(); return; }
		catch (const T& ex) { }
		catch (...) { return; }
		throw AssertException(message);
	}
}
