#pragma once
#include <utility>
#include <functional>
#include <math.h>
#include "Vec3.h"
#include "Ray.h"

typedef Vec3<float> Vec3f;
typedef Ray<float> Rayf;

class Sphere {
	float radius2; // radius^2 
public:
	Vec3f center;                           /// position of the sphere 
	float radius;							 /// sphere radius 
	Vec3f surfaceColor, emissionColor;      /// surface color and emission (light) 
	float transparency, reflection;         /// surface transparency and reflectivity 
	Sphere(
		const Vec3f &center,
		const float &radius,
		const Vec3f &surfaceColor,
		const float &reflection = 0,
		const float &transparency = 0,
		const Vec3f &emissionColor = 0) :
		center(center),
		radius(radius),
		radius2(radius * radius),
		surfaceColor(surfaceColor),
		emissionColor(emissionColor),
		transparency(transparency),
		reflection(reflection) {
	}

	float intersect(const Rayf &ray) const {
		Vec3f l = center - ray.origin;
		float tca = dot(l, ray.dir);
		if (tca < 0) {
			return -1;
		}
		float d2 = dot(l, l) - tca * tca;
		if (d2 > radius2) {
			return -1;
		}
		float thc = sqrt(radius2 - d2);
		float t = tca - thc;
		return t < 0 ? tca + thc : t;
	}
};