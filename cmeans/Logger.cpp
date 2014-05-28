#include "StdAfx.h"
#include "Logger.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define MAX_ROTATOR_FILE_SIZE						5*1024*1024				// 5MB
#define MAX_ROTATOR_FILE_COUNT						10

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Initialize pointer
CLogger* CLogger::m_pInstance = 0;

CLogger* CLogger::Instance() 
{
	if (m_pInstance == 0) // Is it the first call?
	{ 
		m_pInstance = new CLogger; // Create sole Instance
	}
	
	return (m_pInstance); // Address of sole Instance
}

CLogger::CLogger()
	: CFileRotator()
{
	m_fcp			= NULL;
	m_szLogPath		= "";
	m_mode			= DLogModeNone;
}

CLogger::~CLogger()
{
	// Reset data.
	ResetPrivateData();
}

void CLogger::ResetPrivateData()
{
	CDCSMutexAutoLocker csAutoLocker(&m_cs);

	// Close file rotator.
	CFileRotator::Close();
	
	// Close file pointer.
	if (m_fcp) {
		fclose(m_fcp);
		m_fcp = NULL;
	}
}

bool CLogger::Initialize(const char* szLogPath, unsigned int mode)
{
	CDCSMutexAutoLocker csAutoLocker(&m_cs);

	// Set info.
	m_szLogPath = szLogPath;
	m_mode		= mode;
	m_fcp		= NULL;

	// Initialize
#if defined(WIN32)

	if (m_mode & DLogModeConsole) {

		int hConHandle;	
		long lStdHandle;

		// Write log to console.
		// Redirect unbuffered STDOUT to the console	
		lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
		if (lStdHandle != -1) {

			hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);	
			if (hConHandle != -1) {

				m_fcp = _fdopen(hConHandle, "w");	
				if (m_fcp) {

					*stdout = *m_fcp;	
					setvbuf(stdout, NULL, _IONBF, 0);

					*stderr = *m_fcp;	
					setvbuf(stderr, NULL, _IONBF, 0);
				}
			}
		}
	}

#endif // #if defined(WIN32)

	// Write log to file.
	CFileRotator::Initialize(m_szLogPath.c_str(), MAX_ROTATOR_FILE_SIZE, MAX_ROTATOR_FILE_COUNT);
	if (!CFileRotator::Open()) {
		return (false);
	}

	return (true);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CLogger::Log(const char* szFData, ...)
{
	va_list args;
	string szFDataDate;

	if (m_mode & DLogModeEnableDebug) {
		va_start(args, szFData);
		szFDataDate = MakePrefixInfo(DLogLevelDebug, szFData, NULL, 0);
		WriteLog(szFDataDate, &args);
		va_end(args);
	}
}

void CLogger::LogNoPrefix(const char* szFData, ...)
{
	va_list args;
	string szFDataDate;
	
	if (m_mode & DLogModeEnableDebug) {
		va_start(args, szFData);
		szFDataDate = szFData;
		WriteLog(szFDataDate, &args);
		va_end(args);
	}
}

void CLogger::LogInfo(const char* szFData, ...)
{
	va_list args;
	string szFDataDate;
	
	va_start(args, szFData);
	szFDataDate = MakePrefixInfo(DLogLevelInfo, szFData, NULL, 0);
	WriteLog(szFDataDate, &args);
	va_end(args);
}

void CLogger::LogSQL(const char* szFData, ...)
{
	va_list args;
	string szFDataDate;
	
	if (m_mode & DLogModeEnableDebug) {
		va_start(args, szFData);
		szFDataDate  = MakePrefixInfo(DLogLevelSQL, szFData, NULL, 0);
		WriteLog(szFDataDate, &args);
		va_end(args);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CLogger::LogDebug(const char* szFileName, UINT iLineNum, const char* szFData, ...)
{
	va_list args;
	string szFDataDate;

	if (m_mode & DLogModeEnableDebug) {
		va_start(args, szFData);
		szFDataDate = MakePrefixInfo(DLogLevelDebug, szFData, szFileName, iLineNum);
		WriteLog(szFDataDate, &args);
		va_end(args);
	}
}

void CLogger::LogWarning(const char* szFileName, UINT iLineNum, const char* szFData, ...)
{
	va_list args;
	string szFDataDate;
	
	va_start(args, szFData);
	szFDataDate = MakePrefixInfo(DLogLevelWarning, szFData, szFileName, iLineNum);
	WriteLog(szFDataDate, &args);
	va_end(args);
}

void CLogger::LogError(const char* szFileName, UINT iLineNum, const char* szFData, ...)
{
	va_list args;
	string szFDataDate;
	
	va_start(args, szFData);
	szFDataDate = MakePrefixInfo(DLogLevelError, szFData, szFileName, iLineNum);
	WriteLog(szFDataDate, &args);
	va_end(args);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

string CLogger::MakePrefixInfo(DLogLevel level, const char* szFormat, const char* szFileName, UINT iLineNum)
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[128] = {0};

	string szNewFormat;
	const char* pos;

	time(&rawtime);	
	timeinfo = localtime(&rawtime);

	// 2011-03-09 PM 05:17:10:
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %p %I:%M:%S", timeinfo);

	// Date time info.
	szNewFormat = std::string(buffer);	

	// Log type
	switch (level) {
	case DLogLevelInfo:
		szNewFormat += ":INFO";
		break;

	case DLogLevelWarning:
		szNewFormat += ":WARN";
		break;

	case DLogLevelError:
		szNewFormat += ":ERROR";
		break;

	case DLogLevelSQL:
		szNewFormat += ":SQL";
		break;

	case DLogLevelDebug:
	default:
		szNewFormat += ":DEBUG";
		break;
	}	

	// File name info.
	if (szFileName) {

		szNewFormat += ": ";
		pos = strrchr(szFileName, '\\');
		if (pos == NULL) {
			szNewFormat += szFileName;

		} else {
			pos++;
			szNewFormat += pos;
		}
	}

	// Line number info.
	if (iLineNum) {
		sprintf(buffer, ":%d", iLineNum);
		szNewFormat += std::string(buffer);
	}

	// Raw data
	szNewFormat += ": ";
	szNewFormat += szFormat;

	// Log type
	switch (level) {
	case DLogLevelSQL:
		szNewFormat.erase(RemoveIf(szNewFormat.begin(), szNewFormat.end(), IsTab), szNewFormat.end());
		szNewFormat += "\n";
		break;

	default:
		break;
	}

	return (szNewFormat);
}

void CLogger::WriteLog(string szFDataDate, va_list* args)
{
	CDCSMutexAutoLocker csAutoLocker(&m_cs);

	// 1. Write to console.
	if (m_fcp) {
		vfprintf(m_fcp, szFDataDate.c_str(), *args);
		fflush(m_fcp);
	}

	// 2. Write to log file.
	// 2.1. PrepareToWrite: validate file size, if required, rename existing file and reopen a new file.
	CFileRotator::PrepareToWrite();

	// 2.2. Write data to file.
	if (m_fp) {
		vfprintf(m_fp, szFDataDate.c_str(), *args);
		fflush(m_fp);
	}
}
