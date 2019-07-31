#include "units.h"

// If given a certain value of any measurement other than volts, which function should convert such value in volts
namespace converters
{
	auto nullout = [](double value) -> double { return 0.0; };
	auto direct = [](double value) -> double { return value; };
	auto percent = [](double value) -> double { return value / 20.0; };
	auto lux = [](double value) -> double { return log(value); }; // not valid! should be inverse of interpolation used at ldr.cpp
	auto celsius = [](double value) -> double { return value * unitconstants::celsius(); };
}

// changes here affect class measurements (the flyfactory)
measurement measurement::known[]
{
	{ "n", converters::direct },
	{ "V", converters::direct },
	{ "%", converters::percent },
	{ "lux", converters::lux },
	{ "*C", converters::celsius },
	{ "s", converters::nullout }
};

measurement* measurement::resolve(const char* label)
{
	if (label == nullptr || strcmp_P(label, PSTR("")) == 0) return nullptr;

	for (unsigned int i = 0; i < sizeof(known) / sizeof(measurement); ++i)
	{
		if (strcmp(label, known[i].label) == 0)
		{
			return &known[i];
		}
	}

	return nullptr;
}

double measurement::convert(double value) const
{
	if (converter == nullptr) return value;
	return (*converter)(value);
}

long unit::scale(char prefix)
{
	switch (prefix)
	{
		case 'Y': return +24;
		case 'Z': return +21;
		case 'E': return +18;
		case 'P': return +15;
		case 'T': return +12;
		case 'G': return +9;
		case 'M': return +6;
		case 'K': return +3;
		case 'H': return +2;
		case 'D': return +1;
		case ' ': return 0;
		case 'd': return -1;
		case 'c': return -2;
		case 'm': return -3;
		case 'u': return -6;
		case 'n': return -9;
		case 'p': return -12;
		case 'f': return -15;
		case 'a': return -18;
		case 'z': return -21;
		case 'y': return -24;
		default: return 0;
	}
}

double unit::magnitude(char prefix)
{
	switch (prefix)
	{
		case 'Y': return 1e+24;
		case 'Z': return 1e+21;
		case 'E': return 1e+18;
		case 'P': return 1e+15;
		case 'T': return 1e+12;
		case 'G': return 1e+9;
		case 'M': return 1e+6;
		case 'K': return 1e+3;
		case 'H': return 1e+2;
		case 'D': return 1e+1;
		case ' ': return 1e0;
		case 'd': return 1e-1;
		case 'c': return 1e-2;
		case 'm': return 1e-3;
		case 'u': return 1e-6;
		case 'n': return 1e-9;
		case 'p': return 1e-12;
		case 'f': return 1e-15;
		case 'a': return 1e-18;
		case 'z': return 1e-21;
		case 'y': return 1e-24;
		default: return 1e0;
	}
}

bool unit::isValidPrefix(char prefix)
{
	return scale(prefix) != 0 || prefix == ' ';
}
