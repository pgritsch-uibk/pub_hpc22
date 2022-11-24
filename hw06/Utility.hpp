#pragma once

namespace Utility {
	template <typename T>
	T clamp(T value, T min, T max) {
		if (value < min) {
			return min;
		} else if (value > max) {
			return max;
		} else {
			return value;
		}
	}

    template <typename T>
    T map(T value, T a, T b, T c, T d) {
		return c + (value - a) * (d - c) / (b - a);
	}
}