#ifndef BELONGSTOENUM_H_
#define BELONGSTOENUM_H_

#include <functional>
#include "Enums.h"
#include "../widget.h"
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

#endif