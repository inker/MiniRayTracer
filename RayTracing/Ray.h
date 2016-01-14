#pragma once
#include "Vec3.h"

template<typename T>
struct Ray {
	Vec3<T> origin, dir;
	Ray() : origin(Vec3<T>(), Vec3<T>()) {}
	Ray(Vec3<T> origin, Vec3<T> dir) : origin(origin), dir(dir) {}
};