/**
* @file utils.h
* @author Erik Sandrén
* @date 07-07-2018
* @brief [Description Goes Here]
*/

#ifndef __UTILS_H__
#define __UTILS_H__
#include <cstdint>

namespace utils {
void SavePPMFile(const char* fileName, uint32_t width, uint32_t height, const float data[]);
}

#endif
