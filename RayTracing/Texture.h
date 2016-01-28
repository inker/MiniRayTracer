#pragma once

#include <vector>
#include "png/png.h"
#include "util.h"

class Texture {
	VecOfVecs<Vec3f> image;
	size_t w, h;
public:
	bool cubic;

	Texture(const std::string& path, bool cubic = false): cubic(cubic) {
		std::vector<unsigned char> linVec;
		unsigned error = lodepng::decode(linVec, w, h, path);
		if (error) {
			std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		}
		image.resize(h);
		for (size_t y = 0; y < h; ++y) {
			auto& row = image[y] = std::vector<Vec3f>(w);
			size_t yOffset = y * w;
			for (size_t x = 0; x < w; ++x) {
				size_t offset = (yOffset + x) << 2;
				row[x] = {
					float(linVec[offset]) / 255,
					float(linVec[offset + 1]) / 255,
					float(linVec[offset + 2]) / 255
				};
			}
		}
	}

	std::pair<size_t, size_t> fromUV(const std::pair<float, float>& uv) {
		auto coor = std::make_pair(uv.first * w, uv.second * h);
		//auto floor = std::make_pair((size_t)coor.first, (size_t)coor.second);
		//auto ceil = std::make_pair(size_t(coor.first + 1), )
		return std::make_pair(std::min((size_t)coor.first, w - 1), std::min((size_t)coor.second, h - 1));
	}

	size_t width() { return w; }
	size_t height() { return h; }

	std::vector<Vec3f>& operator [] (size_t y) { return image[y]; }
	std::vector<Vec3f> operator [] (size_t y) const { return image[y]; }
};