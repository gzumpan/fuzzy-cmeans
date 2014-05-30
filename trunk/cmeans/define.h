#ifndef FCM_DEFINE_H
#define FCM_DEFINE_H

static TCHAR BASED_CODE g_szFilter[]	= _T("Text Files (*.txt)|*.txt|")
										  _T("Worksheet Files (*.xls;*.csv)|*.xls;*.csv|")
										  _T("All Files (*.*)|*.*||");
const CString FILE_DATA_INFO			= _T("Instances : %d, Attributes : %d");

const CString FCM_DELIMITER				= _T(",");
const CString FCM_LOGPATH				= _T("D:\\log.txt");

#endif //FCM_DEFINE_H