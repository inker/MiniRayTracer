#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <math.h>
#include "Sphere.h"
#include "util.h"

template <size_t MAX_RAY_DEPTH>
class RayTracer {
	std::vector<Sphere> spheres;

	Vec3f trace(const Rayf &ray, int depth) {
		//if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
		float tNear = INFINITY;
		const Sphere* sphere = nullptr;
		// find intersection of this ray with the sphere in the scene
		for (auto& s : spheres) {
			float t = s.intersect(ray);
			if (t > 0 && t < tNear) {
				tNear = t;
				sphere = &s;
			}
		}
		// if there's no intersection return black or background color
		if (!sphere) return Vec3f(2);
		Vec3f surfaceColor = 0; // color of the ray/surfaceof the object intersected by the ray 
		Vec3f point = ray.origin + ray.dir * tNear; // point of intersection
		Vec3f normal = point - sphere->center; // normal at the intersection point 
		normal.normalize(); // normalize normal direction 
		// if the normal and the view direction are not opposite to each other, reverse the normal direction
		float bias = 1e-4; // add some bias to the point from which we'll be tracing 
		bool inside = false;
		if (dot(ray.dir, normal) > 0) {
			normal = -normal;
			inside = true;
		}

		Vec3f colorAtPoint = sphere->surfaceColor;
		if (sphere->texture) {
			auto spherical = toSpherical(sphere->center, point);
			auto coor = sphere->texture->fromUV(toUV(spherical));
			colorAtPoint = (*sphere->texture)[coor.second][coor.first];
		}

		if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH) {
			float facingRatio = -dot(ray.dir, normal);
			float fresnelEffect = mix(pow(1 - facingRatio, 3), 1, 0.1); // mix value may be changed
			// compute reflection direction
			// (not need to normalize because all vectors are already normalized) 
			Vec3f reflDir = ray.dir - normal * 2 * dot(ray.dir, normal);
			Vec3f reflection = trace({ point + normal * bias, reflDir.normalize() }, depth + 1);
			Vec3f refraction = 0;
			// if the sphere is also transparent compute refraction ray (transmission)
			if (sphere->transparency) {
				float ior = 1.1;
				float eta = inside ? ior : 1 / ior; // are we inside or outside the surface? 
				float cosi = -dot(normal, ray.dir);
				float k = 1 - eta * eta * (1 - cosi * cosi);
				Vec3f reflDir = ray.dir * eta + normal * (eta *  cosi - sqrt(k));
				refraction = trace({ point - normal * bias, reflDir.normalize() }, depth + 1);
			}
			// the result is a mix of reflection and refraction (if the sphere is transparent)
			surfaceColor = (
				reflection * fresnelEffect +
				refraction * (1 - fresnelEffect) * sphere->transparency) * colorAtPoint;
		} else {
			// it's a diffuse object, no need to raytrace any further
			for (auto& s : spheres) {
				if (s.emissionColor.x > 0) {
					// this is a light
					Vec3f transmission = 1;
					Vec3f lightDirection = s.center - point;
					lightDirection.normalize();
					for (auto& s2 : spheres) {
						if (&s == &s2) continue;
						if (s2.intersect({ point + normal * bias, lightDirection }) > 0) {
							transmission = 0;
							break;
						}
					}
					surfaceColor += colorAtPoint * transmission *
						std::max(0.0f, dot(normal, lightDirection)) * s.emissionColor;
				}
			}
		}

		// finally, return the resulting pixel's color
		return surfaceColor + sphere->emissionColor;
	}

public:

	RayTracer(const std::vector<Sphere>& spheres) : spheres(spheres) {}

	void render(size_t width, size_t height, const std::string& filepath) {
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
		toPNG(filepath, image);
	}

};