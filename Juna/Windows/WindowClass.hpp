
#ifndef Juna_WindowClassTemplate_Hpp
#define Juna_WindowClassTemplate_Hpp

#include <windows.h>

namespace Juna {

/*
class base_WindowClass
{
protected:
	LRESULT WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};
*/


template<class base_Class>
class WindowClass : public base_Class
{
public:
	struct RParam
	{
		UINT style;
		int cbClsExtra,cbWndExtra;
		HICON hIcon;
		HCURSOR hCursor;
		HBRUSH hbrBackground;
		LPCTSTR lpszMenuName;
		HICON hIconSm;

		RParam(void)
			: style(CS_HREDRAW | CS_VREDRAW),
			  cbClsExtra(0),cbWndExtra(0),hIcon(0),hCursor(::LoadCursor(NULL,IDC_ARROW)),
			  hbrBackground(0),lpszMenuName(0),hIconSm(0)
		{}
	};
	struct CParam
	{
		DWORD dwExStyle,dwStyle;
		int x,y,nWidth,nHeight;
		HWND hWndParent;
		HMENU hMenu;

		CParam(void)
			: dwExStyle(0),
			  dwStyle(WS_VISIBLE | WS_OVERLAPPEDWINDOW),
			  hWndParent(0),hMenu(0),
			  x(CW_USEDEFAULT),y(CW_USEDEFAULT),nWidth(CW_USEDEFAULT),nHeight(CW_USEDEFAULT)
		{}
	};

public:
	WindowClass(void) : base_Class() {}
	WindowClass(const base_Class &src) : base_Class(src) {}

	ATOM Register(
				UINT style,int cbClsExtra,int cbWndExtra,HINSTANCE hinst,
				HICON hIcon,HCURSOR hCursor,HBRUSH hbrBackground,
				LPCTSTR lpszMenuName,LPCTSTR lpszClassName,HICON hIconSm)
	{
		WNDCLASSEX wcex = {sizeof(WNDCLASSEX),style,initWindowProc,
						   cbClsExtra,cbWndExtra,hinst,
						   hIcon,hCursor,hbrBackground,
						   lpszMenuName,lpszClassName,hIconSm};
		return ::RegisterClassEx(&wcex);
	}
	ATOM Register(HINSTANCE hinst,LPCTSTR lpszClassName,const RParam &param = RParam())
	{
		WNDCLASSEX wcex = {sizeof(WNDCLASSEX),param.style,initWindowProc,
						   param.cbClsExtra,param.cbWndExtra,hinst,
						   param.hIcon,param.hCursor,param.hbrBackground,
						   param.lpszMenuName,lpszClassName,param.hIconSm};
		return ::RegisterClassEx(&wcex);
	}

	HWND Create(DWORD dwExStyle,
				LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,
				int x,int y,int nWidth,int nHeight,
				HWND hWndParent,HMENU hMenu,HINSTANCE hinst)
	{
		return ::CreateWindowEx(dwExStyle,lpClassName,lpWindowName,dwStyle,
								x,y,nWidth,nHeight,hWndParent,hMenu,hinst,this);
	}
	HWND Create(HINSTANCE hinst,LPCTSTR lpClassName,LPCTSTR lpWindowName,const CParam &param = CParam())
	{
		return ::CreateWindowEx(param.dwExStyle,lpClassName,lpWindowName,param.dwStyle,
								param.x,param.y,param.nWidth,param.nHeight,param.hWndParent,param.hMenu,hinst,this);
	}

	HWND RegisterCreate(HINSTANCE hinst,LPCTSTR lpClassName,LPCTSTR lpWindowName,
						const RParam &rparam,const CParam &cparam)
	{
		if (!Register(hinst,lpClassName,rparam))
			return 0;
		return Create(hinst,lpClassName,lpWindowName,cparam);
	}


//平凡なGetMessageループ
	static WPARAM GetMessageLoop(void)
	{
		while (1)
		{
			MSG msg;
			int r = ::GetMessage(&msg,NULL,0,0);
			if (r == 0 || r == -1)
				return msg.wParam;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

private:
	//初期化専用
	static LRESULT CALLBACK initWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		if (uMsg == WM_NCCREATE)
		{
			WindowClass *this_ = (WindowClass *)((LPCREATESTRUCT)lParam)->lpCreateParams;
			::SetWindowLongPtr(hwnd,GWL_USERDATA,(LONG_PTR)this_);
			::SetWindowLongPtr(hwnd,GWL_WNDPROC,(LONG_PTR)staticWindowProc);
			return this_->WindowProc(hwnd,uMsg,wParam,lParam);
		}
		return ::DefWindowProc(hwnd,uMsg,wParam,lParam);
	}

	//間接参照
	static LRESULT CALLBACK staticWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return ((WindowClass *)::GetWindowLongPtr(hwnd,GWL_USERDATA))->WindowProc(hwnd,uMsg,wParam,lParam);
	}

};

/*
class base_ModalDialogClass
{
protected:
	BOOL DialogProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
};
*/

template<class base_Class>
class ModalDialogClass : public base_Class
{
public:
	ModalDialogClass(void) : base_Class() {}
	ModalDialogClass(const base_Class &src) : base_Class(src) {}

	int CreateDialogBox(HINSTANCE hInstance,LPCTSTR lpTemplateName,HWND hWndParent)
	{
		return DialogBoxParam(hInstance,lpTemplateName,hWndParent,initDlgProc,(LPARAM)this);
	}
private:
	//初期化専用
	static BOOL CALLBACK initDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		if (uMsg == WM_INITDIALOG)
		{
			ModalDialogClass *this_ = (ModalDialogClass *)lParam;
			::SetWindowLongPtr(hDlg,DWL_USER,(LONG_PTR)this_);
			::SetWindowLongPtr(hDlg,DWL_DLGPROC,(LONG_PTR)staticDlgProc);
			return this_->DialogProc(hDlg,uMsg,wParam,lParam);
		}
		return FALSE;
	}

	//間接参照
	static BOOL CALLBACK staticDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return ((ModalDialogClass *)::GetWindowLongPtr(hDlg,DWL_USER))->DialogProc(hDlg,uMsg,wParam,lParam);
	}

};

}//namespace Juna

#endif
