#pragma once
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cmath> 
#include "Ray.h"
#include "Vec3.h"
#include "Sphere.h"

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

template <size_t MAX_RAY_DEPTH>
class RayTracer {
	std::vector<Sphere> spheres;

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

public:
	RayTracer(const std::vector<Sphere>& spheres) : spheres(spheres) {}

	Vec3f trace(const Rayf &ray, int depth) {
		//if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
		float tnear = INFINITY;
		const Sphere* sphere = nullptr;
		// find intersection of this ray with the sphere in the scene
		for (auto& s : spheres) {
			float t = s.intersect(ray);
			if (t > 0 && t < tnear) {
				tnear = t;
				sphere = &s;
			}
		}
		// if there's no intersection return black or background color
		if (!sphere) return Vec3f(2);
		Vec3f surfaceColor = 0; // color of the ray/surfaceof the object intersected by the ray 
		Vec3f phit = ray.origin + ray.dir * tnear; // point of intersection 
		Vec3f nhit = phit - sphere->center; // normal at the intersection point 
		nhit.normalize(); // normalize normal direction 
						  // If the normal and the view direction are not opposite to each other
						  // reverse the normal direction. That also means we are inside the sphere so set
						  // the inside bool to true. Finally reverse the sign of IdotN which we want
						  // positive.
		float bias = 1e-4; // add some bias to the point from which we will be tracing 
		bool inside = false;
		if (dot(ray.dir, nhit) > 0) {
			nhit = -nhit;
			inside = true;
		}
		if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH) {
			float facingRatio = -dot(ray.dir, nhit);
			// change the mix value to tweak the effect
			float fresnelEffect = mix(pow(1 - facingRatio, 3), 1, 0.1);
			// compute reflection direction (not need to normalize because all vectors
			// are already normalized)
			Vec3f reflDir = ray.dir - nhit * 2 * dot(ray.dir, nhit);
			Vec3f reflection = trace({ phit + nhit * bias, reflDir.normalize() }, depth + 1);
			Vec3f refraction = 0;
			// if the sphere is also transparent compute refraction ray (transmission)
			if (sphere->transparency) {
				float ior = 1.1;
				float eta = inside ? ior : 1 / ior; // are we inside or outside the surface? 
				float cosi = -dot(nhit, ray.dir);
				float k = 1 - eta * eta * (1 - cosi * cosi);
				Vec3f reflDir = ray.dir * eta + nhit * (eta *  cosi - sqrt(k));
				refraction = trace({ phit - nhit * bias, reflDir.normalize() }, depth + 1);
			}
			// the result is a mix of reflection and refraction (if the sphere is transparent)
			surfaceColor = (
				reflection * fresnelEffect +
				refraction * (1 - fresnelEffect) * sphere->transparency) * sphere->surfaceColor;
		} else {
			// it's a diffuse object, no need to raytrace any further
			for (auto& s : spheres) {
				if (s.emissionColor.x > 0) {
					// this is a light
					Vec3f transmission = 1;
					Vec3f lightDirection = s.center - phit;
					lightDirection.normalize();
					for (auto& s2 : spheres) {
						if (&s == &s2) continue;
						if (s2.intersect({ phit + nhit * bias, lightDirection }) > 0) {
							transmission = 0;
							break;
						}
					}
					surfaceColor += sphere->surfaceColor * transmission *
						std::max(0.0f, dot(nhit, lightDirection)) * s.emissionColor;
				}
			}
		}

		return surfaceColor + sphere->emissionColor;
	}
	/**
		antialiasing: 0 - none, 1 - multisampling, >1 - supersampling
	*/
	void render(size_t width, size_t height, const std::string& filepath, size_t antialiasing) {
		if (antialiasing > 1) {
			width *= antialiasing;
			height *= antialiasing;
		} else if (antialiasing) {
			++width;
			++height;
		}
		VecOfVecs<Vec3f> image(height, std::vector<Vec3f>(width));
		float invWidth = 1 / float(width),
			invHeight = 1 / float(height);
		float fov = 30,
			aspectRatio = width / float(height);
		float angle = tan(M_PI * 0.5 * fov / 180.);
		// Trace rays
		for (unsigned y = 0; y < height; ++y) {
			for (unsigned x = 0; x < width; ++x) {
				float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectRatio;
				float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
				image[y][x] = trace({ Vec3f(0), Vec3f(xx, yy, -1).normalize() }, 0);
			}
		}
		// Save result to a PPM image (keep these flags if you compile under Windows)
		std::ofstream ofs(filepath, std::ios::out | std::ios::binary);
		if (!ofs) {
			std::cerr << "Bad stream" << std::endl;
		}
		if (antialiasing > 1) {
			renderSupersampled(ofs, image, antialiasing);
		} else if (antialiasing) {
			renderMultisampled(ofs, image);
		} else {
			renderNormal(ofs, image);
		}
		ofs.close();
	}

};