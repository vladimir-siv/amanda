#include "component.h"

/*
class CommandResult
{
	public: virtual ~CommandResult() { }
};

class IOResult : public CommandResult
{
	protected: union Result final
	{
		DigitalState digital;
		AnalogValue analog;
		Result(DigitalState digital) : digital(digital) { }
		Result(AnalogValue analog) : analog(analog) { }
		bool equals(const Result& other) const { return digital == other.digital || analog == other.analog; }
	} _result;
	
	public: IOResult() : IOResult(0) { }
	public: IOResult(DigitalState digitalValue) : _result(digitalValue) { }
	public: IOResult(AnalogValue analogValue) : _result(analogValue) { }
	public: virtual ~IOResult() { }
	
	public: virtual void set(DigitalState digitalValue) { _result.digital = digitalValue; }
	public: virtual void set(AnalogValue analogValue) { _result.analog = analogValue; }
	
	public: virtual DigitalState as_digital() const { return _result.digital; }
	public: virtual AnalogValue as_analog() const { return _result.analog; }
};
*/
