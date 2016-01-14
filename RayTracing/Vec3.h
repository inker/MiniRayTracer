#pragma once

template<typename T>
struct Vec3 {
	T x, y, z;
	Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec3(T xx) : x(xx), y(xx), z(xx) {}
	Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	Vec3& normalize() {
		T nor2 = lengthSq();
		if (nor2 > 0) {
			T invNor = 1 / sqrt(nor2);
			x *= invNor, y *= invNor, z *= invNor;
		}
		return *this;
	}
	Vec3<T> operator * (const T f) const { return Vec3<T>(x * f, y * f, z * f); }
	Vec3<T> operator / (const T f) const { return Vec3<T>(x / f, y / f, z / f); }
	Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
	Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
	Vec3<T>& operator += (const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
	Vec3<T>& operator *= (const T f) { x *= f, y *= f, z *= f; return *this; }
	Vec3<T>& operator /= (const T f) { x /= f, y /= f, z /= f; return *this; }
	Vec3<T>& operator *= (const Vec3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
	Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }
	T lengthSq() const { return x * x + y * y + z * z; }
	T length() const { return sqrt(lengthSq()); }
	friend T dot(const Vec3<T> &v1, const Vec3<T> &v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}
	friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v) {
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};