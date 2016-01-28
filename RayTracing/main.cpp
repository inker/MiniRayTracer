#include <string>
#include <vector> 
#include <iostream>
#include "Sphere.h"
#include "RayTracer.h"

int main(int argc, char **argv) {
	Texture chess("textures/chess-spoiled.png");
	Texture earth("textures/earth.png");
	std::vector<Sphere> spheres;
	
	// position, radius, surface color, reflectivity, transparency, emission color
	spheres.push_back({ { 0.0f, -1004.0f, -20.0f }, 1000.0f, { 0.20f, 0.20f, 0.20f }, 1, 0.0f });
	spheres.push_back({ { -2.0f, 0.0f, -25.0f }, 4.0f, { 0.2f, 0.55f, 0.21f }, 1, 0.0f });
	spheres.push_back({ { 5.0f, 1.0f, -18.0f }, 2.0f, { 0.65f, 0.77f, 0.97f }, 1, 0.9f });
	spheres.push_back({ { 5.0f, 1.0f, -25.0f }, 3.0f, earth, 0, 0.0f });
	spheres.push_back({ { -5.5f, 0.0f, -15.0f }, 3.0f, { 1.00f, 0.32f, 0.36f }, 1, 0.0f });
	spheres.push_back({ { -1.5f, 1.9f, -18.0f }, 2.0f, chess, 0, 0.0f });
	// lights
	spheres.push_back({ { 0.0, 10, 5 }, 1, { 0.00f, 0.00f, 0.00f }, 0, 0.0f, { 1, 1, 1 } });
	spheres.push_back({ { 1.0, 5, -40 }, 1, { 0.00f, 0.00f, 0.00f }, 0, 0.0f, { 1, 1, 1 } });
	RayTracer<10> rt(spheres);
	rt.render(640, 480, "./out/spheres.png");
	return 0;
}
