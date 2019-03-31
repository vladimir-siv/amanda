#include <suit/assert.h>

#include <exceptions.h>
#include <structures/queue.h>

void _queue_int()
{
	queue<int> que1(6);

	que1.enqueue(3);
	que1.enqueue(4);
	que1.enqueue(1);
	que1.enqueue(7);

	// que1: { 3f, 4, 1, 7, xl, x }
	assert::areEqual(que1.size(), 4, "[A1] Size not equal to 4");
	assert::areEqual(que1.peek(), 3, "[A2] Peek not equal to 3");
	assert::areEqual(que1.dequeue(), 3, "[A3] Dequeue not equal to 3");
	assert::areEqual(que1.peek(), 4, "[A4] Peek not equal to 4");
	assert::areEqual(que1.dequeue(), 4, "[A5] Dequeue not equal to 4");
	assert::areEqual(que1.peek(), 1, "[A6] Peek not equal to 1");
	assert::areEqual(que1.dequeue(), 1, "[A7] Dequeue not equal to 1");
	assert::areEqual(que1.peek(), 7, "[A8] Peek not equal to 7");
	assert::areEqual(que1.dequeue(), 7, "[A9] Dequeue not equal to 7");
	assert::areEqual(que1.size(), 0, "[A10] Size not equal to 0");

	que1.enqueue(9);
	que1.enqueue(2);
	que1.enqueue(1); que1.dequeue();
	que1.enqueue(8);
	que1.enqueue(3);
	que1.enqueue(5);

	queue<int> que2 = que1;
	que2.clear();
	que2[5] = que1[1];

	// que1: { 1, 8, 3, 5, 9l, 2f }
	assert::areEqual(que1.size(), 5, "[A11] Size not equal to 5");
	assert::areEqual(que1.peek(), 2, "[A12] Peek not equal to 2");
	assert::areEqual(que1.dequeue(), 2, "[A13] Dequeue not equal to 2");
	assert::areEqual(que1.dequeue(), 1, "[A14] Dequeue not equal to 1");
	assert::areEqual(que1.peek(), 8, "[A15] Peek not equal to 8");
	assert::areEqual(que1.size(), 3, "[A16] Size not equal to 3");

	// que2: { 1fl, 8, 3, 5, 9, 8 }
	assert::areEqual(que2.capacity(), 6, "[A17] Capacity not equal to 6");
	assert::areEqual(que2.size(), 0, "[A18] Size not equal to 0");
	assert::areEqual(que2[0], 1, "[A19] Element not equal to 1");
	assert::areEqual(que2[1], 8, "[A20] Element not equal to 8");
	assert::areEqual(que2[2], 3, "[A21] Element not equal to 3");
	assert::areEqual(que2[3], 5, "[A22] Element not equal to 5");
	assert::areEqual(que2[4], 9, "[A23] Element not equal to 9");
	assert::areEqual(que2[5], 8, "[A24] Element not equal to 8");

	queue<int> que3 = que1;

	// que3: { 1, 8f, 3, 5, 9l, 2 }
	assert::areEqual(que3.size(), 3, "[A25] Size not equal to 3");
	assert::areEqual(que3.peek(), 8, "[A26] Peek not equal to 8");
	assert::areEqual(que3[0], 1, "[A27] Element not equal to 1");
	assert::areEqual(que3[1], 8, "[A28] Element not equal to 8");
	assert::areEqual(que3[2], 3, "[A29] Element not equal to 3");
	assert::areEqual(que3[3], 5, "[A30] Element not equal to 5");
	assert::areEqual(que3[4], 9, "[A31] Element not equal to 9");

	que3[5] = 7;
	que3[1] = 4;
	que3[3] = 5;
	que3.enqueue(1);

	// que3: { 1, 4f, 3, 5, 1, 7l }
	assert::areEqual(que3.size(), 4, "[A32] Size not equal to 4");
	assert::areEqual(que3.dequeue(), 4, "[A33] Dequeue not equal to 4");
	assert::areEqual(que3.peek(), 3, "[A34] Peek not equal to 3");
	assert::areEqual(que3.dequeue(), 3, "[A35] Dequeue not equal to 3");
	assert::areEqual(que3[0], 1, "[A36] Element not equal to 1");
	assert::areEqual(que3[1], 4, "[A37] Element not equal to 4");
	assert::areEqual(que3[2], 3, "[A38] Element not equal to 3");
	assert::areEqual(que3[3], 5, "[A39] Element not equal to 5");
	assert::areEqual(que3[4], 1, "[A40] Element not equal to 1");
	assert::areEqual(que3[5], 7, "[A41] Element not equal to 7");
	assert::areEqual(que3.capacity(), 6, "[A42] Capacity not equal to 6");
	assert::areEqual(que3.size(), 2, "[A43] Size not equal to 2");
}
