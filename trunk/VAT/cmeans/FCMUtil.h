#pragma once

#include "Unicode/UnicodeUtil.h"
#include "define.h"

#define EmptyString													""

namespace FCMUtil
{
	int		toInt(CString val);
	double	toDouble(CString val);

	std::string ToString(const CString val);

	double	ManhattanDist(Vector& v1, Vector& v2);
	double	EuclideanDist(Vector& v1, Vector& v2);
	double	MinkowskiDist(Vector& v1, Vector& v2, int q);
}

