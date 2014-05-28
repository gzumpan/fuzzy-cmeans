#include "stdafx.h"
#include "FCMUtil.h"

namespace FCMUtil
{
	int toInt(CString val)
	{
		return _tstoi((LPCTSTR)val);
	}

	double toDouble(CString val)
	{
		return _tstof((LPCTSTR)val);
	}

	std::string ToString(const CString val)
	{
		std::string result;

		if (UnicodeUtil::WstringToUtf8(std::wstring(val), result) == FALSE) {
			return (EmptyString);
		}

		return (result);
	}
}