#include <suit/assert.h>

#include <exceptions.h>
#include <structures/bitvector.h>

void _bitvector()
{
	bitvector<32> vector;
	std::string message;

	message = "[Axx] Bit in vector not reset";
	for (unsigned int i = 0; i < 32; ++i)
	{
		message[2] = '0' + i / 10;
		message[3] = '1' + i % 10;
		assert::areEqual(vector.isset(i), false, message.c_str());
	}

	bool values[32] = { };

	values[4] = true;
	values[7] = true;
	values[13] = true;
	values[15] = true;
	values[16] = true;
	values[17] = true;
	values[21] = true;
	values[23] = true;
	values[25] = true;
	values[30] = true;

	for (unsigned int i = 0; i < 32; ++i)
	{
		vector.value(i, values[i]);
	}

	message = "[Axx] Bit in vector not correct";
	for (unsigned int i = 0; i < 32; ++i)
	{
		message[2] = '0' + (i + 33) / 10;
		message[3] = '0' + (i + 33) % 10;
		assert::areEqual(vector.isset(i), values[i], message.c_str());
	}
}
