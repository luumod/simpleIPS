#ifndef BELONGSTOENUM_H_
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

int switch_Dialog_id(int id)
{
	switch (id)
	{
	case BLUR::Average:
		return 0;
	case BLUR::Gaussian:
		return 1;
	case BLUR::Median:
		return 2;
	case BLUR::Bilateral:
		return 3;
	case THRESHOLD::Binary:
	case THRESHOLD::Binary_inv:
	case THRESHOLD::Trunc:
	case THRESHOLD::Tozero:
	case THRESHOLD::Tozero_inv:
		return 4;
	case FORM::Erode:
	case FORM::Dilate:
	case FORM::Open:
	case FORM::Close:
	case FORM::Gradient:
	case FORM::Tophat:
	case FORM::Blackhat:
	case FORM::Hitmiss:
		return 5;
	case CONNECTED::CONNECTED_TYPE1:
	case CONNECTED::CONNECTED_TYPE2:
		return 6;
	case CONTOURS::CONTOURS_TYPE1:
		return 7;
	case SHOW::LIGHT:
		return 8;
	case SHOW::GAMMA:
		return 9;
	case SHOW::LINEAR:
		return 10;
	case SHOW::GRAYWINDOW:
		return 11;
	case SHOW::DPLINEAR:
		return 12;
	case SHOW::NON_DPLINEAR:
		return 13;
	default:
		break;
	}
	return 0;
}

#endif