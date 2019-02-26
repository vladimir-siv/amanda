#pragma once

template<typename T>
class Scheduler final
{
	private: static Scheduler<T>* instance;
	public: static Scheduler<T>* I() { return instance; }
	
	private: T** items;
	private: unsigned int capacity, size;
	private: unsigned int head, tail;
	
	private: explicit Scheduler(unsigned int capacity) : items(new T*[capacity]), capacity(capacity), size(0), head(0), tail(0) { }
	public: ~Scheduler() { delete[] this->items; this->items = nullptr; this->capacity = this->size = 0; this->head = this->tail = 0; }
	public: Scheduler(const Scheduler& scheduler) = delete;
	public: Scheduler(Scheduler&& scheduler) = delete;
	public: Scheduler& operator=(const Scheduler& scheduler) = delete;
	public: Scheduler& operator=(Scheduler&& scheduler) = delete;
	
	public: void put(T* item)
	{
		if (this->size == this->capacity) return; // throw SchedulerFullException
		this->items[this->tail] = item;
		if (++this->tail == this->capacity) this->tail = 0;
		++this->size;
		return;
	}
	
	public: T* get()
	{
		if (this->size == 0) return nullptr;
		T* item = this->items[this->head];
		if (++this->head == this->capacity) this->head = 0;
		--this->size;
		return item;
	}
};
