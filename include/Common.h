﻿#ifndef COMMON_H_
#define COMMON_H_

constexpr auto WINDOW_WIDTH = 640;
constexpr auto WINDOW_HEIGHT = 640;

constexpr auto SUB_LAB_IMG_WIDTH = 200;
constexpr auto SUB_LAB_IMG_HEIGHT = 200;

#define AVG_BLUR_MIN_SIZE 1
#define AVG_BLUR_MAX_SIZE 30

#define GAUSSIAN_BLUR_MIN_SIZE 1
#define GAUSSIAN_BLUR_MAX_SIZE 41
#define GAUSSIAN_BLUR_SIGMAX 40
#define GAUSSIAN_BLUR_SIGMAY 40

#define MEDIAN_BLUR_MAX 81

#define BILATERAL_BLUR_MAX_SIZE 50
#define BILATERAL_BLUR_MAX_COLOR 120
#define BILATERAL_BLUR_MAX_SPACE 150


#endif