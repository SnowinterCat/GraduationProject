#pragma once
#include <gp/config.hpp>

#include <vector>

#define MinHeap 0
#define MaxHeap 1
template <class T, bool Flag>
class Heap {
	std::vector<T> heap;
	int CurrentSize, MaxSize;
	void PushMin();
	void PushMax();
	void PopMin();
	void PopMax();
public:
	Heap() :CurrentSize(0), MaxSize(0) {
		heap.resize(1);
	}
	~Heap() = default;
	bool empty() {
		return !CurrentSize;
	}
	T top() {
		return heap[1];
	}
	void pop();
	void push(const T& a);
};

template<class T, bool Flag>
inline void Heap<T, Flag>::PushMin()
{
	int i = CurrentSize;
	while (i != 1) {
		if (heap[0] < heap[i >> 1])
			heap[i] = heap[i >> 1];
		else break;
		i = i >> 1;
	}
	heap[i] = heap[0];
}

template<class T, bool Flag>
inline void Heap<T, Flag>::PushMax()
{
	int i = CurrentSize;
	while (i != 1) {
		if (heap[0] > heap[i >> 1])
			heap[i] = heap[i >> 1];
		else break;
		i = i >> 1;
	}
	heap[i] = heap[0];
}

template<class T, bool Flag>
inline void Heap<T, Flag>::PopMin()
{
	int i = 2, j = 3;
	while (i <= CurrentSize) {
		j = i ^ 1;
		if (j <= CurrentSize && heap[j] < heap[i] && heap[j] < heap[0])
			heap[i >> 1] = heap[j], i = j << 1;
		else if (heap[i] < heap[0])
			heap[i >> 1] = heap[i], i = i << 1;
		else break;
	}
	heap[i >> 1] = heap[0];
}

template<class T, bool Flag>
inline void Heap<T, Flag>::PopMax()
{
	int i = 2, j = 3;
	while (i <= CurrentSize) {
		j = i ^ 1;
		if (j <= CurrentSize && heap[j] > heap[i] && heap[j] > heap[0])
			heap[i >> 1] = heap[j], i = j << 1;
		else if (heap[i] > heap[0])
			heap[i >> 1] = heap[i], i = i << 1;
		else break;
	}
	heap[i >> 1] = heap[0];
}

template<class T, bool Flag>
inline void Heap<T, Flag>::pop()
{
	heap[0] = heap[1] = heap[CurrentSize--];
	if (Flag == MinHeap)
		PopMin();
	else PopMax();
}

template<class T, bool Flag>
inline void Heap<T, Flag>::push(const T& a)
{
	if (CurrentSize < MaxSize)
		heap[++CurrentSize] = a;
	else {
		heap.push_back(a);
		++CurrentSize;
		++MaxSize;
	}
	heap[0] = heap[CurrentSize];
	if (Flag == MinHeap)
		PushMin();
	else PushMax();
}