#include "StdAfx.h"
#include "DMCUtil.h"

#include <afxtempl.h>

#include <process.h>
#include <windows.h>
#include <wincon.h>
#include <wininet.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <wtypes.h>
#include <Shlobj.h>
#include <windns.h>
#include <MMSystem.h>
#include <Aclapi.h>

#include <cctype>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <list>
#include <conio.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <stdlib.h>
#include <share.h>
#include <sstream>
#include <wtypes.h>
#include <time.h>

#include "Utility/DDCUtil.h"
#include "Utility/RegUtil.h"
#include "Utility/DefApps.h"
#include "Utility/FileTypeIcon.h"
#include "Unicode/UnicodeUtil.h"
#include "File/FileOperation.h"
#include "Utility/TimeEx.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


#define MALLOC(x)	HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x)		HeapFree(GetProcessHeap(), 0, (x))

#define RELEASE_THREAD_TIMEOUT										(1000)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace DMCUtil
{

	CString ToCString(const INT val)
	{
		CString s;

		s.Format(_T("%d"), val);

		return (s);
	}

	CString ToCString(const UINT val)
	{
		CString s;
		
		s.Format(_T("%u"), val);
		
		return (s);
	}

	CString ToCString(const ULONG val)
	{
		CString s;
		
		s.Format(_T("%lu"), val);
		
		return (s);
	}

	CString ToCString(const LONGLONG val)
	{
		CString s;

		s.Format(_T("%I64d"), val);

		return (s);
	}

	CString ToCString(const std::string val)
	{
		CString result;
		wstring wsStr;
				
		// Convert from UTF8 to wstring
		if (UnicodeUtil::Utf8ToWstring(val, wsStr) == TRUE) {
			result = wsStr.c_str();

		} else {
			// Can't convert (input may not utf8 string)
			// Set to original input
			result = CString(val.c_str());			
		}

		return (result);
	}

	CString ToCStringFromDateTime(const LONGLONG llDateTime, BOOL bCompact, BOOL bSecond, BOOL bUnix, CString szAMText, CString szPMText)
	{
		CString szRes, szFormat;

		if (!bUnix) {

			// Not Unix:
			CTime pastTime(DMCUtil::ToSystemTime(llDateTime));
			CTime nowTime(DMCUtil::GetCurrentSystemTime());

			// Check if they are same year/month/day?
			if ((bCompact == TRUE) &&
				(pastTime.GetYear()	 == nowTime.GetYear()) &&
				(pastTime.GetMonth() == nowTime.GetMonth()) &&
				(pastTime.GetDay()	 == nowTime.GetDay())) {

				// They are same day.
				szFormat = bSecond ? DTIME_APM_HHMMSS:DTIME_APM_HHMM;				

			} else {
				szFormat = bSecond ? DTIME_YYMMDD_APM_HHMMSS:DTIME_YYMMDD_APM_HHMM;
			}
			
			// Format
			szRes = (pastTime.Format(szFormat));

			// Remove leading zero of hour part
			szRes = RemoveLeadingZero(szFormat, szRes);
			szRes.Replace(_T("AM"), szAMText);
			szRes.Replace(_T("PM"), szPMText);

		} else {

			// Unix (Ninh added):
			szRes = (DateTimeUnix2AppStr(llDateTime, bCompact, bSecond, szAMText, szPMText));							
		}
		
		return (szRes);
	}
	
	CString ToCStringFromDateTimeFormat(int year, int month, int day, CString szFormat)
	{
		// CTime only uses from 1970
		// CTime time(DMCUtil::ConvertTimestampToLLONG(year, month, day));
		// COleDateTime can use from 1900
		COleDateTime time(year, month, day, 0, 0, 0);
		CString szRes;

		szRes = time.Format(szFormat);
		return (szRes);
	}

	std::string ToString(const INT val) 
	{ 
		char buffer[256] = {0}; 
		string s;
		
		// Convert to string
		sprintf(buffer, "%d", val);
		
		// Copy the buffer into the string object 
		s = buffer;

		return (s); 
	}

	std::string ToString(const UINT val) 
	{ 
		char buffer[256] = {0}; 
		string s;
		
		// Convert to string 
		sprintf(buffer, "%u", val);
		
		// Copy the buffer into the string object 
		s = buffer;
		
		return (s); 
	}

	std::string ToString(const ULONG val) 
	{ 
		char buffer[256] = {0}; 
		string s;
		
		// Convert to string 
		sprintf(buffer, "%lu", val);
		
		// Copy the buffer into the string object 
		s = buffer;
		
		return (s); 
	}

	std::string ToString(const LONGLONG val)
	{
		return (ToString(ToCString(val)));
	}

	std::string ToString(const CString val)
	{
		std::string result;

		if (UnicodeUtil::WstringToUtf8(wstring(val), result) == FALSE) {
			return (EmptyString);
		}

		return (result);
	}

	std::string ToString(const WCHAR* val)
	{
		std::string result;

		if (UnicodeUtil::WstringToUtf8(val, result) == FALSE) {
			return (EmptyString);
		}

		return (result);
	}

	std::string ToLower(std::string val)
	{
		std::transform(val.begin(), val.end(), val.begin(), ::tolower);
		return (val);
	}

	INT ToINT(const CString val)
	{
		return (ToINT(DMCUtil::ToString(val)));
	}

	INT ToINT(const std::string val)
	{
		return (atoi(val.c_str()));
	}

	UINT ToUINT(const CString val)
	{
		return ((UINT)ToINT(val));
	}

	UINT ToUINT(const std::string val)
	{
		return ((UINT)ToINT(val));	
	}

	ULONG ToULONG(const CString val)
	{
		return (ToULONG(DMCUtil::ToString(val)));
	}
	
	ULONG ToULONG(const std::string val)
	{
		return ((ULONG)atol(val.c_str()));	
	}

	LONGLONG ToLLONG(const std::string s)
	{
		LONGLONG val;
		const char* p;
		
		val = 0;
		p = s.c_str();

		for (; *p; p++) {
			val = 10*val + (*p - '0');
		}
		
		return (val);
	}
	
	BOOL ToBOOL(const CString val)
	{
		BOOL bRes = FALSE;
		CString szLowerVal;

		szLowerVal =  val;
		szLowerVal.MakeLower();

		if (szLowerVal == "true" || szLowerVal == "1") {
			bRes = TRUE;

		} else if (szLowerVal == "false" || szLowerVal == "0") {
			bRes = FALSE;

		} else {
			// error: ?
			bRes = FALSE;
		}

		return bRes;		
	}

	BOOL ToBOOL(const std::string val)
	{
		return (ToBOOL(ToCString(val)));
	}

	//////////////////////////////////////////////////////////////////////////
	// FILETIME Utility
	//////////////////////////////////////////////////////////////////////////

	LONGLONG CreateLLONGSessionID()
	{
		return (CreateLLONGDateTime(TRUE)/10000);
	}

	LONGLONG CreateLLONGDateTime(BOOL miliSleep)
	{
		SYSTEMTIME stTime;
		FILETIME nft;
		LARGE_INTEGER nli;
		
		if (miliSleep) {
			Sleep(1);
		}

		stTime = DMCUtil::GetCurrentSystemTime();
		SystemTimeToFileTime(&stTime, &nft);
		
		nli.LowPart		= nft.dwLowDateTime;
		nli.HighPart	= nft.dwHighDateTime;

		return (nli.QuadPart);
	}

	LONGLONG CreateLLONGDateTime(SYSTEMTIME systemTime)
	{
		FILETIME nft;
		LARGE_INTEGER nli;
		
		SystemTimeToFileTime(&systemTime, &nft);
		
		nli.LowPart		= nft.dwLowDateTime;
		nli.HighPart	= nft.dwHighDateTime;
		
		return (nli.QuadPart);
	}

	LONGLONG CreateLLONGDateTime(FILETIME pTimeStamp)
	{
		LARGE_INTEGER nli;

		nli.LowPart		= pTimeStamp.dwLowDateTime;
		nli.HighPart	= pTimeStamp.dwHighDateTime;

		return (nli.QuadPart);
	}

	LONGLONG CreateLLONGUnixDateTime(FILETIME pTimeStamp)
	{
		// Quick convert
		LONGLONG llRes;

 		llRes = (CreateLLONGDateTime(pTimeStamp)/10000000) - SECONDS_DIFF_BETWEEN_UNIXUTC_AND_FILETIME;
		return (llRes);
	}

	LONGLONG CreateLLONGUnixMSecDateTime(FILETIME pTimeStamp)
	{
		// Quick convert
		LONGLONG llRes;
		
		llRes = (CreateLLONGDateTime(pTimeStamp)/10000) - SECONDS_DIFF_BETWEEN_UNIXUTC_AND_FILETIME*1000;
		return (llRes);
	}

	LONGLONG CreateLLONGMSecDateTime()
	{
		return (CreateLLONGDateTime() / 10000);
	}

	LONGLONG CreateLLONGSecDateTime()
	{
		return (CreateLLONGDateTime() / 10000000);
	}
	
	LONGLONG CreateLLONGUnixDateTime()
	{
		// Return current time: seconds since UNIX epoch, UTC (not local time)		
		FILETIME now;

		GetSystemTimeAsFileTime(&now);

		return (CreateLLONGUnixDateTime(now));
	}
	
	LONGLONG CreateLLONGUnixMSecDateTime()
	{
		FILETIME now;

		GetSystemTimeAsFileTime(&now);
		
		return (CreateLLONGUnixMSecDateTime(now));
	}

	CString DateTimeUnix2AppStr(LONGLONG llUnixDateTime, 
								BOOL bCompact, BOOL bSecond,
								CString szAMText, CString szPMText)
	{
		CString szRes, szFormat;
		CString szDate1, szDate2;

		time_t rawtime;		
		struct tm *timeInfo;		
		char buffer[128] = {0};

		if (llUnixDateTime < 0) {
			return _T("");
		}
		
		// Get current date
		rawtime		= (time_t)CreateLLONGUnixDateTime();
		timeInfo	= localtime(&rawtime);

		// Format 
		strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeInfo);
		szDate1		= CString(buffer);

		// The input date
		rawtime		= (time_t)llUnixDateTime;
		timeInfo	= localtime(&rawtime);

		// Format 
		strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeInfo);
		szDate2		= CString(buffer);

		// Compare
		if (bCompact && szDate1 == szDate2) {
			// Compact mode and the date is current date
			szFormat = 	bSecond ? DTIME_APM_HHMMSS : DTIME_APM_HHMM;			

		} else {		
			// Not compact mode or not current date
			szFormat = bSecond ? DTIME_YYMMDD_APM_HHMMSS : DTIME_YYMMDD_APM_HHMM;
		}

		// Format
		strftime(buffer, sizeof(buffer), ToString(szFormat).c_str(), timeInfo);		
		szRes = CString(buffer);

		// Remove leading zero
		szRes = RemoveLeadingZero(szFormat, szRes);
		szRes.Replace(_T("AM"), szAMText);
		szRes.Replace(_T("PM"), szPMText);
		
		return (szRes);		
	}

	CString RemoveLeadingZero(CString szFormat, CString szDateTime, BOOL bOnlyHour /* = TRUE*/)
	{
		CString szRes;

		szRes = szDateTime;

		if (szFormat == DTIME_APM_HHMMSS ||
			szFormat == DTIME_YYMMDD_APM_HHMMSS ||
			szFormat == DTIME_APM_HHMM ||
			szFormat == DTIME_YYMMDD_APM_HHMM)
		{
			if (bOnlyHour) {
				// Replace "M 0" by "M " if it not contain "M 0:"
				if (szRes.Find(_T("M 0:")) < 0) {
					szRes.Replace(_T("M 0"), _T("M "));
				}
			} else {
				// Remove leading zero for all parts: implement later
			}
		}

		return (szRes);		
	}
	
	SYSTEMTIME GetCurrentSystemTime()
	{
		SYSTEMTIME stUTC, stLocal;
		
		GetSystemTime(&stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

		return (stLocal);	
	}

	SYSTEMTIME ToSystemTime(LONGLONG llDateTime)
	{
		FILETIME nft;
		LARGE_INTEGER nli;
		SYSTEMTIME stUTC;
		
		// Convert LLONG date time to system time.
		nli.QuadPart		= llDateTime;		
		nft.dwHighDateTime	= nli.HighPart;
		nft.dwLowDateTime	= nli.LowPart;
		
		FileTimeToSystemTime(&nft, &stUTC);

		return (stUTC);
	}

	FILETIME ToFileTime(LONGLONG llDateTime)
	{
		FILETIME nft;
		LARGE_INTEGER nli;

		nli.QuadPart = llDateTime;
		
		nft.dwHighDateTime = nli.HighPart;
		nft.dwLowDateTime = nli.LowPart;

		return (nft);
	}

	LONGLONG ConvertTimestampToLLONG(std::string datetime)
	{
		LONGLONG tUTCTime;
		struct tm timeInfo;
		char year[5] = {0}, month[3] = {0}, day[3] = {0};
		char hour[3] = {0}, minute[3] = {0}, second[3] = {0};
		char c;

		// DateTime has format: "2013-12-03T06:55:42Z"
		sscanf(datetime.c_str(), "%4s%c%2s%c%2s%c%2s%c%2s%c%2s%c", year, &c, month, &c, day, &c, hour, &c, minute, &c, second, &c);
	
		// Set input time: XMPP time is UTC
		memset(&timeInfo, 0, sizeof(struct tm));

		timeInfo.tm_year	= atoi(year) - 1900;		// tm_year: years since 1900
		timeInfo.tm_mon		= atoi(month) - 1;			// tm_mon: months since January, 0-11
		timeInfo.tm_mday	= atoi(day);
		timeInfo.tm_hour	= atoi(hour);
		timeInfo.tm_min		= atoi(minute);
		timeInfo.tm_sec		= atoi(second);

		tUTCTime = (LONGLONG)mktime(&timeInfo) + TimeDiffLocalAndUTC();
		return (tUTCTime);
	}
	
	std::string ConvertUTCToLocalTime(std::string datetime)
	{
		std::string datestamp;
		char year[5] = {0}, month[3] = {0}, day[3] = {0};
		char hour[3] = {0}, minute[3] = {0}, second[3] = {0};
		char c;

		// DateTime has format: YYYYMMDDTHH:mm:ss - eg: 20131203T14:30:00
		sscanf(datetime.c_str(), "%4s%2s%2s%c%2s%c%2s%c%2s", year, month, day, &c, hour, &c, minute, &c, second);

		CTime time (atoi(year), atoi(month), atoi(day), atoi(hour), atoi(minute), atoi(second));		
		SYSTEMTIME stUTC, stLocal;

		time.GetAsSystemTime(stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

		datestamp = DMCUtil::ToString(DMCUtil::CreateLLONGDateTime(stLocal));
		return (datestamp);
	}

	LONGLONG ConvertTimestampToLLONG(int year, int month, int day)
	{
		LONGLONG tUTCTime;
		struct tm tmTimeInfo;
		
		memset(&tmTimeInfo, 0, sizeof(struct tm));
		
		// Set input time: XMPP time is UTC
		tmTimeInfo.tm_year	= year - 1900;		// tm_year: years since 1900
		tmTimeInfo.tm_mon	= month - 1;		// tm_mon: months since January, 0-11
		tmTimeInfo.tm_mday	= day;
		
		tUTCTime = (LONGLONG)mktime(&tmTimeInfo) + TimeDiffLocalAndUTC();
		return (tUTCTime);
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	LONGLONG DateTimeXMPP2Unix(std::string datetime)
	{
		return (ConvertTimestampToLLONG(datetime));
	}
	
	LONGLONG TimeDiffLocalAndUTC()
	{
		LONGLONG diff_secs;
		struct tm *timeInfo; 
		time_t secs, local_secs, utc_secs;

		time(&secs);

		// Current time in UTC 
		// Remember that localtime/gmtime overwrite same location 
		timeInfo = localtime(&secs); 
		local_secs = mktime(timeInfo);

		timeInfo = gmtime(&secs);
		utc_secs = mktime(timeInfo); 

		diff_secs = LONGLONG(local_secs - utc_secs);		
		return (diff_secs);
	}
	
	LONGLONG DateTimeSQL2Unix(CString szDateTime)
	{
		LONGLONG tUTCTime;
		struct tm tmTimeInfo;		
		char year[5] = {0}, month[3] = {0}, day[3] = {0};
		char hour[3] = {0}, minute[3] = {0}, second[3] = {0};
		char c;
		
		if (szDateTime.GetLength() == 0) {
			return (0);
		}

		// DateTime has format: YYYY-MM-DD HH:mm:ss
		sscanf(DMCUtil::ToString(szDateTime).c_str(), "%4s%c%2s%c%2s%c%2s%c%2s%c%2s", year, &c, month, &c, day, &c, hour, &c, minute, &c, second);
	
		// Set input time: XMPP time is UTC
		memset(&tmTimeInfo, 0, sizeof(struct tm));

		tmTimeInfo.tm_year	= atoi(year) - 1900;		// tm_year: years since 1900
		tmTimeInfo.tm_mon	= atoi(month) - 1;			// tm_mon: months since January, 0-11
		tmTimeInfo.tm_mday	= atoi(day);
		tmTimeInfo.tm_hour	= atoi(hour);
		tmTimeInfo.tm_min	= atoi(minute);
		tmTimeInfo.tm_sec	= atoi(second);

		if (tmTimeInfo.tm_year < 0 || tmTimeInfo.tm_mon < 0) {
			return (0);
		}
		
		// Convert to time_t: input of mktime MUST be LOCAL TIME struct
		// it returns UTC time, -1 on error
		tUTCTime = (LONGLONG)mktime(&tmTimeInfo) + TimeDiffLocalAndUTC();
		
		return (tUTCTime);
	}

	CString DateTimeUnix2SQL(LONGLONG llDateTimeUnix)
	{
		CString szFormat;
		CString szDate1, szDate2;
		
		time_t rawtime;		
		struct tm *timeInfo;		
		char buffer[128] = {0};
		
		if (llDateTimeUnix < 0) {
			return _T("");
		}		
		
		// The input date
		rawtime = (time_t) llDateTimeUnix;
		
		// To local time struct
		timeInfo	= localtime(&rawtime);		
		szFormat	= DTIME_SQL;
		
		// Format
		strftime(buffer, sizeof(buffer), ToString(szFormat).c_str(), timeInfo);		
			
		return (CString(buffer));
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	void UnixTimeToFileTime(time_t t, LPFILETIME pft)
	{
		// Note that LONGLONG is a 64-bit value
		LONGLONG ll;
		
		ll = Int32x32To64(t, 10000000) + SECONDS_DIFF_BETWEEN_UNIXUTC_AND_FILETIME*10000000;
		pft->dwLowDateTime	= (DWORD)ll;
		pft->dwHighDateTime = (DWORD)(ll >> 32);
	}
	
	void UnixTimeToSystemTime(time_t t, LPSYSTEMTIME pst)
	{
		FILETIME ft;
		
		UnixTimeToFileTime(t, &ft);
		FileTimeToSystemTime(&ft, pst);
	}

	time_t GMTToLocalTime()
	{
		TIME_ZONE_INFORMATION tzinfo;
		DWORD dwStandardDaylight;
		long bias;
		
		dwStandardDaylight = GetTimeZoneInformation(&tzinfo);
		bias = tzinfo.Bias;
		
		if (dwStandardDaylight == TIME_ZONE_ID_STANDARD)
			bias += tzinfo.StandardBias;
		
		if (dwStandardDaylight == TIME_ZONE_ID_DAYLIGHT)
			bias += tzinfo.DaylightBias;
		
		return (- bias * 60);
	}

	int	CompareTimeOnlyDate(const CTime &t1, const CTime &t2)
	{
		int y1, y2;

		y1 = t1.GetYear();
		y2 = t2.GetYear();
		if (y1 > y2)	return (1);
		if (y1 < y2)	return (-1);

		y1 = t1.GetMonth();
		y2 = t2.GetMonth();
		if (y1 > y2)	return (1);
		if (y1 < y2)	return (-1);

		y1 = t1.GetDay();
		y2 = t2.GetDay();
		if (y1 > y2)		return (1);
		if (y1 < y2)		return (-1);

		return (0);
	}

	int	CompareTimeOnlyTime(const CTime &t1, const CTime &t2)
	{
		int h1, h2;

		h1 = t1.GetHour();
		h2 = t2.GetHour();
		if (h1 > h2)		return (1);
		if (h1 < h2)		return (-1);

		h1 = t1.GetMinute();
		h2 = t2.GetMinute();
		if (h1 > h2)	return (1);
		if (h1 < h2)	return (-1);

		h1 = t1.GetSecond();
		h2 = t2.GetSecond();
		if (h1 > h2)	return (1);
		if (h2 < h1)	return (-1);

		return (0);
	}

	int	CompareTimeOnlyDate(const COleDateTime &t1, const COleDateTime &t2)
	{
		int y1, y2;

		y1 = t1.GetYear();
		y2 = t2.GetYear();
		if (y1 > y2)	return (1);
		if (y1 < y2)	return (-1);

		y1 = t1.GetMonth();
		y2 = t2.GetMonth();
		if (y1 > y2)	return (1);
		if (y1 < y2)	return (-1);

		y1 = t1.GetDay();
		y2 = t2.GetDay();
		if (y1 > y2)		return (1);
		if (y1 < y2)		return (-1);

		return (0);
	}

	int	CompareTimeOnlyTime(const COleDateTime &t1, const COleDateTime &t2)
	{
		int h1, h2;

		h1 = t1.GetHour();
		h2 = t2.GetHour();
		if (h1 > h2)		return (1);
		if (h1 < h2)		return (-1);

		h1 = t1.GetMinute();
		h2 = t2.GetMinute();
		if (h1 > h2)	return (1);
		if (h1 < h2)	return (-1);

		h1 = t1.GetSecond();
		h2 = t2.GetSecond();
		if (h1 > h2)	return (1);
		if (h2 < h1)	return (-1);

		return (0);
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CString LoadIDString(const UINT uText)
	{
		CString szText;
		
		// Load IDS string in string table
		szText.LoadString(uText);
		
		return (szText);
	}

	BOOL IsInStringArray(const CString input, const CStringArray& strArray)
	{
		CString existedStr;

		for (int i = 0; i < strArray.GetSize(); i++) {
			existedStr = strArray.GetAt(i);
			if (input.Compare(existedStr) == 0) {
				return (TRUE);
			}
		}

		return (FALSE);
	}

	std::string CreateMessageSessionID(std::string jid)
	{
		return (jid + DMCUtil::ToString(CreateLLONGSessionID()));
	}

	CString CreateMUCRoomJID(CString szMUCService, CString szRoomName, CString szRoomNick)
	{
		CString szRoomJID;

		szRoomJID  = szRoomName;
		szRoomJID += _T("@");
		szRoomJID += szMUCService;
		szRoomJID += _T("/");
		szRoomJID += szRoomNick;

		return (szRoomJID);
	}

	CString CreateAlertMailSessionJID(CString szKey/* = _T("")*/)
	{
		CString szSessionJID;

		if (szKey == _T("")) {
			szSessionJID = (_T("mail/") + DMCUtil::ToCString(CreateLLONGSessionID()));
		} else {
			szSessionJID = (_T("mail/") + szKey);
		}

		return (szSessionJID);
	}

	int		CreateRandomNumber(int iMin, int iMax)
	{	
		int random;
		int r1, r2, select;

		// Set new rand.
		srand((UINT)time(0));

		// Calculate new random number
		if (iMax <= RAND_MAX) {
			random = rand() % (iMax - iMin +1) + iMin;

		} else {
			r1 = rand() % (RAND_MAX - iMin + 1) + iMin;		
			r2 = rand() % (iMax - RAND_MAX + 1) + RAND_MAX;

			// Random selection
			select = rand() % 2;
			random = (select == 0) ? r1 : r2;
		}	

		return (random);
	}

	//////////////////////////////////////////////////////////////////////////
	// FILE PATH
	//////////////////////////////////////////////////////////////////////////

	BOOL CheckFileExist(LPCTSTR szPath)
	{
		return (::GetFileAttributes(szPath) != INVALID_FILE_ATTRIBUTES);
	}
	
	BOOL IsDirectory(LPCTSTR szPath)
	{
		DWORD dwAttrib = ::GetFileAttributes(szPath);
		
		return ((dwAttrib != INVALID_FILE_ATTRIBUTES) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}
	
	BOOL GetFileSize(CString szFilePath, ULONG &iFileSize)
	{	
		CFileException ex;
		CFile file;
		BOOL bRet = FALSE;

		try {

			// Open file.
			bRet = file.Open(szFilePath, CFile::modeRead | CFile::shareDenyNone, &ex);
			if (bRet == FALSE) {
				_cprintf("Error opening file, cause: %s.\n", DMCUtil::GetFileExErrorMsg(&ex).c_str());
				return (FALSE);
			}

			// Get file size.
			iFileSize = (ULONG)file.GetLength();

			// Close file.
			file.Close();

		} catch (CFileException* e) {
			_cprintf("Open file failed. cause: %s.\n", DMCUtil::GetFileExErrorMsg(e, TRUE).c_str());
		}

		return (bRet);
	}
	
	CString GetFileName(CString szFilePath)
	{
		CString szFileName;
		int lastSlashIndex, lastSlashIndexTemp, fileNameLength;
		
		// Truncate file name
		szFilePath.TrimLeft(_T(" "));
		szFilePath.TrimRight(_T(" "));		
		szFileName = szFilePath;

		// Get last index of "."
		lastSlashIndex		= szFilePath.ReverseFind('/');
		lastSlashIndexTemp	= szFilePath.ReverseFind('\\');

		if ((lastSlashIndexTemp > 0) && (lastSlashIndexTemp > lastSlashIndex) && (lastSlashIndexTemp < szFilePath.GetLength())) {
			lastSlashIndex = lastSlashIndexTemp;
		}

		if ((lastSlashIndex > 0) && (lastSlashIndex < szFilePath.GetLength() - 1) ) {
			fileNameLength	= szFilePath.GetLength() - 1 - lastSlashIndex;
			szFileName		= szFileName.Right(fileNameLength);
		}
		
		return (szFileName);
	}
	
	CString GetFileNameNoExt(CString filePath)
	{
		CString fileName;
		int lastPointIndex;

		// Get file name
		fileName = GetFileName(filePath);
		
		// Get last index of "."
		lastPointIndex = fileName.ReverseFind('.');
		
		if ((lastPointIndex >= 0) && (lastPointIndex < fileName.GetLength() - 1)) {	
			fileName = fileName.Left(lastPointIndex);
		}

		return (fileName);
	}

	CString GetFileExtension(CString szFileName)
	{
		CString fileExtension = _T("");
		int lastPointIndex;
		int extensionLength;
		
		// Get last index of "."
		lastPointIndex = szFileName.ReverseFind('.');
		
		if ((lastPointIndex >= 0) && (lastPointIndex < szFileName.GetLength() - 1)) {
			extensionLength = szFileName.GetLength() - 1 -  lastPointIndex;
			
			fileExtension = szFileName;
			fileExtension = fileExtension.Right(extensionLength);
		}
		
		return (fileExtension);
	}
	
	CString GetFolderPath(CString szFilePath)
	{
		CString szFileName;
		int lastSlashIndex, lastSlashIndexTemp, fileNameLength;
		
		// Truncate file name
		szFilePath.TrimLeft(_T(" "));
		szFilePath.TrimRight(_T(" "));		
		szFileName = szFilePath;
		
		// Get last index of "\"
		lastSlashIndex		= szFilePath.ReverseFind('//');
		lastSlashIndexTemp	= szFilePath.ReverseFind('\\');
		if (szFilePath.Right(1) == "\\" || szFilePath.Right(1) == "/") {
			szFilePath = szFilePath.Left(szFilePath.GetLength() - 1);
		}
		
		if ((lastSlashIndexTemp > 0) && (lastSlashIndexTemp > lastSlashIndex) && (lastSlashIndexTemp < szFilePath.GetLength())) {
			lastSlashIndex = lastSlashIndexTemp;
		}
		
		if ((lastSlashIndex > 0) && (lastSlashIndex < szFilePath.GetLength() - 1) ) {
			fileNameLength = szFilePath.GetLength() - 1 - lastSlashIndex;
			szFileName = szFileName.Right(fileNameLength);
		}
		
		return (szFileName);
	}
	
	std::string	GetFileName(std::string szFilePath)
	{
		return (DMCUtil::ToString(DMCUtil::GetFileName(ToCString(szFilePath))));
	}

	std::wstring GetFileNameW(LPCWSTR szFilePath)
	{
		std::wstring wszFileName = _T("");
		const WCHAR *p;
		
		// Validate info.
		if (wcslen(szFilePath) == 0) {
			return (wszFileName);
		}
		
		// Find the last '\' character.
		p = wcsrchr(szFilePath, '\\');
		if (p) {
			p++;
			wszFileName = p;
		}
		
		return (wszFileName);
	}

	std::string	GetFileNameNoExt(std::string szFilePath)
	{
		return (DMCUtil::ToString(DMCUtil::GetFileNameNoExt(ToCString(szFilePath))));
	}

	std::string	GetFileExtension(std::string szFileName)
	{
		return (DMCUtil::ToString(DMCUtil::GetFileExtension(ToCString(szFileName))));
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CString GetModuleFileNameEx()
	{
		TCHAR lpFilename[MAX_PATH];
		int iRet;

		// No specified module.
		// This function gets the file name of the executable file of the current process.
		iRet = ::GetModuleFileName(NULL, lpFilename, MAX_PATH);		
		if (iRet == 0 || iRet == MAX_PATH) {
			return _T("");
		}
		
		CString szModuleFileName(lpFilename);

	//	_cprintf("GetModuleFileNameEx: %s\n", ToString(szModuleFileName).c_str());
		return szModuleFileName;
	}

	CString GetLatestFile(CString szPath, CString szFileExt, CStringArray& listFile)
	{
		CFileFind fileFind;
		CTime compareTime, createTime;
		CString szLatestFilePath, szFilePath;
		BOOL bFind;
		
		// Verify input data
		if (szPath == _T("")) {
			return _T("");
		}
		
		// Initialize
		compareTime = 0; // CTime::GetCurrentTime();	
		szFilePath.Format(_T("%s\\*.%s"), szPath, szFileExt);	
		
		// Find specified file in the directory
		bFind = fileFind.FindFile(szFilePath);
		while (bFind) {

			// Find the next
			bFind = fileFind.FindNextFile();

			// Skip if the current found is directory or has name "." or ".."
			if (fileFind.IsDirectory() || fileFind.IsDots()) {
				continue;
			}
			
			// Compare created time
			fileFind.GetCreationTime(createTime);
			if (createTime > compareTime) {
				compareTime			= createTime;
				szLatestFilePath	= fileFind.GetFilePath();
			}

			// Add file that found to list
			listFile.Add(fileFind.GetFilePath());
		}

		// Close finder file.
		fileFind.Close();
		
		return (szLatestFilePath);
	}

	CString GetDirFromFullPath(CString szFilePath)
	{
		CString szDirPath;				
		int iNameIndex, iSlashIndex;		
		
		// Trim space all.
		szFilePath.TrimLeft(_T(" "));
		szFilePath.TrimRight(_T(" "));
		
		// Get position of last slash character.
		iSlashIndex	= szFilePath.ReverseFind(_T('\\'));
		iNameIndex	= szFilePath.GetLength() - 1 - iSlashIndex;

		// Get directory path.
		szDirPath = szFilePath.Left(szFilePath.GetLength() - iNameIndex - 1);		
		return (szDirPath);
	}

	std::string GetDirFromFullPath(std::string szFilePath)
	{
		CString szDirPath;

		szDirPath = GetDirFromFullPath(DMCUtil::ToCString(szFilePath));

		return (DMCUtil::ToString(szDirPath));
	}

	// http://stackoverflow.com/questions/524137/get-icons-for-common-file-types
	// You can get them from the shell by calling SHGetFileInfo() along with the SHGFI_USEFILEATTRIBUTES flag - this flag allows the routine to work without requiring the filename passed in to actually exist, so if you have a file extension just make up a filename, append the extension, and pass it in.
	// 	
	// By combining other flags, you'll be able to retrieve:
	// 	
	// A large or small icon as determined by the system configuration: SHGFI_ICON|SHGFI_LARGEICON or SHGFI_ICON|SHGFI_SMALLICON
	// A large or small icon as determined by the shell configuration: SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SHELLICONSIZE or SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SHELLICONSIZE
	// The index of the icon in the shell's image list along with the appropriate image list: SHGFI_SYSICONINDEX
	// The path and filename of the actual module where the icon is stored (along with the icon index in that module): SHGFI_ICONLOCATION

	HICON GetFileIcon(CString szExt, BOOL bIconSize, BOOL bIsDirectory /*= FALSE*/)
	{	
		return (CFileTypeIcon::GetIconFromPath(_T(".") + szExt, bIconSize, bIsDirectory));		
	}

	CString CreateFolderOfFilePath(const CString szFilePath)
	{
		CString szFileName;
		CString szSubFolder;
		int pos;
		
		pos = szFilePath.FindOneOf((LPCTSTR)"\\");
		pos = szFilePath.Find((LPCTSTR)"\\", pos + 2);
		
		while (pos != -1) {

			szSubFolder = szFilePath.Left(pos);
			
			try	{		
				::CreateDirectory(szSubFolder, NULL); // Archive/messages

			} catch (CException* e) {
				e->ReportError(); // Shows what's going wrong 
			}
			
			szFileName = szFilePath.Mid(pos + 1);		
			pos = szFilePath.Find((LPCTSTR)"\\", pos + 2);
		}
		
		return (szFileName);
	}
	
	BOOL CreateFile(CString szFilePath)
	{
		CStdioFile file;
		
		// Create compact file path
		CreateFolderOfFilePath(szFilePath);
		
		// If file isn't existent.
		if (file.Open((LPCTSTR)szFilePath, 
			CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone) == FALSE) {
			return (FALSE);
		}
		
		// Close file.
		file.Close();
		return (TRUE);
	}

	BOOL DeleteFile(CString szFilePath, bool bAskReadOnly/* = true*/)
	{
		CFileOperation fo;
		BOOL ret = FALSE;
		
		fo.SetAskIfReadOnly(bAskReadOnly);
		if (szFilePath != _T("") && fo.CanDelete(szFilePath)) {
			ret = fo.Delete(szFilePath);
		}

		return (ret);
	}

	void ClearFileInFolder(CString szFolderPath, CStringArray &arrExceptFileTitles)
	{
		CString szPath;
		CFileFind hFile;
		BOOL bFound;
		
		szPath = szFolderPath + _T("\\*.*");
		bFound = hFile.FindFile(szPath);
		while (bFound) {
			bFound = hFile.FindNextFile();
			if (hFile.IsDots() || hFile.IsDirectory()) {
				continue;
			}
			
			// Skip current file title
			if (DMCUtil::IsExistInStringArray(hFile.GetFileTitle(), arrExceptFileTitles)) {
				continue;
			}

			DMCUtil::DeleteFile(hFile.GetFilePath(), false);
		}
	}

	BOOL IsExistInStringArray(CString szName, CStringArray &arrNames)
	{
		for (int i = 0; i < arrNames.GetSize(); i++) {
			if (arrNames.GetAt(i) == szName) {
				return (TRUE);
			}
		}

		return (FALSE);
	}

	CString CreateTempFile(CString szPrefixName, CString szTempDir /*= _T("")*/)
	{
		HANDLE hTempFile = INVALID_HANDLE_VALUE;
		BOOL fSuccess  = FALSE;
		DWORD dwRetVal = 0;
		UINT uRetVal   = 0;
	
		TCHAR szTempFileName[MAX_PATH];  
		TCHAR lpTempPathBuffer[MAX_PATH];

		if (szTempDir == _T("")) {

			//  Gets the temp path environment string (no guarantee it's a valid path).
			dwRetVal = ::GetTempPath(MAX_PATH,				// length of the buffer
								     lpTempPathBuffer);		// buffer for path 
			if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
				return (_T(""));
			}
			
			// Generates a temporary file name. 
			uRetVal = ::GetTempFileName(lpTempPathBuffer,		// directory for tmp files
									    szPrefixName,			// temp file name prefix 
									    0,						// create unique name 
									    szTempFileName);		// buffer for name
			if (uRetVal == 0) {
				return (_T(""));
			}

		} else {

			// Generates a temporary file name. 
			uRetVal = ::GetTempFileName(szTempDir,				// directory for tmp files
									    szPrefixName,			// temp file name prefix 
									    0,						// create unique name 
									    szTempFileName);		// buffer for name
			if (uRetVal == 0) {
				return (_T(""));
			}
		}
		
		// Creates the new file to write to for the upper-case version.
		hTempFile = ::CreateFile((LPTSTR) szTempFileName,		// file name 
								 GENERIC_WRITE,					// open for write 
								 0,								// do not share 
								 NULL,							// default security 
								 CREATE_ALWAYS,					// overwrite existing
								 FILE_ATTRIBUTE_NORMAL,			// normal file 
								 NULL);							// no template 
		if (hTempFile == INVALID_HANDLE_VALUE) { 
			return (_T(""));
		}

		// Close handle
		if (!CloseHandle(hTempFile)) {
			return (_T(""));
		}

		CString szTempFilePath(szTempFileName);
		return (szTempFilePath);
	}

	std::string	CreateTempFile(std::string szPrefixName, std::string szTempDir /*= ""*/)
	{
		CString szTempFilePath;

		szTempFilePath = CreateTempFile(DMCUtil::ToCString(szPrefixName), DMCUtil::ToCString(szTempDir));

		return (DMCUtil::ToString(szTempFilePath));
	}

	int CreateFolderPath(const CString szFullFolderPath)
	{
		CreateFolderOfFilePath(szFullFolderPath + _T("\\"));
		return (0);
	}

	BOOL DeleteFolder(CString szFolderPath, bool bAskReadOnly/* = true*/)
	{
		CFileOperation fo;
		BOOL ret = FALSE;
		
		fo.SetAskIfReadOnly(bAskReadOnly);
		if (szFolderPath != _T("") && fo.CanDelete(szFolderPath)) {
			ret = fo.Delete(szFolderPath);
		}
		
		return (ret);
	}

	BOOL ClearFolder(CString szFolderPath, bool bAskReadOnly/* = true*/)
	{
		CFileOperation fo;
		BOOL ret = FALSE;
		
		fo.SetAskIfReadOnly(bAskReadOnly);
		if (szFolderPath != _T("")) {
			ret = fo.Clear(szFolderPath);
		}
		
		return (ret);
	}

	BOOL ReadFile(CString szFilePath, std::string& szOutData)
	{
		CFileException ex;
		CFile file;
		BOOL fsOpened;
		
		char* buffer = NULL;
		int nRead, bufSize = 1024;
		
		// Step 1: Open file.			
		fsOpened = file.Open(szFilePath, 
							 CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary, &ex);
		
		if (fsOpened == FALSE) {
			// Complain if an error happened
			// no need to delete the ex object			
			return (FALSE);
		}
		
		// Step 2: Read file to std::string.
		buffer = (char*)calloc(bufSize + 1, sizeof(char));
		if (buffer == NULL) {
			return (FALSE);
		}
		
		do {
			nRead = file.Read(buffer, bufSize);
			if (nRead == 0) {
				break;
			}
			
			szOutData.append(buffer, nRead);
			
		} while (nRead > 0);
		
		// Release buffer.
		free(buffer);
		file.Close();

		return (TRUE);
	}

	BOOL ReadFile(CString szFilePath, CString& szOutData)
	{
		std::string szData;
		BOOL ret;

		ret = ReadFile(szFilePath, szData);
		szOutData = DMCUtil::ToCString(szData);

		return (ret);
	}

	BOOL WriteFile(std::string szInData, CString szFilePath)
	{
		CFileException ex;
		CFile file;
		BOOL fsOpened;
		
		// Step 0: Open file
		fsOpened = file.Open(szFilePath, 
							 CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite | CFile::typeBinary, &ex);
		
		if (fsOpened == FALSE) {
			// Complain if an error happened
			// no need to delete the ex object
			return (FALSE);
		}
		
		// Step 2: Write std::string to file.
		file.Write(szInData.data(), (UINT)szInData.length());
		file.Close();

		return (TRUE);
	}

	BOOL WriteFile(CString szInData, CString szFilePath)
	{
		return (WriteFile(DMCUtil::ToString(szInData), szFilePath));
	}

	std::string GetFileExErrorMsg(CFileException* e, BOOL bDelete /*= FALSE*/)
	{
		CString szMsg;
		TCHAR szError[1024];		

		szMsg = _T("");

		if (e) {
			e->GetErrorMessage(szError, 1024);
			szMsg.Format(_T("%s. Cause: %d"), szError, e->m_cause);

			if (bDelete) {
				e->Delete();
			}
		}

		return (ToString(szMsg));
	}

	DWORD GetListOfFileInDir(CString szDirPath, CStringArray &fileList) 
	{
		WIN32_FIND_DATA ffd;
		CString szDir, szFilePath;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwError = 0;
		
		// Prepare string for use with FindFile functions.  First, copy the
		// string to a buffer, then append '\*' to the directory name.
		szDir  = szDirPath;
		szDir += _T("\\*");
		
		// Find the first file in the directory.
		hFind = FindFirstFile(szDir, &ffd);
		if (hFind == INVALID_HANDLE_VALUE) {			
			return dwError;
		} 
		
		// List all the files in the directory with some info about them.
		do {
			// Make full path.
			szFilePath = szDirPath + _T("\\") + ffd.cFileName;
			_cprintf("Folder: %s\n", DMCUtil::ToString(szFilePath).c_str());

			// Check type of file.
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// Nothing to do.
				
			} else {
				fileList.Add(szFilePath);
			}
			
		} while (FindNextFile(hFind, &ffd) != 0);
		
		dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES) {
			_cprintf("FindFirstFile\n");
		}
		
		FindClose(hFind);
		return dwError;
	}

	//////////////////////////////////////////////////////////////////////////
	// WORDWRAP
	//////////////////////////////////////////////////////////////////////////

	int FindCharIndex(int startIndex, CString input, int findCharsLength, TCHAR* findChars)
	{
		int result;
		int stringLength;
		TCHAR inputChar;
		int findIndex;

		stringLength = input.GetLength();
		if (input.IsEmpty() || findCharsLength <= 0 || startIndex < 0 || startIndex >= stringLength) {
			return (-1);
		}
		
		result = -1;
		for (int i = 0; i < findCharsLength; i++) {
			inputChar = findChars[i];
			findIndex  = input.Find(inputChar,startIndex);
			if ((result == -1) || ((findIndex >= 0) && (findIndex < result) && (findIndex < stringLength))) {
				result = findIndex;
			}
		}
		return (result);
	}
	
	CString GetBreakedPart(CString input, int maxIndex)
	{
		if (input.IsEmpty()) {
			return (input);

		} else {
			return (input.Mid(0, maxIndex));
		}
	}
	
	CString GetDataAtIndex(CString szData, CString szSeparator, int iIndex) 
	{
		CStringArray DataArray;
		CString szResult = _T("");

		DMCUtil::SplitString(szData, szSeparator, DataArray);
		if (DataArray.GetSize() > iIndex) {
			szResult = DataArray[iIndex];
		}

		return (szResult);
	}

	int		SplitString(const CString szInput, const CString szDelimiter, CStringArray& DataArray) 
	{
		// Will return by reference an array of CStrings that are sub strings of
		// xstrInput. The return of the function is the number of substrings found.

		CArray<int, int> PosArray;
		CString szSubStr;
		int nPos = -1, nNewPos = -1;
		int nDelimLen, nInputLen, nPosSize;		
		
		// Validate delimiter.
		if (szDelimiter == _T("")) {
			DataArray.Add(szInput);
			return ((int)DataArray.GetSize());
		}

		// Get info.
		nDelimLen = szDelimiter.GetLength();
		nInputLen = szInput.GetLength();		
		DataArray.RemoveAll();

		// Find now.
		nNewPos = szInput.Find(szDelimiter);	
		if (nNewPos < 0) {
			DataArray.Add(szInput);
			return ((int)DataArray.GetSize());
		}

		// First one.
		PosArray.Add(0);

		while (nNewPos >= nPos) {

			// Position is include nDelimLen.
			nPos = nNewPos + nDelimLen;

			// Add to position array.
			PosArray.Add(nPos);

			// Find next position.
			nNewPos = szInput.Find(szDelimiter, nPos);
		}
		
		// Split string base on above position list.
		nPosSize = (int)PosArray.GetSize();
		for (int n = 1; n < nPosSize; n++) {
			szSubStr = szInput.Mid(PosArray[n - 1], PosArray[n] - nDelimLen - PosArray[n - 1]);
			DataArray.Add(szSubStr);
		}

		// Add the last sub string to array.
		szSubStr = szInput.Mid(PosArray[nPosSize - 1], nInputLen - PosArray[nPosSize - 1]);
		DataArray.Add(szSubStr);
		
		return ((int)DataArray.GetSize());
	}
	
	void	SplitStringEx(CStringArray& LetterArray, CString szInput, int iDelimeterLength, TCHAR* pzDelimeter, BOOL bExtractSplit)
	{
		CString szResult, szSubString, szRemainString;
		int iSplitIndex, iStartIndex, iInputLength;
		
		if (szInput.IsEmpty()) {
			return;
		}
		
		iSplitIndex		= 0;
		iStartIndex		= 0;
		szResult		= _T("");		
		iInputLength	= szInput.GetLength();
		szRemainString	= szInput;
		
		while (!szRemainString.IsEmpty()) {

			iSplitIndex = FindCharIndex(0, szRemainString, iDelimeterLength, pzDelimeter);
			if (iSplitIndex == -1) {
				szSubString		= szRemainString;
				szRemainString	= _T("");
				
			} else {
				if (bExtractSplit) {
					szSubString = szRemainString.Mid(0, iSplitIndex - iStartIndex);					
				} else {
					szSubString = szRemainString.Mid(0, iSplitIndex - iStartIndex + 1);
				}
				
				szRemainString = szRemainString.Mid(iSplitIndex + 1, szRemainString.GetLength() - iSplitIndex);
			}

			LetterArray.Add(szSubString);
		}
	}

	int CStringFindCount(CString& szData, LPCTSTR szPattern)
	{
		if (szData.IsEmpty() || szPattern == _T("")) {
			return (0);
		}
		
		int nFind = -1;
		int nCount = 0;
		
		while ((nFind = szData.Find(szPattern, nFind + 1)) != -1) {
			nCount++;
		}
		
		return nCount;
	}

	void GetNiceUnit(UINT n, UINT *div, CString *unit)
	{
		UINT gb = 1024 * 1024 * 1024;
		UINT mb = 1024 * 1024;
		UINT kb = 1024;

		if (n >= gb) {
			*div = gb;
			*unit = _T("GB");
		}
		else if (n >= mb) {
			*div = mb;
			*unit = _T("MB");
		}
		else if (n >= kb) {
			*div = kb;
			*unit = _T("KB");
		}
		else {
			*div = 1;
			*unit = _T("B");
		}
	}
	
	CString RoundedNumber(UINT n, UINT div)
	{
		CString result = _T("");
		bool decimal = false;

		if (div >= 1024) {
			div /= 10;
			decimal = true;
		}

		UINT x = (UINT)(n / div);

		if (decimal) {
			double f = (double)x;
			f /= 10;
			result.Format(_T("%.1f"), f);

		} else {
			result.Format(_T("%d"), x);
		}

		return result;
	}

	CString GetRateView(UINT size)
	{
		CString result;
		UINT div;
		CString unit;

		result = _T("0B");
		if (size > 0) {
			GetNiceUnit(size, &div, &unit);
			result = RoundedNumber(size, div) + unit;
		}

		return result;
	}

	LPSTR	WC2MB(LPCWSTR szInName, ULONG nCodePage)
	{
		if (!szInName[0])
			return NULL;
		
		int nResult = WideCharToMultiByte(nCodePage, WC_COMPOSITECHECK,  
										  szInName, -1, NULL, 0, NULL, NULL);
		
		if (nResult == 0 && nCodePage != CP_ACP) {
			nResult = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK,  
										  szInName, -1, NULL, 0, NULL, NULL);
			if (nResult > 0)
				nCodePage = CP_ACP;
		}
		
		if (nResult <= 0)
			return NULL;
		
		CHAR* szOutName = (LPSTR)malloc((nResult + 1));
		
		int nRet = WideCharToMultiByte(nCodePage, WC_COMPOSITECHECK, 
									   szInName, -1, szOutName, nResult + 1, NULL, NULL);
		
		if (nRet == 0) {
			
			DWORD nError = GetLastError();
			
			free(szOutName);
			szOutName = NULL;
		}
		
		return szOutName;
	}
	
	CString ToCStringFromChar(char* val)
	{
		CString cstring;
		
		cstring = CString(val);
		return (cstring);
	}
	
	void	DMCAssert(BOOL condition)
	{
		#ifdef _DEBUG
			ASSERT(condition);
		#else

		#endif
	}
	
	void	DMCVerify(BOOL condition)
	{
		#ifdef _DEBUG
			VERIFY(condition);
		#else
			
		#endif
	}

	CString	ExecuteExternalProg(CString szExeName, CString szArguments, CString szExeDir)
	{
		CString szExecute, szOutput;
		SECURITY_ATTRIBUTES secattr; 
		HANDLE rPipe, wPipe;
		STARTUPINFO sInfo; 
		PROCESS_INFORMATION pInfo;
		BOOL res;
		
		// Execute string
		szOutput  = _T("");
		szExecute = szExeName + _T(" ") + szArguments;
		_cprintf("ExecuteExternalFile: szExecute: %s\n", DMCUtil::ToString(szExecute).c_str());
		
		ZeroMemory(&secattr, sizeof(secattr));
		secattr.nLength = sizeof(secattr);
		secattr.bInheritHandle = TRUE;
		
		// Create pipes to write and read data
		if (CreatePipe(&rPipe, &wPipe, &secattr, 0) == FALSE) {
			_cprintf("ExecuteExternalFile: Can't create pipes to write and read data\n");
			return (szOutput);
		}
		
		ZeroMemory(&sInfo,sizeof(sInfo));	
		ZeroMemory(&pInfo,sizeof(pInfo));
		sInfo.cb = sizeof(sInfo);
		sInfo.dwFlags = STARTF_USESTDHANDLES;
		sInfo.hStdInput = NULL; 
		sInfo.hStdOutput = wPipe; 
		sInfo.hStdError = wPipe;
		
		// Create the process here.	
		res = CreateProcess(0, szExecute.GetBuffer(szExecute.GetLength()), 0, 0, 
							TRUE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 0, 
							szExeDir, &sInfo, &pInfo);
		CloseHandle(wPipe);
		
		if (res == FALSE) {
			_cprintf("ExecuteExternalFile: Can't create the process.\n");
			return (szOutput);
		}
		
		// Now read the output pipe here.	
		char buf[100];
		DWORD reDword; 
		CString szTemp;
		
		do {			
			res = ::ReadFile(rPipe, buf, 100, &reDword, 0);
			szTemp = buf;
			szOutput += szTemp.Left(reDword);
			
		} while (res);
		
		_cprintf("ExecuteExternalFile: szOutput: %s\n", DMCUtil::ToString(szOutput).c_str());
		return szOutput;
	}
	
	HRESULT	CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink, LPCWSTR lpszIcon, LPCWSTR lpszDesc)
	{ 
		HRESULT hres; 
		IShellLink* psl; 

		// Initialize COM
		hres = CoInitialize(NULL);
		if (!SUCCEEDED(hres)) {
			return (S_FALSE);
		}

		// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
		// has already been called.
		hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
		if (SUCCEEDED(hres)) { 

			IPersistFile* ppf;

			// Set the path to the shortcut target and add the description.
			// lpszPathObj -> path file we want to create shortcut
			psl->SetPath(lpszPathObj); 
			psl->SetDescription(lpszDesc); 
			psl->SetIconLocation(lpszIcon, 0);

			// Query IShellLink for the IPersistFile interface, used for saving the 
			// shortcut in persistent storage. 
			hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
			if (SUCCEEDED(hres)) {

				// Save the link by calling IPersistFile::Save. 
				hres = ppf->Save(lpszPathLink, TRUE);
				if (!SUCCEEDED(hres)) {
					_cprintf("CreateShortCut() failed, error \n");
				}
				ppf->Release(); 
			} 
			psl->Release(); 
		}

		// Call CoUninitialize()
		CoUninitialize();

		return (hres);
	}

	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
	typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

	// http://msdn.microsoft.com/en-us/library/ms724429%28v=vs.85%29.aspx
	// http://msdn.microsoft.com/en-us/library/ms724451%28v=vs.85%29.aspx
	BOOL	IsWindowsXPorLater()
	{
		OSVERSIONINFO osvi;
		BOOL bIsWindowsXPorLater;
		
		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		
		GetVersionEx(&osvi);
		
		bIsWindowsXPorLater = ((osvi.dwMajorVersion > 5) || ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1)));
		
		if (bIsWindowsXPorLater) {
			_cprintf("The system is WindowXP or Later version.\n");

		} else {
			_cprintf("The system is before Window XP version.\n");
		}

		return (bIsWindowsXPorLater);
	}

	// http://msdn.microsoft.com/en-us/library/bb762181%28v=vs.85%29.aspx
	// http://support.microsoft.com/kb/310294
	CString GetWindowAllUserAppDataDir()
	{
		TCHAR szPath[MAX_PATH];
		
		if (SUCCEEDED(SHGetFolderPath(NULL,
									  CSIDL_COMMON_APPDATA/*|CSIDL_FLAG_CREATE*/, 
									  NULL, 
									  SHGFP_TYPE_CURRENT, 
									  szPath))) 
		{
			CString csPath(szPath);

			_cprintf("GetWindowAllUserAppDataDir: %s\n", DMCUtil::ToString(csPath).c_str());
			return (csPath);
		}

		_cprintf("GetWindowAllUserAppDataDir: failed\n");
		return _T("");
	}

	CString GetWindowCurUserDocumentDir()
	{
		TCHAR szPath[MAX_PATH];
		
		if (SUCCEEDED(SHGetFolderPath(NULL,
									  CSIDL_PERSONAL /*|CSIDL_FLAG_CREATE*/, 
									  NULL, 
									  0, 
									  szPath))) 
		{
			CString csPath(szPath);
			
			_cprintf("GetWindowCurUserDocumentDir: %s\n", DMCUtil::ToString(csPath).c_str());
			return (csPath);
		}
		
		_cprintf("GetWindowCurUserDocumentDir: failed\n");
		return _T("");
	}
	
	int	CompareVersion(CString szCurVersion, CString szNewVersion, int iLevel)
	{
		CStringArray xarrCurVersion;
		CStringArray xarrNewVersion;
		int ret;

		SplitString(szCurVersion, _T("."), xarrCurVersion);
		SplitString(szNewVersion, _T("."), xarrNewVersion);

		if (xarrCurVersion.GetSize() != iLevel || xarrNewVersion.GetSize() != iLevel) {
			return (-1);
		}

		for (int i = 0; i < 3; i++) {
			ret = CompareINT(ToINT(xarrCurVersion.GetAt(i)), ToINT(xarrNewVersion.GetAt(i)));
			if (ret != 0) {
				break;
			}
		}

		return (ret);
	}

	int	CompareINT(int a, int b)
	{
		if (a < b) {
			return (-1);
		} else if (a == b) {
			return (0);
		} else {
			return (1);
		}
	}

	int StrCompare(const char* str1, const char* str2)
	{
		int diff;

		if (str1 == NULL || str2 == NULL) {
			diff = 1;
		} else {
			diff = strcmp(str1, str2);
		}

		return diff;
	}

	std::string ConvertRGBtoHex(int num) 
	{
		static string hexDigits = "0123456789ABCDEF";
		string rgb;

		for (int i = (3 * 2) - 1; i >= 0; i--) {
			rgb += hexDigits[((num >> i*4) & 0xF)];
		}

		return rgb;		
	}
	
	std::string ConvertRGBtoHex(int r, int g, int b) {
		// The maximum value for a single byte is 0xFF.
		int rgbNum = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
		
		return ConvertRGBtoHex(rgbNum);
	}
	
	std::string ConvertRGBtoHex( COLORREF color )
	{
		return ConvertRGBtoHex(GetRValue(color), GetGValue(color), GetBValue(color));
	}
		
	BOOL UpdateString(CString& szSrc, CString szDes)
	{
		if (szSrc != szDes) {
			szSrc = szDes;
			return (TRUE);
		}

		return (FALSE);
	}

	BOOL UpdateInt(int& iSrc, int iDes)
	{
		if (iSrc != iDes) {
			iSrc = iDes;
			return (TRUE);
		}
		
		return (FALSE);
	}
	
	CString GetCurrentDateTime(CString szFormat)
	{
		CString szRes;

		CTime nowTime(DMCUtil::GetCurrentSystemTime());
		szRes = (nowTime.Format(szFormat));

		return (szRes);
	}

	CString Concat(CStringArray& arr, BOOL bQuote /*= FALSE*/ , CString szDelimiter /*= _T(",")*/)
	{
		CString szRes;
		CString szItem;
		int i;
		
		if (arr.GetSize() == 0) {
			return szRes;
		}
	
		szRes = _T("");
		
		for (i = 0; i < (arr.GetSize()-1); i ++) {
			if (bQuote) {
				szItem = _T("'") + arr[i] + _T("'");
			} else {
				szItem = arr[i];
			}
			
			szRes += szItem + szDelimiter;
		}

		// The last item
		i = (int)arr.GetSize() - 1;

		if (bQuote) {
			szItem = _T("'") + arr[i] + _T("'");
		} else {
			szItem = arr[i];
		}

		szRes += szItem;
		
		return szRes;
	}

	//////////////////////////////////////////////////////////////////////////
	// Escape and relax HTML.
	// Ref: Gloox lib (tag.h,.cpp)
	//////////////////////////////////////////////////////////////////////////

	static const char escape_chars[] = { '&', '<', '>', '\'', '"', '<', '>',
	'\'', '"', '<', '>', '<', '>', '\'', '"', '<', '>', '<', '>', '\'', '"' };

	static const std::string escape_seqs[] = { "amp;", "lt;", "gt;", "apos;",
	"quot;", "#60;", "#62;", "#39;", "#34;", "#x3c;", "#x3e;", "#x3C;",
	"#x3E;", "#x27;", "#x22;", "#X3c;", "#X3e;", "#X3C;", "#X3E;", "#X27;",
	"#X22;" };

	static const unsigned nb_escape = sizeof(escape_chars)/sizeof(char);
	static const unsigned escape_size = 5;

	const std::string Escape( std::string esc )
	{
		for (unsigned val, i = 0; i < esc.length(); ++i) {
			for (val = 0; val < escape_size; ++val) {
				if (esc[i] == escape_chars[val]) {
					esc[i] = '&';
					esc.insert(i+1, escape_seqs[val]);
					i += (unsigned)escape_seqs[val].length();
					break;
				}
			}
		}

		return esc;
	}

	CString Escape(CString what)
	{
		std::string szData;

		szData	= ToString(what);
		szData	= Escape(szData);

		return (DMCUtil::ToCString(szData));
	}

	/*
	 * When a sequence is found, do not repack the string directly, just set
	 * the new symbol and mark the rest for delectation (0).
	 */
	const std::string Relax( std::string esc )
	{
		const unsigned int l = (unsigned int)esc.length();
		unsigned int p = 0;
		unsigned int i = 0;

		for (unsigned int val; i < l; ++i) {
			if (esc[i] != '&')
				continue;

			for( val = 0; val < nb_escape; ++val ) {
				if ((i + escape_seqs[val].length() <= l) &&
					!strncmp(esc.data() + i + 1, escape_seqs[val].data(), escape_seqs[val].length()))
				{
					esc[i] = escape_chars[val];
					for (p=1; p <= escape_seqs[val].length(); ++p)
						esc[i+p] = 0;
					i += p-1;
					break;
				}
			}
		}
		
		if (p) {
			for (p = 0, i = 0; i < l; ++i) {
				if (esc[i] != 0) {
					if (esc[p] == 0) {
						esc[p] = esc[i];
						esc[p+1] = 0;
					}
					++p;
				}
			}
			esc.resize( p );
		}

		return esc;
	}

	CString Relax(CString what)
	{
		std::string szData;
		
		szData	= ToString(what);
		szData	= Relax(szData);
		
		return (DMCUtil::ToCString(szData));
	}

	CString FixBirthDay(CString szBirthDay, CString szFormat)
	{
		CString szResult;

		// Update birthday format
		szResult = szBirthDay;
		if (szBirthDay.GetLength() > szFormat.GetLength() ) {
			szResult = szBirthDay.Left(szFormat.GetLength());
		}

		return (szResult);
	}

	BOOL IsValidBirthDay(CString szBirthDay)
	{
		CString szResult;

		if (szBirthDay.IsEmpty()) {
			return (FALSE);
		}

		if (szBirthDay == _T("0000-00-00") || 
			szBirthDay == _T("1970-01-01")) {
			return (FALSE);
		}

		return (TRUE);
	}

	BOOL IsBirthDayToday(CString szBirthDay)
	{
		CStringArray strArr;
		CTime nowTime(DMCUtil::GetCurrentSystemTime());

		// Get birthday info.
		DMCUtil::SplitString(szBirthDay, _T("-"), strArr);
		if (strArr.GetSize() < 3) {
			return (FALSE);
		}

		// Only compare month and day.
		return ((nowTime.GetMonth() == DMCUtil::ToINT(strArr.GetAt(1))) &&
				(nowTime.GetDay()   == DMCUtil::ToINT(strArr.GetAt(2))));
	}

	BOOL CreateFolderWithEveryOneACL(LPCTSTR lpPath)
	{
		if (::CreateDirectory(lpPath, NULL) == FALSE && ::GetLastError() != ERROR_ALREADY_EXISTS) {
			return (FALSE);
		}

		return (SetFileSecurityWithEveryOneACL(lpPath));
	}

	BOOL CreateFileWithEveryOneACL(LPCTSTR lpPath, BOOL bSetPerm /*= TRUE*/)
	{	
		HANDLE hFile = ::CreateFile(lpPath, GENERIC_ALL, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE && ::GetLastError() != ERROR_ALREADY_EXISTS) {
			return (FALSE);
		}

		if (hFile != INVALID_HANDLE_VALUE) {
			::CloseHandle(hFile);
		}

		return (bSetPerm ? SetFileSecurityWithEveryOneACL(lpPath) : TRUE);
	}

	BOOL SetFileSecurityWithEveryOneACL(HANDLE &hFile)
	{	
		SECURITY_DESCRIPTOR* pSD = NULL;
		ACL *pOldDACL = NULL, *pNewDACL = NULL;
		BOOL bRet = FALSE;
		DWORD dwError;

		// Get security info of this handle, after that add EveryOne perm.
		dwError = ::GetSecurityInfo(hFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, (void**)&pSD);
		if (dwError != ERROR_SUCCESS) {
			return (FALSE);
		}

		// Step 2: Modify with new security.
		EXPLICIT_ACCESS ea[2];
		ZeroMemory(&ea, 2*sizeof(EXPLICIT_ACCESS));

		// Create a well-known SID for the Everyone group.
		SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
		PSID pEveryoneSID = NULL;
		::AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pEveryoneSID);

		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow Everyone read access to the key.
		ea[0].grfAccessMode			= GRANT_ACCESS;
		ea[0].grfAccessPermissions	= GENERIC_ALL;
		ea[0].grfInheritance		= CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
		ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[0].Trustee.ptstrName		= (LPTSTR)pEveryoneSID;
		
		// User group.
		SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
		PSID pUserSID = NULL;
		::AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS, 0, 0, 0, 0, 0, 0, &pUserSID);

		// The ACE will allow Everyone read access to the key.
		ea[1].grfAccessMode			= GRANT_ACCESS;
		ea[1].grfAccessPermissions	= GENERIC_ALL;
		ea[1].grfInheritance		= CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
		ea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
		ea[1].Trustee.ptstrName		= (LPTSTR)pUserSID;

		// Create a new ACL that contains the new ACEs.
		dwError = ::SetEntriesInAcl(2, ea, pOldDACL, &pNewDACL);		
		if (dwError != ERROR_SUCCESS || !pNewDACL) {
			goto _SetFileSecurityWithEveryOneACL_Exit_;
		}

		// Add the ACL to the security descriptor. 
		dwError = ::SetSecurityInfo(hFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);
		if (dwError != ERROR_SUCCESS) {
			goto _SetFileSecurityWithEveryOneACL_Exit_;
		}

		bRet = TRUE;
		
	_SetFileSecurityWithEveryOneACL_Exit_:
		if (pEveryoneSID) 	::FreeSid(pEveryoneSID);
		if (pUserSID) 		::FreeSid(pUserSID);
		if (pNewDACL)		::LocalFree(pNewDACL);
		if (pSD)			::LocalFree(pSD);

		// No free pOldDACL! B/c http://msdn.microsoft.com/en-us/library/windows/desktop/aa379283%28v=vs.85%29.aspx
		
		return (bRet);
	}
	 
	// http://msdn.microsoft.com/en-us/library/aa446595%28v=vs.85%29.aspx
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa379649%28v=vs.85%29.aspx
	BOOL SetFileSecurityWithEveryOneACL(LPCTSTR lpPath)
	{
		SECURITY_DESCRIPTOR* pOldSD = NULL;
		ACL *pOldDACL = NULL, *pNewACL = NULL;
		BOOL bRet = FALSE;
		DWORD dwError;

		// Step 1: Try open existing path.
		HANDLE hFile = ::CreateFile(lpPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
								    IsDirectory(lpPath) ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL,
								    NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			dwError = ::GetLastError();
			return (FALSE);
		}

		// Get current security.
		dwError = ::GetSecurityInfo(hFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, (void**)&pOldSD);
		if (dwError != ERROR_SUCCESS) {
			::CloseHandle(hFile);
			return (FALSE);
		}

		::CloseHandle(hFile);

		// Step 2: Modify with new security.
		EXPLICIT_ACCESS ea[2];
		ZeroMemory(&ea, 2*sizeof(EXPLICIT_ACCESS));

		// Create a well-known SID for the Everyone group.
		SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
		PSID pEveryoneSID = NULL;
		::AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pEveryoneSID);

		// Initialize an EXPLICIT_ACCESS structure for an ACE.
		// The ACE will allow Everyone read access to the key.
		ea[0].grfAccessMode			= GRANT_ACCESS;
		ea[0].grfAccessPermissions	= GENERIC_ALL;
		ea[0].grfInheritance		= CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
		ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[0].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[0].Trustee.ptstrName		= (LPTSTR)pEveryoneSID;

		// User group.
		SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
		PSID pUserSID = NULL;
		::AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS, 0, 0, 0, 0, 0, 0, &pUserSID);

		// The ACE will allow Everyone read access to the key.
		ea[1].grfAccessMode			= GRANT_ACCESS;
		ea[1].grfAccessPermissions	= GENERIC_ALL;
		ea[1].grfInheritance		= CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
		ea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
		ea[1].Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
		ea[1].Trustee.ptstrName		= (LPTSTR)pUserSID;

		// Create a new ACL that contains the new ACEs.
		dwError = ::SetEntriesInAcl(2, ea, pOldDACL, &pNewACL);
		if (dwError != ERROR_SUCCESS || !pNewACL) {
			goto _SetFolderSecurityWithEveryOneACL_Exit_;
		}

		// Initialize a security descriptor.  
		PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		::InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);

		// Add the ACL to the security descriptor. 
		if (::SetSecurityDescriptorDacl(pSD, TRUE, pNewACL, FALSE) == FALSE) {
			goto _SetFolderSecurityWithEveryOneACL_Exit_;
		}
		
		// Change the security attributes
		dwError = ::SetFileSecurity(lpPath, DACL_SECURITY_INFORMATION, pSD);
		if (dwError != ERROR_SUCCESS) {
			goto _SetFolderSecurityWithEveryOneACL_Exit_;
		}

		bRet = TRUE;

	_SetFolderSecurityWithEveryOneACL_Exit_:
		if (pEveryoneSID) 	::FreeSid(pEveryoneSID);
		if (pUserSID) 		::FreeSid(pUserSID);
		if (pNewACL) 		::LocalFree(pNewACL);
		if (pSD) 			::LocalFree(pSD);

		// No free pOldDACL! B/c http://msdn.microsoft.com/en-us/library/windows/desktop/aa379283%28v=vs.85%29.aspx
 		if (pOldSD)			::LocalFree(pOldSD);

		return (bRet);
	}

	BOOL	SaveBitmapFile(const char *szFilename, HBITMAP hBitmap)
	{
		HDC					hdc = NULL;
		FILE*				fp = NULL;
		LPVOID				pBuf = NULL;
		BITMAPINFO			bmpInfo;
		BITMAPFILEHEADER	bmpFileHeader;
		BOOL				bRet = TRUE;
		
		do {			
			hdc = GetDC(NULL);

			ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
			bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
			
			if (bmpInfo.bmiHeader.biSizeImage <= 0) {
				bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth * abs(bmpInfo.bmiHeader.biHeight) * (bmpInfo.bmiHeader.biBitCount + 7) / 8;
			}
			
			if ((pBuf = malloc(bmpInfo.bmiHeader.biSizeImage)) == NULL) {
				bRet = FALSE;
				break;
			}
			
			bmpInfo.bmiHeader.biCompression = BI_RGB;
			GetDIBits(hdc, hBitmap, 0, bmpInfo.bmiHeader.biHeight, pBuf, &bmpInfo, DIB_RGB_COLORS);	
			
			if ((fp=fopen(szFilename, "wb")) == NULL) {
				bRet = FALSE;
				break;
			}
			
			bmpFileHeader.bfReserved1	= 0;
			bmpFileHeader.bfReserved2	= 0;
			bmpFileHeader.bfSize		= sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
			bmpFileHeader.bfType		= 'MB';
			bmpFileHeader.bfOffBits		= sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
			
			fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
			fwrite(&bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);
			fwrite(pBuf, bmpInfo.bmiHeader.biSizeImage, 1, fp);
			
		} while (false);
		
		if (hdc) { ReleaseDC(NULL, hdc); }		
		if (pBuf) { free(pBuf);	}		
		if (fp) { fclose(fp); }
		
		return (bRet);
	}

	BOOL	EnableFlashEx(HWND hWnd, DWORD dwFlags) 
	{
		FLASHWINFO fwi;

		// ::FlashWindow() VS ::FlashWindowEx()
		//
		// ::FlashWindow()
		// Flashes the specified window one time. It does not change the active state of the window.
		// When a window flashes, it appears to change from inactive to active status. 
		// An inactive caption bar changes to an active caption bar; an active caption bar changes to an inactive caption bar.
		//
		// ::FlashWindowEx()
		// Flashes the specified window. It does not change the active state of the window.		

		if (!hWnd) {
			return (FALSE);
		}

		ZeroMemory(&fwi, sizeof(FLASHWINFO));

		fwi.cbSize		= sizeof(FLASHWINFO);
		fwi.dwFlags		= dwFlags;
		fwi.hwnd		= hWnd;
		fwi.uCount		= 86400;
		fwi.dwTimeout	= 1000;
		
		return (FlashWindowEx(&fwi));
	}
	
	void	ReleaseThread(HANDLE &hThread, DWORD dwMiliseconds/* = -1*/)
	{
		// Waiting for terminating thread.
		if (!hThread) {
			return;
		}
			
		// Thread may be suspended, so resume before shutting down.
		::ResumeThread(hThread);
		
		// Set default time out.
		if (dwMiliseconds == -1) {
			dwMiliseconds = RELEASE_THREAD_TIMEOUT;
		}
		
		// Terminal thread if it is timeout.
		if (::WaitForSingleObject(hThread, dwMiliseconds) == WAIT_TIMEOUT) {
			::TerminateThread(hThread, -RELEASE_THREAD_TIMEOUT);
		}

		// Close the handle and NULL it out
		::CloseHandle(hThread);
		hThread = NULL;
	}

	void	SetCurrentProcessPriority(DWORD dwPriorityClass)
	{
		DWORD dwError, dwPriClass;
		
		if (!SetPriorityClass(GetCurrentProcess(), dwPriorityClass)) {
			dwError = GetLastError();
			_cprintf("Failed to enter PRIORITY_CLASS: (%d)\n", dwError);
			
		} else {	
			// Display priority class	
			dwPriClass = GetPriorityClass(GetCurrentProcess());	
			_cprintf("Current priority class is 0x%x\n", dwPriClass);
		}
	}

	void RestoreDubuDiskPath(CString &szHTMLContent, CLogger *m_pLogger)
	{
		RestoreDubuDisk_iPath(szHTMLContent, m_pLogger);
		RestoreDubuDisk_ePath(szHTMLContent, m_pLogger);
	}

	void RestoreDubuDisk_iPath(CString &szHTMLContent, CLogger *m_pLogger)
	{
		CString szDiskLabel			= _T("");
		CString szDubuDiskLabel		= _T("");
		CString szCompanyLabel		= _T("");
		CString szDubuUserViewDir	= _T("");
		CString szComUserViewDir	= _T("");

		// Get DubuDisk User Directory and Company User Directory from registry.
		if (RegUtil::GetDDC_iLoginInfo(szDiskLabel, szDubuDiskLabel, szCompanyLabel)) {

			// Parse to get szDubuUserDir/szComUserDir.
			szDubuUserViewDir = DMCUtil::GetDataAtIndex(szDubuDiskLabel, DEFAULT_SEPARATOR_NAME, 1);
			szComUserViewDir  = DMCUtil::GetDataAtIndex(szCompanyLabel, DEFAULT_SEPARATOR_NAME, 1);

			// DubuDisk was installed on this machine. szDiskLabel has to be valid.

		} else {
			// DubuDisk was not installed on this machine.
			m_pLogger->LogDebug(FILEINFO, "RestoreDubuDiskPath: DubuDisk-i: Cannot open registry to read LoginInfo. DubuDisk was not installed on this machine.\n");
		}

		// Disk label
		if (szDiskLabel == _T("")) {
			szDiskLabel = DDC_i_DEFAULT_DISK_LABEL;	
		}
		szDiskLabel += _T("\\");

		// DubuDisk root folder
		if (szDubuUserViewDir != _T("")) {
			szDubuUserViewDir = szDiskLabel + szDubuUserViewDir;
		} else {
			szDubuUserViewDir = szDiskLabel + DDC_i_DEFAULT_DUBUDISK_ROOT;
		}

		// Company root folder
		if (szComUserViewDir != _T("")) {
			szComUserViewDir = szDiskLabel + szComUserViewDir;
		} else {
			szComUserViewDir = szDiskLabel + DDC_i_DEFAULT_COMPANY_ROOT;
		}

		// Restore DubuDisk-i path
		szHTMLContent.Replace(DDC_i_DUBUDISK_ROOT_REPLACEMENT, szDubuUserViewDir);
		szHTMLContent.Replace(DDC_i_COMPANY_ROOT_REPLACEMENT, szComUserViewDir);
		szHTMLContent.Replace(DDC_i_DUBUDISK_REDIRECT_ROOT_REPLACEMENT, szDiskLabel);
	}
	
	void RestoreDubuDisk_ePath(CString &szHTMLContent, CLogger *m_pLogger)
	{
		CString szDiskLabel			= _T("");
		CString szDubuDiskLabel		= _T("");
		CString szCompanyLabel		= _T("");

		// Get DubuDisk User Directory and Company User Directory from registry.
		if (RegUtil::GetDDC_eLoginInfo(szDiskLabel, szDubuDiskLabel, szCompanyLabel)) {
			// DubuDisk was installed on this machine. szDiskLabel has to be valid.

		} else {
			// DubuDisk was not installed on this machine.
			m_pLogger->LogDebug(FILEINFO, "RestoreDubuDiskPath: DubuDisk-i: Cannot open registry to read LoginInfo. DubuDisk was not installed on this machine.\n");
		}

		// Disk name
		if (szDiskLabel == _T("")) {
			szDiskLabel = DDC_e_DEFAULT_DISK_LABEL;
		}
		szDiskLabel += _T("\\");

		// Restore DubuDisk-e path
		szHTMLContent.Replace(DDC_e_DUBUDISK_REDIRECT_ROOT_REPLACEMENT, szDiskLabel);
	}

	int	RestoreDubuDiskLink(CString szHref, CString &szResPath, int &iTypeDisk, CLogger *m_pLogger)
	{
		int iDDCStatus;

		// Detect DubuDisk-i link first
		iTypeDisk = 1;
		iDDCStatus = RestoreDubuDisk_iLink(szHref, szResPath, m_pLogger);
		if (iDDCStatus == DDC_STATUS_UNKNOWN) {

			// Detect DubuDisk-e link
			iTypeDisk = 2;
			iDDCStatus = RestoreDubuDisk_eLink(szHref, szResPath, m_pLogger);
		}

		return iDDCStatus;
	}

	int	RestoreDubuDisk_iLink(CString szHref, CString &szResPath, CLogger *m_pLogger)
	{
		CString szMountPoint		= _T("");
		CString szDiskLabel			= _T("");
		CString szDubuDiskLabel		= _T("");
		CString szCompanyLabel		= _T("");
		int iDDCStatus	= DDC_STATUS_UNKNOWN;
		
		// Reset szResPath value
		szResPath = _T("");

		// Validate input
		if (szHref.GetLength() == 0) {
			return (DDC_STATUS_UNKNOWN);
		}

		// Get DubuDisk User Directory and Company User Directory from registry.
		RegUtil::GetDDC_iLoginInfo(szDiskLabel, szDubuDiskLabel, szCompanyLabel);

		// Set default if it is not exist
		if (szDiskLabel == _T("")) {
			szDiskLabel = DDC_i_DEFAULT_DISK_LABEL;
		}
		
		// Is DubuDisk path?
		if (szHref.Left(szDiskLabel.GetLength()) == szDiskLabel) {
			
			// Get mount point
			if (RegUtil::GetDDC_iMountPoint(szMountPoint)) {
				if (szMountPoint.GetLength() > 2) {

					// DubuDisk is logged.
					iDDCStatus		= DDC_STATUS_LOGGED;
					szMountPoint	= szMountPoint.Left(2);
					szResPath		= szMountPoint + szHref.Mid(szDiskLabel.GetLength());
				//	m_pLogger->LogDebug(FILEINFO, "RestoreDubuDiskLink: DubuDisk logged, szResPath: %s\n", DMCUtil::ToString(szResPath).c_str());
					
				} else {
					// DubuDisk does not login.
					iDDCStatus = DDC_STATUS_LOGOFF;
					m_pLogger->LogDebug(FILEINFO, "RestoreDubuDiskLink: DubuDisk does not login. Mount point is invalid.\n");
				}

			} else {
				// Cannot access DubuDisk-i registry.
				// DubuDisk might not be installed.
				iDDCStatus = DDC_STATUS_NOT_INSTALLED;
				m_pLogger->LogDebug(FILEINFO, "RestoreDubuDiskLink: DubuDisk might not be installed.\n");
			}
		}
		// else iDDCStatus = DDC_STATUS_UNKNOWN

		return (iDDCStatus);
	}

	int	RestoreDubuDisk_eLink(CString szHref, CString &szResPath, CLogger *m_pLogger)
	{
		CString szMountPoint		= _T("");
		CString szDiskLabel			= _T("");
		CString szDubuDiskLabel		= _T("");
		CString szCompanyLabel		= _T("");
		int iDDCStatus	= DDC_STATUS_UNKNOWN;

		// Reset szResPath value
		szResPath = _T("");

		// Validate input
		if (szHref.GetLength() == 0) {
			return (DDC_STATUS_UNKNOWN);
		}

		// Get DubuDisk User Directory and Company User Directory from registry.
		RegUtil::GetDDC_eLoginInfo(szDiskLabel, szDubuDiskLabel, szCompanyLabel);

		// Set default if it is not exist
		if (szDiskLabel == _T("")) {
			szDiskLabel = DDC_e_DEFAULT_DISK_LABEL;
		}

		// Is DubuDisk path?
		if (szHref.Left(szDiskLabel.GetLength()) == szDiskLabel) {

			// Get mount point
			if (RegUtil::GetDDC_eMountPoint(szMountPoint)) {
				if (szMountPoint.GetLength() > 2) {

					// DubuDisk is logged.
					iDDCStatus		= DDC_STATUS_LOGGED;
					szMountPoint	= szMountPoint.Left(2);
					szResPath		= szMountPoint + szHref.Mid(szDiskLabel.GetLength());
					//	m_pLogger->LogDebug(FILEINFO, "RestoreDubuDiskLink: DubuDisk logged, szResPath: %s\n", DMCUtil::ToString(szResPath).c_str());

				} else {
					// DubuDisk does not login.
					iDDCStatus = DDC_STATUS_LOGOFF;
					m_pLogger->LogDebug(FILEINFO, "RestoreDubuDiskLink: DubuDisk does not login. Mount point is invalid.\n");
				}

			} else {
				// Cannot access DubuDisk-i registry.
				// DubuDisk might not be installed.
				iDDCStatus = DDC_STATUS_NOT_INSTALLED;
				m_pLogger->LogDebug(FILEINFO, "RestoreDubuDiskLink: DubuDisk might not be installed.\n");
			}
		}
		// else iDDCStatus = DDC_STATUS_UNKNOWN

		return (iDDCStatus);
	}

	BOOL IsDubuDiskPath(CString szData)
	{
		CString szMountPoint;
		BOOL bRet = FALSE;

		if (szData.GetLength() < 3) {
			return (FALSE);
		}

		// Get mount point
		if (RegUtil::GetDDC_iMountPoint(szMountPoint) == TRUE) {
			bRet = (IsDubuDiskPath(szData, szMountPoint));
		}

		if (!bRet && RegUtil::GetDDC_eMountPoint(szMountPoint) == TRUE) {
			bRet = (IsDubuDiskPath(szData, szMountPoint));
		}

		return (bRet);
	}

	BOOL IsDubuDiskPath(CString szData, CString szMountPoint)
	{
		CString szDisk;

		if (szMountPoint.GetLength() > 2) {

			// DubuDisk is logged
			szMountPoint = szMountPoint.Left(2) + _T("\\");
			szDisk		 = szData.Left(3);
			if (szDisk.CompareNoCase(szMountPoint) == 0) {

				// Check the full path is valid
				if (CheckFileExist(szData)) {
					return (TRUE);
				}
			}
		}

		return (FALSE);
	}

	CString RemoveBeginningSlash(CString str)
	{
		int pos = 0;
		
		// Remove slash at the beginning
		while (pos < str.GetLength()) {
			if (str.GetAt(pos) == '\\' || str.GetAt(pos) == '/') {
				pos++;
			} else {
				break;
			}
		}
		
		return (str.Mid(pos));
	}
	
	CString RemoveTrailingSlash(CString str)
	{
		int pos;

		// Remove slash at the trailing
		pos = str.GetLength() - 1;
		while (pos >= 0) {
			if (str.GetAt(pos) == '\\' || str.GetAt(pos) == '/') {
				pos--;
			} else {
				break;
			}
		}

		return (str.Left(pos + 1));
	}

	CString Convert2FilePath(CString szFilePath)
	{
		CString szBeReplaced	= _T("//");
		CString szReplaceWith	= _T("\\");	
		CString szResult		= szFilePath;
		
		if (szFilePath.GetLength() == 0) {
			return (szResult);
		}
		
		while (szResult.GetLength() > 0 && (szResult.Find(szBeReplaced, 0) >= 0)) {
			szResult.Replace(szBeReplaced, szReplaceWith);
		}
		
		return (szResult);
	}

	BOOL BrowseAndSelectFilePath(CString szFilePath)
	{
		CString szCmdOption;
		
		szFilePath = Convert2FilePath(szFilePath);
		szCmdOption.Format(_T("/select, \"%s\""), szFilePath);
		
		if (!DMCUtil::CheckFileExist(szFilePath)) {
			return (FALSE);
		} else {
			ShellExecute(NULL, NULL, _T("explorer"), szCmdOption, NULL, SW_SHOWNORMAL);
			return (TRUE);
		}
	}

	BOOL Browse2FolderPath(CString szFolderPath)
	{
		szFolderPath = Convert2FilePath(szFolderPath);
		if (!DMCUtil::CheckFileExist(szFolderPath)) {
			return (FALSE);
		}
		
		ShellExecute(NULL, _T("open"),  szFolderPath, NULL, NULL, SW_SHOWNORMAL);
		return (TRUE);
	}

	//////////////////////////////////////////////////////////////////////////
	// Font utilities
	//////////////////////////////////////////////////////////////////////////

	void CopyFont(CFont *pSrcFont, CFont *pDesFont)
	{
		LOGFONT lf;

		if (!pSrcFont || !pDesFont) {
			return;
		}

		memset(&lf, 0, sizeof(lf));
		pSrcFont->GetLogFont(&lf);

		if (pDesFont->GetSafeHandle()){
			pDesFont->DeleteObject();
		}

		pDesFont->CreateFontIndirect(&lf);
	}

	//////////////////////////////////////////////////////////////////////////
	// Color utilities
	//////////////////////////////////////////////////////////////////////////

	COLORREF StrToColor(CString str)
	{
		if (str.GetLength() != 7) {
			return (0);
		}

		int iColor;
		CString str2 = str.Mid(5, 2) + str.Mid(3, 2) + str.Mid(1, 2);
		str2 = _T("0x") + str2;
		StrToIntEx(str2, STIF_SUPPORT_HEX, &iColor);

		return (COLORREF) iColor;
	}

	std::string ColorToStr(COLORREF color)
	{
		char str[10];
		sprintf(str, "#%06X", color);
		return std::string(str);
	}

	COLORREF TransparentColor(COLORREF clr, int alpha)
	{
		float fAlpha = (float) alpha/255;

		BYTE r = (BYTE) (((float) (1 - fAlpha) * 255) + fAlpha * GetRValue(clr));
		BYTE g = (BYTE) (((float) (1 - fAlpha) * 255) + fAlpha * GetGValue(clr));
		BYTE b = (BYTE) (((float) (1 - fAlpha) * 255) + fAlpha * GetBValue(clr));

		return (RGB(r, g, b));
	}

} // End of namespace
