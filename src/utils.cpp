/**
 * @file utils.cpp
 * @author Erik Sandrén
 * @date 07-07-2018
 * @brief utility functions
 */
#include "utils.h"
#include <iostream>

namespace utils {
void SavePPMFile(const char* fileName, const uint32_t width, const uint32_t height,
								 const float data[]) {
	FILE* f = fopen(fileName, "w");
	fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);

	// ppm is stored top left to bottom right
	// opengl is stored with bottom left to top right
	for (int y = height - 1; y >= 0; --y) {
		for (unsigned int x = 0; x < width; ++x) {
			float d = data[y * height + x];
			d = d < 0 ? 0 : d > 1.0 ? 1.0 : d;
			fprintf(f, "%d %d %d ", int(d * 255), int(d * 255), int(d * 255));
		}
	}
	fclose(f);
}
} // namespace utils
