#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>
#include <time.h>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#include "FileRotator.h"
#include "DCSMutex.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define FILEINFO					__FILE__, __LINE__

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define DEBUG_WITH_CONSOLE

//////////////////////////////////////////////////////////////////////////
// WARNING: This file use C/C++ standard.
// Giang.Nguyen
//////////////////////////////////////////////////////////////////////////

/**
 * Describes a log message's severity.
 */
enum DLogLevel
{
	DLogLevelInfo,                   /**< Info messages. */
	DLogLevelDebug,                  /**< Debug messages. */
	DLogLevelWarning,                /**< Non-critical warning messages. */
	DLogLevelError,                  /**< Critical, unrecoverable errors. */
	DLogLevelSQL					 /**< SQL. */
};

enum DLogMode
{
	DLogModeNone		= 0x00,
	DLogModeConsole		= 0x01,
	DLogModeEnableDebug = 0x02,
	DLogModeAll			= 0xFF
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CLogger : public CFileRotator
{
public:
	// Implement the logic here to instantiate the class for the first time by validating the 
	// member pointer. If member pointer is already pointing to some valid memory it means
	// that the first object is created and it should not allow for the next instantiation
	// so simply return the member pointer without calling its ctor. 
	static CLogger*		Instance();

	// Attributes
private:
	// A member pointer to itself which point to the firstly created object and this should be
	// returned if another object is instantiated that means the second object what you 
	// thought of created is nothing but the first Instance	
	static CLogger*		m_pInstance;

public:
	CLogger();
	~CLogger();
	
	void	ResetPrivateData();

public:
	bool	Initialize(const char* szLogPath, unsigned int mode);

	//////////////////////////////////////////////////////////////////////////
	// Write log without FILEINFO.
	//////////////////////////////////////////////////////////////////////////

	void	Log(const char* szFData, ...);
	void	LogNoPrefix(const char* szFData, ...);
	void	LogInfo(const char* szFData, ...);

	// Write SQL log without FILEINFO.
	void	LogSQL(const char* szFData, ...);

	//////////////////////////////////////////////////////////////////////////
	// Write log with FILEINFO.
	//////////////////////////////////////////////////////////////////////////

	void	LogDebug(const char* szFileName, UINT iLineNum, const char* szFData, ...);
	void	LogWarning(const char* szFileName, UINT iLineNum, const char* szFData, ...);
	void	LogError(const char* szFileName, UINT iLineNum, const char* szFData, ...);

protected:
	string	MakePrefixInfo(DLogLevel level, const char* szFormat, const char* szFileName, UINT iLineNum);
	void	WriteLog(string szFDataDate, va_list* args);

protected:
	CDCSMutex		 		m_cs;

	std::string				m_szLogPath;
	unsigned int			m_mode;

	FILE*					m_fcp;			// Write to console window.
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

inline bool IsTab(char c) 
{
	return (c == '\t');
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<typename T, typename P>
T RemoveIf(T beg, T end, P pred)
{
	T dest = beg;
	for (T itr = beg;itr != end; ++itr)
		if (!pred(*itr))
			*(dest++) = *itr;
	return dest;
}

#endif // #ifndef _LOGGER_H_
