#include <suit/assert.h>
#include <suit/outputcomparer.h>

#include <RAII/generic.h>

#include <string>

class RAIITest
{
	public: static OutputComparer<std::string> o;
	public: static RAIITest instance;
	public: static void staticMethod() { o.append("static_call"); }
	public: void nonStaticMethod() { o.append("nonstatic_call"); }
};

OutputComparer<std::string> RAIITest::o;
RAIITest RAIITest::instance;

void eval_raii()
{
	RAIITest::o.append("before_creation");

	RAII::Generic afterExecution2(new Executable<>(&RAIITest::staticMethod), true);
	RAII::Generic afterExecution1(Callback<RAIITest>(RAIITest::instance, &RAIITest::nonStaticMethod));

	RAIITest::o.append("before_return");
}

void _raii()
{
	RAIITest::o.append("before_call");
	eval_raii();
	RAIITest::o.append("after_call");

	OutputComparer<std::string>& o = RAIITest::o;
	assert::areEqual(o.size(), 6, "[A1] Size not equal to 6");
	o.next("before_call", "[A2]");
	o.next("before_creation", "[A3]");
	o.next("before_return", "[A4]");
	o.next("nonstatic_call", "[A5]");
	o.next("static_call", "[A6]");
	o.next("after_call", "[A7]");
}
