#pragma once

#include "Unicode/UnicodeUtil.h"
#define EmptyString													""

namespace FCMUtil
{
	int		toInt(CString val);
	double	toDouble(CString val);

	std::string ToString(const CString val);
}

