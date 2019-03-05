#pragma once

class Exception
{
	public: Exception() = default;
	public: Exception(const Exception& ex) = default;
	public: Exception(Exception&& ex) = default;
	public: Exception& operator=(const Exception& ex) = default;
	public: Exception& operator=(Exception&& ex) = default;
	public: virtual ~Exception() = default;

	public: virtual const char* what() const
	{
		return "An exception occured.";
	}
};
