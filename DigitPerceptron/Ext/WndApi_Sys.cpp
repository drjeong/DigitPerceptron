#include "pch.h"
#include "WndApi_Sys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef WIN32
#include <direct.h>    
#define GetCurrentDir _getcwd
#else    
#include <unistd.h>    
#define GetCurrentDir getcwd 
#endif

BOOL WINAPI WAPI_DeleteFile(CString strFilename)
{
	return (DeleteFile(strFilename));
}

CString WINAPI WAPI_GetModulePath()
{
	int nSlash = 0;
	TCHAR  szSlash = '\\';
	TCHAR  szFullPathName[_MAX_PATH];

	CString strAppName = _T("");
	CString strExeFileName = _T("");
	CString strInstalledFolder = _T("");
	if (::GetModuleFileName(AfxGetInstanceHandle(), szFullPathName, _MAX_PATH) )
	{
		strAppName = szFullPathName;
		nSlash = strAppName.ReverseFind(szSlash);
		strInstalledFolder = strAppName.Left(nSlash+1);
	}
	return strInstalledFolder;
}

CString WINAPI WAPI_GetCurrentPath()
{
	char cCurrentPath[FILENAME_MAX];
	GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));
	return CString(cCurrentPath);
}

void WINAPI WAPI_SetCurrentPath(CString path)
{
	WAPI_CheckDirectory(path);
	SetCurrentDirectory(path);
}

BOOL WINAPI WAPI_CheckDirectory(CString &szpPath)
{
	BOOL bSuccess = FALSE;
	CString szDrive, szTmp, szPath;
	szDrive = szpPath.Left(2);
	szTmp = szpPath.GetBuffer(szpPath.GetLength());
	szpPath.ReleaseBuffer();
	szPath = szDrive;

	while (szTmp.Find('\\') != -1)
	{
		szTmp.Delete(0, szTmp.Find('\\')+1);
		CString szFolder = szTmp;
		int pos, len;
		pos = szFolder.Find('\\');
		len = szFolder.GetLength();
		if (pos != -1)
			szFolder.Delete(pos, len-pos);
		szPath = szPath + ('\\') + szFolder;
		
		if ( CreateDirectoryEx(szDrive, szPath ,NULL) )
		{	// success to create folder (directory)
			bSuccess = TRUE;
//			break;
		}
	}
	return bSuccess;
}