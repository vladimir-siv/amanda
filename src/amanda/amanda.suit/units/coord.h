#pragma once

#include <event_system/delegate.h>

#include <string>
#include <list>

class Coord
{
	public: class Subscription final
	{
		private: const IDelegate<void, const std::string&>* del;
		public: Subscription(const IDelegate<void, const std::string&>* del) : del(del) { Coord::listen(del); }
		public: ~Subscription() { Coord::stop(del); delete del; del = nullptr; }
	};
	
	protected: static unsigned int id_gen;
	protected: static int generate_id();
	public: static void reset_ids();
	
	protected: static std::list<const IDelegate<void, const std::string&>*> _onwrite;
	public: static void listen(const IDelegate<void, const std::string&>* del);
	public: static void stop(const IDelegate<void, const std::string&>* del);
	protected: static void onwrite(const std::string& what);
	
	public: unsigned int id = generate_id();
	public: int x, y;
	
	protected: void copy(const Coord& coord);
	protected: void write(const char* what = "<not_specified>");
	
	public: Coord();
	public: Coord(int x, int y);
	public: Coord(const Coord& coord);
	public: Coord(Coord&& coord);
	public: virtual ~Coord();
	public: Coord& operator=(const Coord& coord);
	public: Coord& operator=(Coord&& coord);
};
