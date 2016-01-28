#pragma once
#include <algorithm>
#include <utility>
#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include "Vec3.h"
#include "Ray.h"

#if defined __linux__ || defined __APPLE__ 
// "Compiled for Linux
#else 
// Windows doesn't define these values by default, Linux does
#define M_PI 3.14159265358979323846
#define INFINITY 1e8 
#endif 

using Vec3f = Vec3<float>;
using Rayf = Ray<float>;
template<typename T> using VecOfVecs = std::vector<std::vector<T>>;

float mix(float a, float b, float mix) {
	return b * mix + a * (1.0f - mix);
}

// first - latitude, second - longitude
std::pair<float, float> toSpherical(const Vec3f& center, const Vec3f& point) {
	Vec3f v = point - center;
	return std::make_pair(atan2f(v.z, v.x), acosf(v.y / v.length()));
}

// first - U, second - V
std::pair<float, float> toUV(const std::pair<float, float>& spherical) {
	float invPi = 1.0f / M_PI;
	return std::make_pair(1 - spherical.first * 0.5f * invPi, spherical.second * invPi);
}

void toPNG(const std::string& fileName, const VecOfVecs<Vec3f>& image) {
	size_t width = image.front().size(), height = image.size();
	std::vector<unsigned char> imgVec;
	for (auto& row : image) {
		for (auto& pixel : row) {
			imgVec.push_back((unsigned char)std::min(255, int(pixel.x * 255)));
			imgVec.push_back((unsigned char)std::min(255, int(pixel.y * 255)));
			imgVec.push_back((unsigned char)std::min(255, int(pixel.z * 255)));
			imgVec.push_back(255);
		}
	}
	unsigned error = lodepng::encode(fileName, imgVec, width, height);
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

