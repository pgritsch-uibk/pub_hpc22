#pragma once

#include <cmath>
#include <random>

#if __has_include(<raylib-cpp.hpp>)
#include <raylib-cpp.hpp>
#endif

struct Vector3D {
  private:
  public:
	float x;
	float y;
	float z;

	Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3D() : x(0), y(0), z(0) {}

	// TODO: include Vector3 for when raylib is included

#if __has_include(<raylib-cpp.hpp>)
	// convert from Vector3 to Position constexpr
	Vector3D(const raylib::Vector3& vector) : x(vector.x), y(vector.y), z(vector.z) {}

	// convert from Position to Vector3
	operator raylib::Vector3() const { return { x, y, z }; }

//	 Vector3D& operator=(const raylib::Vector3& position) {
//	 	x = position.x;
//	 	y = position.y;
//	 	z = position.z;
//	 	return *this;
//	 }
#endif


	Vector3D& operator+=(const Vector3D& position) {
		x += position.x;
		y += position.y;
		z += position.z;
		return *this;
	}

	Vector3D& operator-=(const Vector3D& position) {
		x -= position.x;
		y -= position.y;
		z -= position.z;
		return *this;
	}

	Vector3D& operator*=(const Vector3D& position) {
		x *= position.x;
		y *= position.y;
		z *= position.z;
		return *this;
	}

	Vector3D& operator/=(const Vector3D& position) {
		x /= position.x;
		y /= position.y;
		z /= position.z;
		return *this;
	}

	Vector3D& operator+=(float value) {
		x += value;
		y += value;
		z += value;
		return *this;
	}

	Vector3D& operator-=(float value) {
		x -= value;
		y -= value;
		z -= value;
		return *this;
	}

	Vector3D& operator*=(float value) {
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	Vector3D& operator/=(float value) {
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}

	Vector3D operator+(const Vector3D& position) const {
		return {x + position.x, y + position.y, z + position.z};
	}

	Vector3D operator-(const Vector3D& position) const {
		return {x - position.x, y - position.y, z - position.z};
	}

	Vector3D operator*(const Vector3D& position) const {
		return {x * position.x, y * position.y, z * position.z};
	}

	Vector3D operator/(const Vector3D& position) const {
		return {x / position.x, y / position.y, z / position.z};
	}

	Vector3D operator+(float value) const { return {x + value, y + value, z + value}; }

	Vector3D operator-(float value) const { return {x - value, y - value, z - value}; }

	Vector3D operator*(float value) const { return {x * value, y * value, z * value}; }

	Vector3D operator/(float value) const { return {x / value, y / value, z / value}; }

	bool operator==(const Vector3D& position) const {
		return x == position.x && y == position.y && z == position.z;
	}

	bool operator!=(const Vector3D& position) const {
		return x != position.x || y != position.y || z != position.z;
	}

	bool operator<(const Vector3D& position) const {
		return x < position.x && y < position.y && z < position.z;
	}

	bool operator>(const Vector3D& position) const {
		return x > position.x && y > position.y && z > position.z;
	}

	bool operator<=(const Vector3D& position) const {
		return x <= position.x && y <= position.y && z <= position.z;
	}

	bool operator>=(const Vector3D& position) const {
		return x >= position.x && y >= position.y && z >= position.z;
	}

	Vector3D operator-() const { return {-x, -y, -z}; }

	Vector3D operator+() const { return {+x, +y, +z}; }

//	Vector3D operator!() const { return {!x, !y, !z}; }

	Vector3D operator++() { return {++x, ++y, ++z}; }

	Vector3D operator--() { return {--x, --y, --z}; }

	Vector3D operator++(int) { return {x++, y++, z++}; }

	Vector3D operator--(int) { return {x--, y--, z--}; }

	// calculate length
	float length() const { return std::sqrt((x * x) + (y * y) + (z * z)); }

	// calculate distance
	float distance(const Vector3D& position) const {
		return std::sqrt((x - position.x) * (x - position.x) + (y - position.y) * (y - position.y) +
		                 (z - position.z) * (z - position.z));
	}

	// calculate normalized vector
	Vector3D normalize() const {
		float length = this->length();
		return {x / length, y / length, z / length};
	}

	// calculate direction vector
	Vector3D direction(const Vector3D& position) const {
		float distance = this->distance(position);
		return {(x - position.x) / distance, (y - position.y) / distance, (z - position.z) / distance};
	}

	// generate random vector between min and max
	static Vector3D random(float min, float max) {
		std::default_random_engine rng(std::random_device{}());
		std::uniform_real_distribution<float> dist(min, max);

		return {dist(rng), dist(rng), dist(rng)};
	}
};
