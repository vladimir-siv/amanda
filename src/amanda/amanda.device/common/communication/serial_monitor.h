#pragma once

#include <Arduino.h>

#include <system.h>

namespace communication
{
	class SerialMonitor final
	{
		private: SerialMonitor() { }
		
		public: class Functional final
		{
			friend class SerialMonitor;
			public: using Delegate = void (*)();
			
			private: Delegate del;
			private: Functional(Delegate del) : del(del) { }
			public: Functional& operator()() { del(); return *this; }
			public: const Functional& operator()() const { del(); return *this; }
		};
		public: static Functional endl;
		
		public: static void begin(unsigned long baud = 9600)
		{
			Serial.begin(baud);
			while (!Serial) ;
			Serial.flush();
		}
		
		private: template <typename T> static void _print(T&& param) { Serial.print(std::forward<T>(param)); }
		private: static void _print(Functional& param) { param(); }
		
		private: static void _pass() { }
		private: template <typename T, typename... Types> static void _pass(T&& param, Types&&... params)
		{
			_print(std::forward<T>(param));
			_pass(std::forward<Types>(params)...);
		}
		
		public: template <typename... Types> static void print(Types&&... params)
		{
			System::lock();
			_pass(std::forward<Types>(params)...);
			System::unlock();
		}
		public: template <typename... Types> static void println(Types&&... params)
		{
			System::lock();
			_pass(std::forward<Types>(params)...);
			Serial.println();
			System::unlock();
		}
	};
}
