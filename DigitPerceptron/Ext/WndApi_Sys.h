#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

BOOL WINAPI WAPI_DeleteFile(CString strFilename);
CString WINAPI WAPI_GetModulePath();
CString WINAPI WAPI_GetCurrentPath();
void WINAPI WAPI_SetCurrentPath(CString path);
BOOL WINAPI WAPI_CheckDirectory(CString &szpPath);