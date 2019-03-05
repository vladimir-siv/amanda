#pragma once

class EventArgs
{
	private: static EventArgs _empty;
	public: static EventArgs& empty() { return _empty; };
};
