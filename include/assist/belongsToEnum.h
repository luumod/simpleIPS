﻿#ifndef BELONGSTOENUM_H_
#define BELONGSTOENUM_H_

#include "Enums.h"

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
	else if constexpr (std::is_same_v<T, CVTCOLOR>) {
		switch (static_cast<T>(data)) {
		case T::CVTCOLOR_TYPE1:
			return true;
		default:
			return false;
		}
	}
	else if constexpr (std::is_same_v<T, SHOW>) {
		switch (static_cast<T>(data)) {
		case T::LIGHT:
			return true;
		case T::SHARPEN:
			return true;
		default:
			return false;
		}
	}
	return false;
}

#endif