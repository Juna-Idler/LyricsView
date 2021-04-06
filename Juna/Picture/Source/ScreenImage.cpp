
#include "../../Windows/WindowClass.hpp"
#include "../../Windows/Utility.h"
#include "../ScreenImage.h"
#include <new>
#include <windows.h>


namespace Juna {
namespace Picture {



bool ScreenImage::Initialize(int width,int height)
{
	Terminalize();

	HWND hwnd = static_cast<HWND>(Juna::Windows::GetMyWindowHandle());
	if (hwnd == NULL)
		return false;

	if (!Image.Initialize(width,height))
		return false;

	hWindow = hwnd;
	Juna::Windows::AdjustWindowClientRect(hwnd,width,height);

	return true;
}

void ScreenImage::Terminalize(void)
{
	Image.Terminalize();
	hWindow = 0;
}
void ScreenImage::Update(const Rect *rect)
{
	HWND hwnd = static_cast<HWND>(hWindow);
	HDC hdc = ::GetDC(hwnd);
	if (!hdc)
		return;
	Rect r;
	if (rect)
		r = *rect;
	else
		r.Set(0,0,Image.width(),Image.height());

	::BitBlt(hdc,r.x1,r.y1,r.width(),r.height(),
		static_cast<HDC>(Image.GetDC()),r.x1,r.y1,SRCCOPY);
	::ReleaseDC(hwnd,hdc);
}



namespace {
struct SUITPram
{
	HWND hwnd;
	HANDLE hevent;
	HDC hDC;
};
class ScreenWindow
{
	HDC hDC;
public:
	ScreenWindow(HDC hdc) : hDC(hdc) {}
protected:
	LRESULT WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CLOSE:
			return 0;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				::BeginPaint(hwnd,&ps);
				::BitBlt(ps.hdc,ps.rcPaint.left,ps.rcPaint.top,
						 ps.rcPaint.right - ps.rcPaint.left,
						 ps.rcPaint.bottom - ps.rcPaint.top,
						 hDC,ps.rcPaint.left,ps.rcPaint.top,
						 SRCCOPY);
				::EndPaint(hwnd,&ps);
			}
			return 0;
		}
		return ::DefWindowProc(hwnd,uMsg,wParam,lParam);
	}
};
}//	anonymous namespace

DWORD WINAPI ScreenUIThread(LPVOID lpParameter)
{
	SUITPram &param = *static_cast<SUITPram*>(lpParameter);

	Juna::WindowClass<ScreenWindow> window(param.hDC);
	Juna::WindowClass<ScreenWindow>::RParam rparam;
	Juna::WindowClass<ScreenWindow>::CParam cparam;
	rparam.hIcon;
	rparam.hIconSm;
	cparam.dwStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_DLGFRAME;
	param.hwnd = window.RegisterCreate(::GetModuleHandle(NULL),
						__TEXT("JunaPictureScreenWindow"),__TEXT("Screen"),
						rparam,cparam);
	::SetEvent(param.hevent);
	if (!param.hwnd)
		return false;
	::ShowWindow(param.hwnd,SW_SHOWDEFAULT);

	window.GetMessageLoop();

	return 0;
}

bool ScreenImage2::Initialize(int width,int height,bool make_private_screen)
{
	Terminalize();
	Private = make_private_screen;

	if (!Image.Initialize(width,height))
		return false;

	HWND hwnd;
	if (make_private_screen)
	{
		DWORD id;
		SUITPram param;
		param.hDC = static_cast<HDC>(Image.GetDC());
		param.hevent = ::CreateEvent(NULL,true,false,NULL);
		HANDLE hthread = ::CreateThread(NULL,0,ScreenUIThread,&param,0,&id);
		if (!hthread)
		{
			::CloseHandle(param.hevent);
			Terminalize();
			return false;
		}
		::CloseHandle(hthread);
		::WaitForSingleObject(param.hevent,INFINITE);
		::CloseHandle(param.hevent);
		if (!param.hwnd)
		{
			Terminalize();
			return false;
		}
		hwnd = param.hwnd;
	}
	else
	{
		hwnd = static_cast<HWND>(Juna::Windows::GetMyWindowHandle());
		if (!hwnd)
		{
			Terminalize();
			return false;
		}
	}

	hWindow = hwnd;
	Juna::Windows::AdjustWindowClientRect(hwnd,width,height);

	return true;
}

void ScreenImage2::Terminalize(void)
{
	Image.Terminalize();
	if (Private && hWindow)
		::DestroyWindow(static_cast<HWND>(hWindow));
	hWindow = 0;
}
void ScreenImage2::Update(const Rect *rect)
{
	HWND hwnd = static_cast<HWND>(hWindow);
	HDC hdc = ::GetDC(hwnd);
	if (!hdc)
		return;
	Rect r;
	if (rect)
		r = *rect;
	else
		r.Set(0,0,Image.width(),Image.height());

	::BitBlt(hdc,r.x1,r.y1,r.width(),r.height(),
			 static_cast<HDC>(Image.GetDC()),r.x1,r.y1,SRCCOPY);
	::ReleaseDC(hwnd,hdc);
}

}//namespace Picture
}//namespace Juna

