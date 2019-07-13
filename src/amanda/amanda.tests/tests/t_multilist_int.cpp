#include <suit/assert.h>

#include <exceptions.h>
#include <structures/specialized/vmultilist.h>

#include <string>

struct State
{
	int*** mlist = nullptr;
	unsigned int N = 0, M = 0;

	State() { }
	State(int*** state, unsigned int N, unsigned int M) { set(state, N, M); }

	State& set(int*** mlist, unsigned int N, unsigned int M)
	{
		this->mlist = mlist;
		this->N = N;
		this->M = M;
		return *this;
	}

	void areEqual(vmultilist<int>& vml, int time)
	{
		static auto msg = [](int time, int k, int errcode) -> const char*
		{
			static std::string message = "[Axx] State is not correct; errcode: xx:x";
			message[2] = '0' + k / 10;
			message[3] = '0' + k % 10;

			message[message.size() - 4] = '0' + time / 10;
			message[message.size() - 3] = '0' + time % 10;

			message[message.size() - 1] = '0' + errcode;

			return message.c_str();
		};
		static int k = 0;

		assert::areEqual(vml.size(), N, msg(time, k++, 1));

		auto i = vml.begin();
		auto ise = vml.end();
		for (unsigned int ci = 0; i != ise; ++i, ++ci)
		{
			int tag = (int)mlist[ci][0];
			int size = (int)mlist[ci][1];
			vlist<int>* sublist = (*i)->e<0>();

			assert::areEqual((*i)->e<1>(), tag, msg(time, k++, 2));
			assert::areEqual(sublist->size(), size, msg(time, k++, 3));

			if (sublist->size() > M - 2)
			{
				throw AssertException("Test could not be executed due to invalid memory *size* [@ int areEqual(vmultilist<int>& vml, int time, int k = 0)]");
			}

			auto j = sublist->begin();
			auto jse = sublist->end();
			for (unsigned int cj = 2; j != jse; ++j, ++cj)
			{
				if (cj >= M)
				{
					// it shouldn't be possible for this exception to be thrown due to the previous size check (AssertException above)
					throw AssertException("Test could not be executed due to invalid memory *indexing* [@ int areEqual(vmultilist<int>& vml, int time, int k = 0)]");
				}

				assert::areEqual(*j, mlist[ci][cj], msg(time, k++, 4));
			}
		}
	}
};

void _vmultilist_int()
{
	vmultilist<int> vml;

	State state;
	int k = 0;

	int n1 = 100;
	int n2 = 200;
	int n3 = 300;

	vml.tick();
	vml.tick();

	vml.insert(&n1, 3);
	vml.insert(&n2, 5);
	vml.insert(&n3, 4);
	vml.insert(&n2, 4);

	int*** s = new int**[3];
	state.set(s, 3, 7);

	/* State [+2 tick(s)]:
	 *  [5] -> n1
	 *  [6] -> n3 -> n2
	 *  [7] -> n2
	 */
	state.N = 3;
	s[0] = new int*[7] { (int*)5, (int*)1, &n1, nullptr, nullptr, nullptr, nullptr };
	s[1] = new int*[7] { (int*)6, (int*)2, &n3, &n2, nullptr, nullptr, nullptr };
	s[2] = new int*[7] { (int*)7, (int*)1, &n2, nullptr, nullptr, nullptr, nullptr };

	state.areEqual(vml, 1);

	vml.tick();
	vml.tick();
	assert::areEqual(vml.take(), nullptr, "[A0] The take() method returned something other than nullptr");

	/* State [+4 tick(s)]:
	 *  [5] -> n1
	 *  [6] -> n3 -> n2
	 *  [7] -> n2
	 */
	state.N = 3;

	state.areEqual(vml, 2);

	vml.tick();

	/* State [+5 tick(s)]:
	 *  [5] -> n1
	 *  [6] -> n3 -> n2
	 *  [7] -> n2
	 */

	state.areEqual(vml, 3);

	vml.tick();

	/* State [+6 tick(s)]:
	 *  [5] -> n1
	 *  [6] -> n3 -> n2
	 *  [7] -> n2
	 */

	state.areEqual(vml, 4);

	assert::areEqual(vml.take(), &n1, "[A1] The take() method returned something other than n1");
	assert::areEqual(vml.take(), &n3, "[A2] The take() method returned something other than n3");

	/* State [+6 tick(s)]:
	 *  [6] -> n2
	 *  [7] -> n2
	 */
	state.N = 2;
	delete[] s[0]; s[0] = new int*[7] { (int*)6, (int*)1, &n2, nullptr, nullptr, nullptr, nullptr };
	delete[] s[1]; s[1] = new int*[7] { (int*)7, (int*)1, &n2, nullptr, nullptr, nullptr, nullptr };
	delete[] s[2]; s[2] = nullptr;

	state.areEqual(vml, 5);

	vml.insert(&n1, 0);
	vml.insert(&n1, 1);
	vml.insert(&n3, 4);
	vml.insert(&n1, 4);

	/* State [+6 tick(s)]:
	 *  [6] -> n2
	 *  [7] -> n2 -> n1
	 *  [10] -> n3 -> n1
	 */
	state.N = 3;
	delete[] s[0]; s[0] = new int*[7] { (int*)6, (int*)1, &n2, nullptr, nullptr, nullptr, nullptr };
	delete[] s[1]; s[1] = new int*[7] { (int*)7, (int*)2, &n2, &n1, nullptr, nullptr, nullptr };
	delete[] s[2]; s[2] = new int*[7] { (int*)10, (int*)2, &n3, &n1, nullptr, nullptr, nullptr };

	state.areEqual(vml, 6);

	assert::areEqual(vml.take(), &n2, "[A3] The take() method returned something other than n2");
	assert::areEqual(vml.take(), nullptr, "[A4] The take() method returned something other than nullptr");

	/* State [+6 tick(s)]:
	 *  [7] -> n2 -> n1
	 *  [10] -> n3 -> n1
	 */
	state.N = 2;
	delete[] s[0]; s[0] = new int*[7] { (int*)7, (int*)2, &n2, &n1, nullptr, nullptr, nullptr };
	delete[] s[1]; s[1] = new int*[7] { (int*)10, (int*)2, &n3, &n1, nullptr, nullptr, nullptr };
	delete[] s[2]; s[2] = nullptr;

	state.areEqual(vml, 7);

	vml.insert(&n2, 8);
	vml.tick();
	vml.tick();
	vml.tick();
	assert::areEqual(vml.take(), &n2, "[A5] The take() method returned something other than n2");

	/* State [+9 tick(s)]:
	 *  [7] -> n1
	 *  [10] -> n3 -> n1
	 *  [14] -> n2
	 */
	state.N = 3;
	delete[] s[0]; s[0] = new int*[7] { (int*)7, (int*)1, &n1, nullptr, nullptr, nullptr, nullptr };
	delete[] s[1]; s[1] = new int*[7] { (int*)10, (int*)2, &n3, &n1, nullptr, nullptr, nullptr };
	delete[] s[2]; s[2] = new int*[7] { (int*)14, (int*)1, &n2, nullptr, nullptr, nullptr, nullptr };

	state.areEqual(vml, 8);

	vml.tick();
	vml.tick();
	vml.tick();
	assert::areEqual(vml.take(), &n1, "[A6] The take() method returned something other than n1");

	/* State [+12 tick(s)]:
	 *  [10] -> n3 -> n1
	 *  [14] -> n2
	 */
	state.N = 2;
	delete[] s[0]; s[0] = new int*[7] { (int*)10, (int*)2, &n3, &n1, nullptr, nullptr, nullptr };
	delete[] s[1]; s[1] = new int*[7] { (int*)14, (int*)1, &n2, nullptr, nullptr, nullptr, nullptr };
	delete[] s[2]; s[2] = nullptr;

	state.areEqual(vml, 9);

	vml.insert(&n1, 2);
	vml.insert(&n3, 3);

	/* State [+12 tick(s)]:
	 *  [10] -> n3 -> n1
	 *  [14] -> n2 -> n1
	 *  [15] -> n3
	 */
	state.N = 3;
	delete[] s[0]; s[0] = new int*[7] { (int*)10, (int*)2, &n3, &n1, nullptr, nullptr, nullptr };
	delete[] s[1]; s[1] = new int*[7] { (int*)14, (int*)2, &n2, &n1, nullptr, nullptr, nullptr };
	delete[] s[2]; s[2] = new int*[7] { (int*)15, (int*)1, &n3, nullptr, nullptr, nullptr, nullptr };

	state.areEqual(vml, 10);

	assert::areEqual(vml.take(), &n3, "[A7] The take() method returned something other than n3");

	/* State [+12 tick(s)]:
	 *  [10] -> n1
	 *  [14] -> n2 -> n1
	 *  [15] -> n3
	 */
	state.N = 3;
	delete[] s[0]; s[0] = new int*[7] { (int*)10, (int*)1, &n1, nullptr, nullptr, nullptr, nullptr };
	delete[] s[1]; s[1] = new int*[7] { (int*)14, (int*)2, &n2, &n1, nullptr, nullptr, nullptr };
	delete[] s[2]; s[2] = new int*[7] { (int*)15, (int*)1, &n3, nullptr, nullptr, nullptr, nullptr };

	state.areEqual(vml, 11);

	vml.tick();
	vml.tick();
	vml.tick();
	vml.tick();
	vml.tick();
	assert::areEqual(vml.take(), &n1, "[A8] The take() method returned something other than n1");
	assert::areEqual(vml.take(), &n2, "[A9] The take() method returned something other than n2");
	assert::areEqual(vml.take(), &n1, "[A10] The take() method returned something other than n1");
	assert::areEqual(vml.take(), &n3, "[A11] The take() method returned something other than n3");
	assert::areEqual(vml.take(), nullptr, "[A12] The take() method returned something other than nullptr");

	/* State:
	 */
	state.N = 0;
	delete[] s[0]; s[0] = nullptr;
	delete[] s[1]; s[1] = nullptr;
	delete[] s[2]; s[2] = nullptr;

	state.areEqual(vml, 12);

	delete[] s[0];
	delete[] s[1];
	delete[] s[2];
	delete[] s;
}
