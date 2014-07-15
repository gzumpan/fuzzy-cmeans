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

	double	ManhattanDist(Vector& v1, Vector& v2)
	{
		ASSERT(v1.size() == v2.size());

		double sum = 0;
		
		for (int i = 0; i < v1.size(); i++)
		{
			sum += abs(v1[i] - v2[i]);
		}

		return sum;
	}

	double	EuclideanDist(Vector& v1, Vector& v2)
	{
		ASSERT(v1.size() == v2.size());

		double sum = 0;

		for (int i = 0; i < v1.size(); i++)
		{
			sum += pow(v1[i] - v2[2], 2);
		}

		return sum;
	}

	double	MinkowskiDist(Vector& v1, Vector& v2, int q)
	{
		ASSERT(v1.size() == v2.size());

		double sum = 0;

		for (int i = 0; i < v1.size(); i++)
		{
			sum += pow(abs(v1[i] - v2[2]), q);
		}

		return sum;
	}
}