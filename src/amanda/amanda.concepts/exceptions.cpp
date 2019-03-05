#include "exceptions.h"

#if MAX_EXCEPTIONS > 1

#include "structures/specialized/qbuffer.h"
QBuffer<Exception*> Context(MAX_EXCEPTIONS);

namespace Exceptions
{	
	void Throw(Exception* ex) { Context.push(ex); }
	Exception* Fetch() { return Context.size() > 0 ? Context.pop() : nullptr; }
	Exception* Check() { return Context.size() > 0 ? Context.peek() : nullptr; }
	unsigned int Count() { return Context.size(); }
	void Clear() { Context.clear(); }
}

#else

Exception* Last = nullptr;

namespace Exceptions
{
	void Throw(Exception* ex) { Last = ex; }
	Exception* Fetch() { Exception* ex = Last; Last = nullptr; return ex; }
	Exception* Check() { return Last; }
	unsigned int Count() { return Last != nullptr ? 1 : 0 }
	void Clear() { Last = nullptr; }
}

#endif
