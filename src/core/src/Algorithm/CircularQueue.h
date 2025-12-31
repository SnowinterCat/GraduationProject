#pragma once
#include <gp/config.hpp>

#include <vector>

template<class T>
class CircularQueue {
	std::vector<T> val;
	unsigned int l, r, Lens;
public:
	CircularQueue() :l(0), r(0), Lens(0) {}
	CircularQueue(int num) :l(0), r(0), Lens(num) {
		val.resize(num);
	}
	~CircularQueue() = default;

	T& operator[](unsigned int i);
	const T& operator[](unsigned int i)const;
	bool resize(int num);

	T front();
	T back();
	bool push(const T& a);
	bool empty();
	bool pop();
	unsigned int size();
};

template<class T>
inline T& CircularQueue<T>::operator[](unsigned int i)
{
	if (l + i >= Lens)
		return val[l + i - Lens];
	return val[l + i];
}

template<class T>
inline const T& CircularQueue<T>::operator[](unsigned int i) const
{
	if (l + i >= Lens)
		return val[l + i - Lens];
	return val[l + i];
}

template<class T>
inline bool CircularQueue<T>::resize(int num)
{
	l = r = 0;
	Lens = num;
	val.resize(Lens);
	return true;
}

template<class T>
inline T CircularQueue<T>::front()
{
	return val[l];
}

template<class T>
inline T CircularQueue<T>::back()
{
	if (r)
		return val[r - 1];
	return val[Lens - 1];
}

template<class T>
inline bool CircularQueue<T>::push(const T& a)
{
	val[r] = a;
	if (++r == Lens)
		r = 0;
	if (r == l)
		return false;
	return true;
}

template<class T>
inline bool CircularQueue<T>::empty()
{
	return r == l;
}

template<class T>
inline bool CircularQueue<T>::pop()
{
	if (l == r)
		return false;
	if (++l == Lens)
		l = 0;
	return true;
}

template<class T>
inline unsigned int CircularQueue<T>::size()
{
	if (r < l)
		return r - l + Lens;
	return r - l;
}