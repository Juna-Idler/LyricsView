
#include "../Utility.h"

#include <windows.h>

namespace Juna {
namespace Windows {

bool DLLHandle::Load(const wchar_t *filename)
{
	hDll = ::LoadLibraryW(filename);
	return hDll != 0;
}
void DLLHandle::Free(void)
{
	if (hDll) ::FreeLibrary((HINSTANCE)hDll);
	hDll = 0;
}
void *DLLHandle::GetProcAdress(const char *proc_name)
{
	if (!hDll) return 0;
	return ::GetProcAddress((HINSTANCE)hDll,proc_name);
}


void *GetMyWindowHandle(void)
{
	HWND hwnd = NULL;
	{	//EnumThreadWindows‚Å’T‚·
		struct local {	//Enum‚Ì‚¨‹Ÿ‚É
			static BOOL CALLBACK ETWP(HWND hwnd,LPARAM lParam)
			{(*(HWND *)lParam) = hwnd;return FALSE;}
		};
		::EnumThreadWindows(::GetCurrentThreadId(),local::ETWP,(LPARAM)&hwnd);
	}
	if (hwnd == NULL)
	{	//EnumWindows + GetWindowThreadProcessId‚Å’T‚·
		struct local
		{
			static BOOL CALLBACK EWP(HWND hwnd,LPARAM lParam)
			{
				DWORD wpid;
				::GetWindowThreadProcessId(hwnd,&wpid);
				if (wpid == ::GetCurrentProcessId()) {
					(*(HWND *)lParam) = hwnd;
					return FALSE;
				}
				return TRUE;
			}
		};
		::EnumWindows(local::EWP,(LPARAM)&hwnd);
	}
	return hwnd;
}

void AdjustWindowClientRect(void *hWnd,int client_width,int client_height)
{
	HWND hwnd = static_cast<HWND>(hWnd);
	RECT wrect,crect;
	::GetWindowRect(hwnd,&wrect);
	::GetClientRect(hwnd,&crect);
	int ncwidth,ncheight;
	ncwidth = (wrect.right - wrect.left) - (crect.right - crect.left);
	ncheight = (wrect.bottom - wrect.top) - (crect.bottom - crect.top);

	::SetWindowPos(hwnd,NULL,0,0,
				   client_width + ncwidth,client_height + ncheight,
				   SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	::GetWindowRect(hwnd,&wrect);
	::GetClientRect(hwnd,&crect);
	int ncheight2 = (wrect.bottom - wrect.top) - (crect.bottom - crect.top);
	if (ncheight != ncheight2)
	{
		::SetWindowPos(hwnd,NULL,0,0,
					   client_width + ncwidth,client_height + ncheight2,
					   SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
		::GetWindowRect(hwnd,&wrect);
	}
	RECT drect;
	::GetClientRect(GetDesktopWindow(),&drect);
	int x = wrect.left - (wrect.right > drect.right ? wrect.right - drect.right : 0);
	int y = wrect.top - (wrect.bottom > drect.bottom ? wrect.bottom - drect.bottom : 0);
	x = x < 0 ? 0 : x;
	y = y < 0 ? 0 : y;
	::SetWindowPos(hwnd,NULL,x,y,0,0,
				   SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

}//namespace Windows
}//namespace Juna
