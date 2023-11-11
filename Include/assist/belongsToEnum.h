#ifndef BELONGSTOENUM_H_
#define BELONGSTOENUM_H_

#include <functional>
#include "Enums.h"
#include "../Widget/widget.h"
template<typename T>
inline bool belongsToEnum(int data) {
	if constexpr (std::is_same_v<T, BLUR>) {
		switch (static_cast<T>(data)) {
		case T::Average:
		case T::Gaussian:
		case T::Median:
		case T::Bilateral:
			return true;
		default:
			return false;
		}
	}
	else if constexpr (std::is_same_v<T, THRESHOLD>) {
		switch (static_cast<T>(data)) {
		case T::Binary:
		case T::Binary_inv:
		case T::Trunc:
		case T::Tozero:
		case T::Tozero_inv:
			return true;
		default:
			return false;
		}
	}
	else if constexpr (std::is_same_v<T, FORM>) {
		switch (static_cast<T>(data)) {
		case T::Erode:
		case T::Dilate:
		case T::Open:
		case T::Close:
		case T::Gradient:
		case T::Tophat:
		case T::Blackhat:
		case T::Hitmiss:
			return true;
		default:
			return false;
		}
	}
	else if constexpr (std::is_same_v<T, CONNECTED>) {
		switch (static_cast<T>(data)) {
		case T::CONNECTED_TYPE1:
		case T::CONNECTED_TYPE2:
			return true;
		default:
			return false;
		}
	}
	else if constexpr (std::is_same_v<T, CONTOURS>) {
		switch (static_cast<T>(data)) {
		case T::CONTOURS_TYPE1:
			return true;
		default:
			return false;
		}
	}
	else if constexpr (std::is_same_v<T, SHOW>) {
		switch (static_cast<T>(data)) {
		case T::LIGHT:
			return true;
		case T::GAMMA:
			return true;
		case T::LINEAR:
			return true;
		case T::GRAYWINDOW:
			return true;
		case T::DPLINEAR:
			return true;
		case T::NON_DPLINEAR:
			return true;
		default:
			return false;
		}
	}
	return false;
}

// 定义一个映射表，将枚举值与操作标识关联起来
static std::map<int, int> operationMap = {
	{BLUR::Average, 0},
	{BLUR::Gaussian, 1},
	{BLUR::Median, 2},
	{BLUR::Bilateral, 3},
	{THRESHOLD::Binary, 4},
	{THRESHOLD::Binary_inv, 4},
	{THRESHOLD::Trunc, 4},
	{THRESHOLD::Tozero, 4},
	{THRESHOLD::Tozero_inv, 4},
	{FORM::Erode, 5},
	{FORM::Dilate, 5},
	{FORM::Open, 5},
	{FORM::Close, 5},
	{FORM::Gradient, 5},
	{FORM::Tophat, 5},
	{FORM::Blackhat, 5},
	{FORM::Hitmiss, 5},
	{CONNECTED::CONNECTED_TYPE1, 6},
	{CONNECTED::CONNECTED_TYPE2, 6},
	{CONTOURS::CONTOURS_TYPE1, 7},
	{SHOW::LIGHT, 8},
	{SHOW::GAMMA, 9},
	{SHOW::LINEAR, 10},
	{SHOW::GRAYWINDOW, 11},
	{SHOW::DPLINEAR, 12},
	{SHOW::NON_DPLINEAR, 13}
};

// 使用映射表进行操作查找
static int switch_Dialog_id(int id) {
	auto it = operationMap.find(id);
	if (it != operationMap.end()) {
		return it->second;
	}
	return 0;
}


//图片信息：格式转换
QString imageFormatToString(QImage::Format format) {
    switch (format) {
    case QImage::Format_Invalid:
        return "Invalid";
    case QImage::Format_Mono:
        return "Mono";
    case QImage::Format_MonoLSB:
        return "MonoLSB";
    case QImage::Format_Indexed8:
        return "Indexed8";
    case QImage::Format_RGB32:
        return "RGB32";
    case QImage::Format_ARGB32:
        return "ARGB32";
    case QImage::Format_ARGB32_Premultiplied:
        return "ARGB32_Premultiplied";
    case QImage::Format_RGB16:
        return "RGB16";
    case QImage::Format_ARGB8565_Premultiplied:
        return "ARGB8565_Premultiplied";
    case QImage::Format_RGB666:
        return "RGB666";
    case QImage::Format_ARGB6666_Premultiplied:
        return "ARGB6666_Premultiplied";
    case QImage::Format_RGB555:
        return "RGB555";
    case QImage::Format_ARGB8555_Premultiplied:
        return "ARGB8555_Premultiplied";
    case QImage::Format_RGB888:
        return "RGB888";
    case QImage::Format_RGB444:
        return "RGB444";
    case QImage::Format_ARGB4444_Premultiplied:
        return "ARGB4444_Premultiplied";
    case QImage::Format_RGBX8888:
        return "RGBX8888";
    case QImage::Format_RGBA8888:
        return "RGBA8888";
    case QImage::Format_RGBA8888_Premultiplied:
        return "RGBA8888_Premultiplied";
    case QImage::Format_BGR30:
        return "BGR30";
    case QImage::Format_A2BGR30_Premultiplied:
        return "A2BGR30_Premultiplied";
    case QImage::Format_RGB30:
        return "RGB30";
    case QImage::Format_A2RGB30_Premultiplied:
        return "A2RGB30_Premultiplied";
    case QImage::Format_Alpha8:
        return "Alpha8";
    case QImage::Format_Grayscale8:
        return "Grayscale8";
    case QImage::Format_Grayscale16:
        return "Grayscale16";
    case QImage::Format_RGBX64:
        return "RGBX64";
    case QImage::Format_RGBA64:
        return "RGBA64";
    case QImage::Format_RGBA64_Premultiplied:
        return "RGBA64_Premultiplied";
    case QImage::Format_BGR888:
        return "BGR888";
    case QImage::Format_RGBX16FPx4:
        return "RGBX16FPx4";
    case QImage::Format_RGBA16FPx4:
        return "RGBA16FPx4";
    case QImage::Format_RGBA16FPx4_Premultiplied:
        return "RGBA16FPx4_Premultiplied";
    case QImage::Format_RGBX32FPx4:
        return "RGBX32FPx4";
    case QImage::Format_RGBA32FPx4:
        return "RGBA32FPx4";
    case QImage::Format_RGBA32FPx4_Premultiplied:
        return "RGBA32FPx4_Premultiplied";
    default:
        return "Unknown Format";
    }
}


#endif
