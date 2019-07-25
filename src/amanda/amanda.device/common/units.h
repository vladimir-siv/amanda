#pragma once

#include <Arduino.h>

/**
 * Unit prefixes are optimized for engineering. The table of contents
 * is presented below. Column 'Using' displays used prefixes in the
 * system. Note that only kilo, hecto and deca have different symbols.
 * Character 'u' is used for micro for the sake of simplicity only
 * (some clients may have difficulties with micro).
 *
 * Also, unit system naming convention is a bit different. The term
 * 'measurement' stands only for what's called a conventional measuring
 * unit but without a prefix. The term 'unit' stands for both,
 * prefix and 'measurement' bound together.
 * 
 *		+========+========+=======+=======+
 *		| Prefix | Symbol | Using | Scale |
 *		+========+========+=======+=======+
 *		| yotta  |   Y    |   Y   |  +24  |
 *		+--------+--------+-------+-------+
 *		| zetta  |   Z    |   Z   |  +21  |
 *		+--------+--------+-------+-------+
 *		|  exa   |   E    |   E   |  +18  |
 *		+--------+--------+-------+-------+
 *		|  peta  |   P    |   P   |  +15  |
 *		+--------+--------+-------+-------+
 *		|  tera  |   T    |   T   |  +12  |
 *		+--------+--------+-------+-------+
 *		|  giga  |   G    |   G   |  +9   |
 *		+--------+--------+-------+-------+
 *		|  mega  |   M    |   M   |  +6   |
 *		+--------+--------+-------+-------+
 *		|  kilo  |   k    |   K   |  +3   |
 *		+--------+--------+-------+-------+
 *		| hecto  |   h    |   H   |  +2   |
 *		+--------+--------+-------+-------+
 *		|  deca  |   da   |   D   |  +1   |
 *		+--------+--------+-------+-------+
 *		|   /    |   /    |\space |   0   |
 *		+--------+--------+-------+-------+
 *		|  deci  |   d    |   d   |  -1   |
 *		+--------+--------+-------+-------+
 *		| centi  |   c    |   c   |  -2   |
 *		+--------+--------+-------+-------+
 *		| milli  |   m    |   m   |  -3   |
 *		+--------+--------+-------+-------+
 *		| micro  |  \mu   |   u   |  -6   |
 *		+--------+--------+-------+-------+
 *		|  nano  |   n    |   n   |  -9   |
 *		+--------+--------+-------+-------+
 *		|  pico  |   p    |   p   |  -12  |
 *		+--------+--------+-------+-------+
 *		| femto  |   f    |   f   |  -15  |
 *		+--------+--------+-------+-------+
 *		|  ato   |   a    |   a   |  -18  |
 *		+--------+--------+-------+-------+
 *		| zepto  |   z    |   z   |  -21  |
 *		+--------+--------+-------+-------+
 *		| yocto  |   y    |   y   |  -24  |
 *		+--------+--------+-------+-------+
 */

using unitconverter = double (*)(double);

class unitconstants final
{
	private: unitconstants() { }
	
	public:
		static constexpr const double celsius() { return 2.046 /*2.15*/; }
};

// [flyweight]
class measurement final
{
	friend class measurements;

	private: static measurement known[];
	
	public: measurement(const measurement&) = delete;
	public: measurement(measurement&&) = delete;
	public: measurement& operator=(const measurement&) = delete;
	public: measurement& operator=(measurement&&) = delete;
	
	public: static measurement* resolve(const char* label);
	
	public: const char* const label;
	public: const unitconverter converter;
	
	private: measurement(const char* label, unitconverter converter) :
		label(label),
		converter(converter)
	{ }
	
	public: double convert(double value) const;
};

// [flyfactory(measurement)]
class measurements final
{
	private: measurements() { }
	
	public: static measurement& unnamed() { return measurement::known[0]; }
	public: static measurement& volts() { return measurement::known[1]; }
	public: static measurement& lux() { return measurement::known[2]; }
	public: static measurement& celsius() { return measurement::known[3]; }
};

class unit final
{
	public: static long scale(char prefix);
	public: static double magnitude(char prefix);
	
	public: static bool isValidPrefix(char prefix);
	
	private: char _prefix;
	private: measurement* _measure;
	
	public: unit() { reset(); }
	public: unit(char prefix, const char* label) { if (!set(prefix, label)) to_default(); }
	public: unit(const char* unit) { if (!set(unit)) to_default(); }
	
	public: void reset()
	{
		_prefix = ' ';
		_measure = nullptr;
	}
	public: void to_default()
	{
		_prefix = ' ';
		_measure = &measurements::unnamed();
	}
	
	public: bool isset() const { return _measure != nullptr; }
	
	public: char prefix() const { return _prefix; }
	public: measurement* measure() const { return _measure; }
	
	public: long scale() const { return unit::scale(_prefix); }
	public: double magnitude() const { return unit::magnitude(_prefix); }
	
	public: bool set(char prefix, const char* label)
	{
		auto measure = measurement::resolve(label);

		if (isValidPrefix(prefix) && measure != nullptr)
		{
			_prefix = prefix;
			_measure = measure;
			return true;
		}

		return false;
	}
	public: bool set(const char* unit)
	{
		if (unit == nullptr || strcmp(unit, "") == 0) return false;
		if (set(*unit, unit + 1)) return true;
		if (set(' ', unit)) return true;
		return false;
	}
	
	public: double convert(double value) const
	{
		if (_measure == nullptr) return value * magnitude();
		return _measure->convert(value * magnitude());
	}
};
