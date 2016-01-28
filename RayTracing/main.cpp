#include "stdafx.h"

#include <string>
#include <vector> 
#include <iostream>
#include "Sphere.h"
#include "RayTracer.h"
#include "util.h"

int main(int argc, char **argv) {
	//srand48(13);
	Texture earth("textures/mercator.png");
	std::cout << earth.width() << " " << earth.height() << std::endl;
	std::vector<Sphere> spheres;
	
	// position, radius, surface color, reflectivity, transparency, emission color
	spheres.push_back({ { 0.0f, -1004.0f, -20.0f }, 1000.0f, { 0.20f, 0.20f, 0.20f }, 1, 0.0f });
	spheres.push_back({ { -2.0f, 0.0f, -25.0f }, 4.0f,{ 0.90f, 0.90f, 0.90f } , 1, 0.5f });
	spheres.push_back({ { 5.0f, -1.0f, -18.0f }, 2.0f, { 0.65f, 0.77f, 0.97f }, 1, 0.0f });
	spheres.push_back({ { 5.0f, 0.0f, -25.0f }, 3.0f, { 0.4f, 0.89f, 0.36f }, 1, 0.0f });
	spheres.push_back({ { -5.5f, 0.0f, -15.0f }, 3.0f, { 1.00f, 0.32f, 0.36f }, 1, 0.0f });
	spheres.push_back({ { -1.5f, 1.9f, -18.0f }, 2.0f, earth, 0, 0.0f });
	// lights
	spheres.push_back({ { 0.0, 10, 5 }, 1, { 0.00f, 0.00f, 0.00f }, 0, 0.0f, { 1, 1, 1 } });
	spheres.push_back({ { 1.0, 5, -40 }, 1, { 0.00f, 0.00f, 0.00f }, 0, 0.0f, { 1, 1, 1 } });
	RayTracer<5> rt(spheres);
	size_t w = 640, h = 480;
	rt.renderPNG(w, h, "./out/spheres.png");
	//rt.render(w, h, "./rt-no.png", 0);
	//rt.render(w, h, "./rt-ms.ppm", 1);
	//rt.render(w, h, "./rt-ss.ppm", 2);
	return 0;
}
