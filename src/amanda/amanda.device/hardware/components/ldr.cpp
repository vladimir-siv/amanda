#include "ldr.h"

#include <avr/pgmspace.h>

const PROGMEM float table[70][2]
{
	{	0.0f	,	0.0f	},
	{	0.02f	,	0.1f	},
	{	0.06f	,	0.3f	},
	{	0.11f	,	0.5f	},
	{	0.19f	,	1.0f	},
	{	0.33f	,	2.0f	},
	{	0.46f	,	3.0f	},
	{	0.57f	,	4.0f	},
	{	0.68f	,	5.0f	},
	{	0.77f	,	6.0f	},
	{	0.86f	,	7.0f	},
	{	0.95f	,	8.0f	},
	{	1.03f	,	9.0f	},
	{	1.1f	,	10.0f	},
	{	1.28f	,	12.5f	},
	{	1.43f	,	15.0f	},
	{	1.57f	,	17.5f	},
	{	1.7f	,	20.0f	},
	{	1.81f	,	22.5f	},
	{	1.92f	,	25.0f	},
	{	2.01f	,	27.5f	},
	{	2.1f	,	30.0f	},
	{	2.19f	,	32.5f	},
	{	2.27f	,	35.0f	},
	{	2.34f	,	37.5f	},
	{	2.41f	,	40.0f	},
	{	2.47f	,	42.5f	},
	{	2.54f	,	45.0f	},
	{	2.59f	,	47.5f	},
	{	2.65f	,	50.0f	},
	{	2.75f	,	55.0f	},
	{	2.84f	,	60.0f	},
	{	2.93f	,	65.0f	},
	{	3.0f	,	70.0f	},
	{	3.07f	,	75.0f	},
	{	3.14f	,	80.0f	},
	{	3.25f	,	90.0f	},
	{	3.36f	,	100.0f	},
	{	3.47f	,	112.5f	},
	{	3.56f	,	125.0f	},
	{	3.64f	,	137.5f	},
	{	3.72f	,	150.0f	},
	{	3.84f	,	175.0f	},
	{	3.94f	,	200.0f	},
	{	4.02f	,	225.0f	},
	{	4.09f	,	250.0f	},
	{	4.15f	,	275.0f	},
	{	4.2f	,	300.0f	},
	{	4.24f	,	325.0f	},
	{	4.28f	,	350.0f	},
	{	4.32f	,	375.0f	},
	{	4.35f	,	400.0f	},
	{	4.38f	,	425.0f	},
	{	4.41f	,	450.0f	},
	{	4.43f	,	475.0f	},
	{	4.45f	,	500.0f	},
	{	4.47f	,	525.0f	},
	{	4.49f	,	550.0f	},
	{	4.51f	,	575.0f	},
	{	4.52f	,	600.0f	},
	{	4.54f	,	625.0f	},
	{	4.55f	,	650.0f	},
	{	4.57f	,	675.0f	},
	{	4.58f	,	700.0f	},
	{	4.6f	,	750.0f	},
	{	4.62f	,	800.0f	},
	{	4.64f	,	850.0f	},
	{	4.65f	,	900.0f	},
	{	4.67f	,	950.0f	},
	{	4.68f	,	1000.0f	}
};
float interpolate(float value)
{
	if (value <= 0.0f) return 0.0f;
	if (value >= 4.68f) return 1000.0f;

	float next = 0.0f, prev = 0.0f;
	byte i = 0;

	// TODO: Improve with binary search
	for (i = 1; i < 70 && value > next; ++i)
	{
		prev = next;
		next = pgm_read_float(&table[i][0]);
	}

	i -= 2;

	if (prev == value) return pgm_read_float(&table[i][1]);
	if (next == value) return pgm_read_float(&table[i + 1][1]);

	float& x1 = prev;
	float& x2 = next;

	float y1 = pgm_read_float(&table[i][1]);
	float y2 = pgm_read_float(&table[i + 1][1]);

	float k = (y1 - y2) / (x1 - x2);
	float n = y2 - k * x2;

	return k * value + n;
}

AnalogValue LDR::read() const { return interpolate((float)AnalogSensor::read() / 204.6f); }
