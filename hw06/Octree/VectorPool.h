#ifndef HPC22_VECTORPOOL_H
#define HPC22_VECTORPOOL_H

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>

template <typename T> class VectorPool {

	std::vector<T> vector;

	int current{};

  public:
	VectorPool(int _size, T object) : vector(_size, object), current(0) {}

	T* getNext() {
		if(vector.size() < current + 1) {
			throw std::runtime_error("cannot return new element");
		}
		return &vector[current++];
	}

	void reset() { current = 0; }

	void for_each(std::function<void(T&)> function) {
		std::for_each(vector.begin(), vector.end(), [&](T& t) { function(t); });
	}
};

#endif // HPC22_VECTORPOOL_H
