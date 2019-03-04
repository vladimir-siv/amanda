#include "coord.h"

#include <event_system/delegate.h>

#include <string>
#include <sstream>
#include <list>

unsigned Coord::id_gen = 0;
int Coord::generate_id()											{ return id_gen++; }
void Coord::reset_ids()												{ id_gen = 0; }

std::list<const IDelegate<void, const std::string&>*> Coord::_onwrite;
void Coord::listen(const IDelegate<void, const std::string&>* del)	{ _onwrite.push_back(del); }
void Coord::stop(const IDelegate<void, const std::string&>* del)	{ _onwrite.remove(del); }
void Coord::onwrite(const std::string& what)						{ for (auto i = _onwrite.begin(); i != _onwrite.end(); ++i) (*i)->invoke(what); }

void Coord::copy(const Coord& coord)								{ x = coord.x; y = coord.y; }
void Coord::write(const char* what)									{ std::stringstream s; s << "Coord<" << id << ">(" << x << ", " << y << ")::" << what; std::string str = s.str(); onwrite(str); }

Coord::Coord() : Coord(0, 0)										{ }
Coord::Coord(int x, int y) : x(x), y(y)								{ write("ctor"); }
Coord::Coord(const Coord& coord)									{ copy(coord); write("copy"); }
Coord::Coord(Coord&& coord)											{ copy(coord); write("move"); }
Coord::~Coord()														{ write("dtor"); }
Coord& Coord::operator=(const Coord& coord)							{ copy(coord); write("copy="); return *this; }
Coord& Coord::operator=(Coord&& coord)								{ copy(coord); write("move="); return *this; }
