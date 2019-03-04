#include <suit/assert.h>

#include "../../amanda.os/exceptions.h"
#include "../../amanda.os/structures/vector.h"

void _vector_int()
{
	Vector<int> vec1(6);
	
	vec1.push(3);
	vec1.push(4);
	vec1.push(1);
	vec1.push(7);

	// vec1: { 3, 4, 1, 7, x*, x }
	assert::areEqual(vec1.size(), 4, "[A1] Size not equal to 4");
	assert::areEqual(vec1.peek(), 7, "[A2] Peek not equal to 7");
	assert::areEqual(vec1.pop(), 7, "[A3] Pop not equal to 7");
	assert::areEqual(vec1.peek(), 1, "[A4] Peek not equal to 1");
	assert::areEqual(vec1.pop(), 1, "[A5] Pop not equal to 1");
	assert::areEqual(vec1.peek(), 4, "[A6] Peek not equal to 4");
	assert::areEqual(vec1.pop(), 4, "[A7] Pop not equal to 4");
	assert::areEqual(vec1.peek(), 3, "[A8] Peek not equal to 3");
	assert::areEqual(vec1.pop(), 3, "[A9] Pop not equal to 3");
	assert::areEqual(vec1.size(), 0, "[A10] Size not equal to 0");

	vec1.push(9);
	vec1.push(2);
	vec1.push(1); vec1.pop();
	vec1.push(8);
	vec1.push(3);
	vec1.push(5);

	Vector<int> vec2 = vec1;
	vec2.clear();
	vec2[5] = vec1[1];

	// vec1: { 9, 2, 8, 3, 5, x* }
	assert::areEqual(vec1.size(), 5, "[A11] Size not equal to 5");
	assert::areEqual(vec1.peek(), 5, "[A12] Peek not equal to 5");
	assert::areEqual(vec1.pop(), 5, "[A13] Pop not equal to 5");
	assert::areEqual(vec1.pop(), 3, "[A14] Pop not equal to 3");
	assert::areEqual(vec1.peek(), 8, "[A15] Peek not equal to 8");
	assert::areEqual(vec1.size(), 3, "[A16] Size not equal to 3");

	// vec2: { 9, 2, 8, 3, 5, 2 }
	assert::areEqual(vec2.capacity(), 6, "[A17] Capacity not equal to 6");
	assert::areEqual(vec2.size(), 0, "[A18] Size not equal to 0");
	assert::areEqual(vec2[0], 9, "[A19] Element not equal to 9");
	assert::areEqual(vec2[1], 2, "[A20] Element not equal to 2");
	assert::areEqual(vec2[2], 8, "[A21] Element not equal to 8");
	assert::areEqual(vec2[3], 3, "[A22] Element not equal to 3");
	assert::areEqual(vec2[4], 5, "[A23] Element not equal to 5");
	assert::areEqual(vec2[5], 2, "[A24] Element not equal to 2");

	Vector<int> vec3 = vec1;
	
	// vec3: { 9, 2, 8, 3*, 5, x }
	assert::areEqual(vec3.size(), 3, "[A25] Size not equal to 3");
	assert::areEqual(vec3.peek(), 8, "[A26] Peek not equal to 8");
	assert::areEqual(vec3[0], 9, "[A27] Element not equal to 9");
	assert::areEqual(vec3[1], 2, "[A28] Element not equal to 2");
	assert::areEqual(vec3[2], 8, "[A29] Element not equal to 8");
	assert::areEqual(vec3[3], 3, "[A30] Element not equal to 3");
	assert::areEqual(vec3[4], 5, "[A31] Element not equal to 5");

	vec3[5] = 7;
	vec3[1] = 4;
	vec3[3] = 5;
	vec3.push(1);

	// vec3: { 9, 4, 8, 1, 5*, 7 }
	assert::areEqual(vec3.size(), 4, "[A32] Size not equal to 4");
	assert::areEqual(vec3.pop(), 1, "[A33] Pop not equal to 1");
	assert::areEqual(vec3.peek(), 8, "[A34] Peek not equal to 8");
	assert::areEqual(vec3.pop(), 8, "[A35] Pop not equal to 8");
	assert::areEqual(vec3[0], 9, "[A36] Element not equal to 9");
	assert::areEqual(vec3[1], 4, "[A37] Element not equal to 4");
	assert::areEqual(vec3[2], 8, "[A38] Element not equal to 8");
	assert::areEqual(vec3[3], 1, "[A39] Element not equal to 1");
	assert::areEqual(vec3[4], 5, "[A40] Element not equal to 5");
	assert::areEqual(vec3[5], 7, "[A41] Element not equal to 7");
	assert::areEqual(vec3.capacity(), 6, "[A42] Capacity not equal to 6");
	assert::areEqual(vec3.size(), 2, "[A43] Size not equal to 2");
}
