#pragma once
#include <math.h>
#include "Texture.h"
#include "util.h"

class Sphere {
	template <size_t U> friend class RayTracer;

	float radius2; // radius^2
	Texture* texture = nullptr; // if null, falls back to uniform grey

public:
	
	Vec3f center;
	float radius;
	Vec3f surfaceColor, emissionColor; // surface color and light emission color
	float transparency, reflection; // surface transparency and reflectivity 

	// plain color
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

	// textured
	Sphere(
		const Vec3f &center,
		const float &radius,
		const Texture& texture,
		const float &reflection = 0,
		const float &transparency = 0,
		const Vec3f &emissionColor = 0) :
		center(center),
		radius(radius),
		radius2(radius * radius),
		texture((Texture*)&texture),
		surfaceColor({ 0.5f, 0.5f, 0.5f }),
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