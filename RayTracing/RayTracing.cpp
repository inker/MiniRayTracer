#include "stdafx.h"

#include <vector> 
#include "Vec3.h"
#include "Sphere.h"
#include "RayTracer.h"

typedef Vec3<float> Vec3f;
typedef Ray<float> Rayf;

int main(int argc, char **argv) {
	//srand48(13);
	std::vector<Sphere> spheres;
	// position, radius, surface color, reflectivity, transparency, emission color
	spheres.push_back({ { 0.0f, -1004.0f, -20.0f }, 1000.0f, { 0.20f, 0.20f, 0.20f }, 0, 0.0f });
	spheres.push_back({ { 0.0f, 0.0f, -20.0f }, 4.0f, { 1.00f, 0.32f, 0.36f }, 1, 0.5f });
	spheres.push_back({ { 5.0f, -1.0f, -15.0f }, 2.0f, { 0.90f, 0.76f, 0.46f }, 1, 0.0f });
	spheres.push_back({ { 5.0f, 0.0f, -25.0f }, 3.0f, { 0.65f, 0.77f, 0.97f }, 1, 0.0f });
	spheres.push_back({ { -5.5f, 0.0f, -15.0f }, 3.0f, { 0.90f, 0.90f, 0.90f }, 1, 0.0f });
	// light
	spheres.push_back({ { 0.0, 20, -30 }, 3, { 0.00f, 0.00f, 0.00f }, 0, 0.0f, { 3, 3, 3 } });
	RayTracer<5> rt(spheres);
	size_t w = 640, h = 480;
	rt.render(w, h, "./rt-no.ppm", 0);
	rt.render(w, h, "./rt-ms.ppm", 1);
	rt.render(w, h, "./rt-ss.ppm", 2);
	return 0;
}
