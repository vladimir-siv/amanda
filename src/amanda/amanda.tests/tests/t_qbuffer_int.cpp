#include <suit/assert.h>

#include <structures/specialized/qbuffer.h>

void _qbuffer_int()
{
	QBuffer<int> qbuffer1(4);

	qbuffer1.push(3);
	qbuffer1.push(4);
	
	// qbuffer1: { 3, 4 }
	assert::areEqual(qbuffer1.size(), 2, "[A1] Size is not equal to 2");
	assert::areEqual(qbuffer1.peek(), 4, "[A2] Peek not equal to 4");
	assert::areEqual(qbuffer1.pop(), 4, "[A3] Pop not equal to 4");

	qbuffer1.push(5);
	qbuffer1.push(7);
	qbuffer1.push(1);
	qbuffer1.push(9);
	qbuffer1.push(8);

	// qbuffer1: { 7, 1, 9, 8 }
	assert::areEqual(qbuffer1.size(), 4, "[A4] Size not equal to 4");
	assert::areEqual(qbuffer1.peek(), 8, "[A5] Peek not equal to 8");
	assert::areEqual(qbuffer1.pop(), 8, "[A6] Pop not equal to 8");
	assert::areEqual(qbuffer1.peek(), 9, "[A7] Peek not equal to 9");
	assert::areEqual(qbuffer1.pop(), 9, "[A8] Pop not equal to 9");
	assert::areEqual(qbuffer1.peek(), 1, "[A9] Peek not equal to 1");
	assert::areEqual(qbuffer1.pop(), 1, "[A10] Pop not equal to 1");
	assert::areEqual(qbuffer1.peek(), 7, "[A11] Peek not equal to 7");
	assert::areEqual(qbuffer1.pop(), 7, "[A12] Pop not equal to 7");
	assert::areEqual(qbuffer1.size(), 0, "[A13] Size not equal to 0");

	qbuffer1.push(3);
	qbuffer1.push(7);
	qbuffer1.push(5);
	qbuffer1.clear();

	// qbuffer1: { }
	assert::areEqual(qbuffer1.size(), 0, "[A14] Size not equal to 0");

	qbuffer1.push(0);
	qbuffer1.push(7); qbuffer1.pop();
	qbuffer1.push(1);
	qbuffer1.push(2);

	// qbuffer1: { 0, 1, 2 }
	assert::areEqual(qbuffer1.size(), 3, "[A15] Size not equal to 3");
	assert::areEqual(qbuffer1.peek(), 2, "[A16] Peek not equal to 2");
	assert::areEqual(qbuffer1.pop(), 2, "[A17] Pop not equal to 2");
	assert::areEqual(qbuffer1.pop(), 1, "[A18] Pop not equal to 1");
	assert::areEqual(qbuffer1.peek(), 0, "[A19] Peek not equal to 0");
	assert::areEqual(qbuffer1.pop(), 0, "[A20] Pop not equal to 0");
	assert::areEqual(qbuffer1.size(), 0, "[A21] Size not equal to 0");

	int n1 = 1;
	int n2 = 2;
	int n3 = 3;
	int n4 = 4;

	QBuffer<int*> qbuffer2(7);

	qbuffer2.push(&n4);
	qbuffer2.push(&n1);
	qbuffer2.push(&n3);
	qbuffer2.push(&n2);

	QBuffer<int*> qbuffer3 = qbuffer2;

	// qbuffer2: { 4, 1, 3, 2 }
	// qbuffer3: { 4, 1, 3, 2 }
	assert::areEqual(qbuffer2.size(), 4, "[A22] Size not equal to 4");
	assert::areEqual(qbuffer3.size(), 4, "[A23] Size not equal to 4");
	assert::areEqual(qbuffer2.peek(), qbuffer3.peek(), "[A24] Peeks are not equal");
	assert::areEqual(*qbuffer2.peek(), *qbuffer3.peek(), "[A25] Peeks are not equal");
	assert::areEqual(*qbuffer2.pop(), 2, "[A26] Pop not equal to 2");
	assert::areEqual(*qbuffer3.pop(), 2, "[A27] Pop not equal to 2");
	assert::areEqual(qbuffer2.peek(), qbuffer3.peek(), "[A28] Peeks are not equal");
	assert::areEqual(*qbuffer2.peek(), *qbuffer3.peek(), "[A29] Peeks are not equal");
	assert::areEqual(*qbuffer2.pop(), 3, "[A30] Pop not equal to 3");
	assert::areEqual(*qbuffer3.pop(), 3, "[A31] Pop not equal to 3");
	assert::areEqual(qbuffer2.peek(), qbuffer3.peek(), "[A32] Peeks are not equal");
	assert::areEqual(*qbuffer2.peek(), *qbuffer3.peek(), "[A33] Peeks are not equal");
	assert::areEqual(*qbuffer2.pop(), 1, "[A34] Pop not equal to 1");
	assert::areEqual(*qbuffer3.pop(), 1, "[A35] Pop not equal to 1");
	assert::areEqual(qbuffer2.peek(), qbuffer3.peek(), "[A36] Peeks are not equal");
	assert::areEqual(*qbuffer2.peek(), *qbuffer3.peek(), "[A37] Peeks are not equal");
	assert::areEqual(*qbuffer2.pop(), 4, "[A38] Pop not equal to 4");
	assert::areEqual(*qbuffer3.pop(), 4, "[A39] Pop not equal to 4");
	assert::areEqual(qbuffer2.size(), 0, "[A40] Size not equal to 0");
	assert::areEqual(qbuffer3.size(), 0, "[A41] Size not equal to 0");
}
