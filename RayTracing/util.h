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

std::pair<float, float> toCubicUV(const std::pair<float, float>& spherical) {
	float invPi = 1.0f / M_PI;
	float a = 1 - spherical.first * 0.5f * invPi, b = spherical.second * invPi;
	float x, y;
	if (a < 0.25f) {

	} else if (a >= 0.75f) {

	} else {
		float temp = a * 4.0f;
		x = (int)temp - temp;
		y = b * 2.0f - 0.5f;
	}
	return std::make_pair(x, y);
}

void outputPixel(std::ofstream& ofs, const Vec3f& pixel) {
	ofs << (unsigned char)std::min(255, int(pixel.x * 255)) <<
		(unsigned char)std::min(255, int(pixel.y * 255)) <<
		(unsigned char)std::min(255, int(pixel.z * 255));
}

void outputPixel(std::ofstream& ofs, const VecOfVecs<Vec3f>& image, size_t x, size_t y, size_t side) {
	if (side == 0) {
		return outputPixel(ofs, image[y][x]);
	}
	Vec3<unsigned> sum(0);
	for (size_t i = y; i < y + side; ++i) {
		for (size_t j = x; j < x + side; ++j) {
			auto& el = image[i][j];
			//std::cout << el.x << std::endl;
			sum += {
				(std::min(unsigned(255), unsigned(el.x * 255))),
					(std::min(unsigned(255), unsigned(el.y * 255))),
					(std::min(unsigned(255), unsigned(el.z * 255))),
			};
		}
	}
	sum /= (side * side);
	Vec3<unsigned char> px = { (unsigned char)sum.x, (unsigned char)sum.y, (unsigned char)sum.z };
	ofs << px.x << px.y << px.z;
}

void renderSupersampled(std::ofstream& ofs, const VecOfVecs<Vec3f>& image, size_t nSamples) {
	size_t width = image.front().size(), height = image.size();
	ofs << "P6\n" << width / nSamples << " " << height / nSamples << "\n255\n";
	for (size_t y = 0; y < height; y += nSamples) {
		for (size_t x = 0; x < width; x += nSamples) {
			outputPixel(ofs, image, x, y, nSamples);
		}
	}
}

void renderMultisampled(std::ofstream& ofs, const VecOfVecs<Vec3f>& image) {
	size_t width = image.front().size() - 1, height = image.size() - 1;
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t y = 0; y < height; ++y) {
		for (size_t x = 0; x < width; ++x) {
			outputPixel(ofs, image, x, y, 2);
		}
	}
}

void renderNormal(std::ofstream& ofs, const VecOfVecs<Vec3f>& image) {
	size_t width = image.front().size(), height = image.size();
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (auto& row : image) {
		for (auto& pixel : row) {
			outputPixel(ofs, pixel);
		}
	}
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

