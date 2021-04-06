
#include <Windows.h>
#include <crtdbg.h>

#include <commctrl.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "Juna/Windows/WindowClass.hpp"


#include "Juna/Picture/DIBImage.h"
#include "Juna/Picture/Blit.h"

#include "Juna/Picture/Font.h"
//#include "Juna/Picture/ExpandedFont.h"

#include "Juna/Lyrics/LyricsViewer.h"

#include "TextFile.h"

#include "JunaLyricsMessage.h"



class WindowClass
{
private:
	HWND hWindow;

	UINT RegisterMessage;
	HANDLE hDataFileMapping;
	HANDLE hDataMutex;

	HANDLE hUserFileMapping;
	HANDLE hUserMutex;

	Juna::Picture::BufferedOutlineFont Font;
	Juna::Picture::BufferedOutlineFont RubyFont;


	Juna::Lyrics::LyricsViewer Viewer;
	Juna::Picture::DIBImage Image;

	JunaLyricsMessage Message;
	std::wstring FilePath;
	std::wstring Title;
	std::wstring Artist;
	std::wstring Album;
	std::wstring Genre;
	std::wstring Date;
	std::wstring Comment;

	TextFileReader Lyrics;

	DWORD StartTime;

	DWORD LastDrawTime;

public:
	WindowClass(void) : RegisterMessage(WM_NULL),
		hDataFileMapping(NULL), hDataMutex(NULL),
		hUserFileMapping(NULL), hUserMutex(NULL)
	{
		::timeBeginPeriod(1);
	}


	~WindowClass()
	{
		Terminalize();
		::timeEndPeriod(1);
	}

private:

	void Terminalize(void)
	{
		Message.Exit();
	}

	bool Initialize(void)
	{
		Terminalize();

		Message.EnterListener(hWindow);

		Juna::Picture::OutlineFont font;
		Juna::Picture::OutlineFont::InitParam fontparam;
//		fontparam.facename = L"ＭＳ Ｐゴシック";
		fontparam.charset = Juna::Picture::OutlineFont::CharSet::CharSetShiftJis;
		fontparam.pitch = Juna::Picture::OutlineFont::Pitch::PitchFixed;
		font.Initialize(32,fontparam,3);

		Font.Initialize(font);

		font.Initialize(16,fontparam,3);
		RubyFont.Initialize(font);

//		Viewer.ScrollTime = -1;
//		Viewer.ScrollFadeMax = 500;

//		Viewer.KaraokeCharFade = false;

//		Viewer.Alignment = Juna::Lyrics::SA_Center;
		Viewer.Parameter.HAlignment = Juna::Lyrics::HA_Right;
		Viewer.Parameter.HAlignment = Juna::Lyrics::HA_Center;

//		Viewer.RubyLineMode = Juna::Lyrics::RubyLyricsViewer::RLM_Force;
//		Viewer.RubyLineMode = Juna::Lyrics::RubyLyricsViewer::RLM_Ignore;

		Viewer.Parameter.LineSpace = 2;
		Viewer.Parameter.CharSpace = 0;
		Viewer.Parameter.LeftMargin = 50;
		Viewer.Parameter.RightMargin = 50;
		Viewer.Parameter.TopMargin = 0;
		Viewer.Parameter.BottomMargin = 0;
//		Viewer.CurrentLineYOffset = 0;
//*
		Viewer.Parameter.SetFont(&Font,&RubyFont);
//		Viewer.SetFont(&Font);
		Viewer.Parameter.CurrentTextColor.Set(255,255,255);
		Viewer.Parameter.OtherTextColor.Set(128,128,128);
		Viewer.Parameter.StandbyTextColor.Set(255,128,128);
		Viewer.Parameter.StandbyTextOutlineColor.Set(255,0,0);

/*/
//		Viewer.SetFont(&Font,&RubyFont,&EFont,&ERubyFont);
		Viewer.SetFont(&Font,&RubyFont);

		Viewer.OtherTextColor.Set(128,128,128);
		Viewer.OtherTextOutlineColor.Set(128,128,128);

		Viewer.CurrentTextColor.Set(255,255,255);
		Viewer.CurrentTextOutlineColor.Set(0,0,255);



//*/

//		Viewer.KaraokeCharFade = false;

		Viewer.Parameter.CurrentLineBackColor.Set(128,0,0,64);
//		Viewer.CurrentLineBack = true;


		::RECT rect;
		::GetClientRect(hWindow,&rect);
		Image.Initialize(rect.right,rect.bottom);

		return true;
	}

	void PlaybackEvent( JunaLyricsMessage::enumPlaybackEvent pbevent , unsigned long milisec )
	{
		switch ( pbevent )
		{
		case JunaLyricsMessage::PBE_Time:
			break;
		case JunaLyricsMessage::PBE_New:
			{
				Message.GetData(FilePath,Title,Artist,Album,Genre,Date,Comment);
				size_t ext = FilePath.find_last_of(L'.');
				if (ext == std::wstring::npos)
				{
					return;
				}
				std::wstring filename = FilePath.substr(0, ext);
				size_t pathoffset = filename.find( L"file://" );
				if ( pathoffset == 0 )
				{
					filename = filename.substr( 7 );
				}
				std::wstring lrcpath = filename + L".lrc";
				std::wstring krapath = filename + L".kra";
				if (!Lyrics.Open(krapath.c_str()))
				{
					Lyrics.Open(lrcpath.c_str());
				}
				Viewer.SetLyrics(Lyrics.GetString());
				StartTime = milisec;
				::SetTimer(hWindow,1,16,NULL);
			}
			break;
		case JunaLyricsMessage::PBE_Stop:
			::KillTimer(hWindow,1);
			break;
		case JunaLyricsMessage::PBE_Seek:
			{
				LastDrawTime = ::timeGetTime();
				StartTime = LastDrawTime - milisec;
//				if ( !timer1.Enabled )
				{
					Viewer.Update(LastDrawTime - StartTime);
					::InvalidateRect(hWindow,NULL,false);
				}
			}
			break;
		case JunaLyricsMessage::PBE_Pause:
			::KillTimer(hWindow,1);
			break;
		case JunaLyricsMessage::PBE_PauseCancel:
			{
				LastDrawTime = ::timeGetTime();
				StartTime = LastDrawTime - milisec;
				::SetTimer(hWindow,1,16,NULL);
			}
			break;
		}
	}


protected:
	LRESULT WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		JunaLyricsMessage::MessageParam mp;
		if (Message.TestMessage(mp,uMsg,wParam,lParam))
		{
			PlaybackEvent( mp.pb_event , mp.milisec );
			return 0;
		}


		switch (uMsg)
		{
		case WM_CREATE:
			{
				hWindow = hwnd;
				Initialize();
			}
			return 0l;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0l;

		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0l;

		case WM_PAINT:
			{
			PAINTSTRUCT ps;
			BeginPaint(hwnd,&ps);

			::BitBlt(ps.hdc,ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right - ps.rcPaint.left,ps.rcPaint.bottom - ps.rcPaint.top,
				(HDC)Viewer.GetCanvas().GetDC(),ps.rcPaint.left,ps.rcPaint.top,SRCCOPY);

			EndPaint(hwnd,&ps);
			}
			return 0l;

		case WM_SIZING:
			{
			const int min_width = 200;
			const int min_height = 200;

			LPRECT rect = (LPRECT)lParam;
			if (rect->right - rect->left < min_width)
				switch (wParam)
				{
				case WMSZ_LEFT:
				case WMSZ_TOPLEFT:
				case WMSZ_BOTTOMLEFT:
					rect->left = rect->right - min_width;
					break;

				case WMSZ_RIGHT:
				case WMSZ_TOPRIGHT:
				case WMSZ_BOTTOMRIGHT:
					rect->right = rect->left + min_width;
					break;
				}
			if (rect->bottom - rect->top < min_height)
				switch (wParam)
				{
				case WMSZ_TOP:
				case WMSZ_TOPLEFT:
				case WMSZ_TOPRIGHT:
					rect->top = rect->bottom - min_height;
					break;

				case WMSZ_BOTTOM:
				case WMSZ_BOTTOMLEFT:
				case WMSZ_BOTTOMRIGHT:
					rect->bottom = rect->top + min_height;
					break;
				}
			::RECT r;
			::GetClientRect(hwnd,&r);
			Viewer.Resize(r.right,r.bottom);
			Viewer.Update(LastDrawTime - StartTime);
			::InvalidateRect(hWindow,NULL,false);
			}
			return TRUE;


		case WM_DROPFILES:
	//ドラッグアンドドロップによるファイル入力
			{
			wchar_t file[MAX_PATH];
			POINT pt;
			DragQueryFile((HDROP)wParam,0,file,MAX_PATH);
			DragQueryPoint((HDROP)wParam,&pt);
			DragFinish((HDROP)wParam);
			}
			return 0l;

		case WM_TIMER:
			{
				if (Viewer.IsValid())
				{
					LastDrawTime = ::timeGetTime();
					if (Viewer.Update(LastDrawTime - StartTime))
						::InvalidateRect(hWindow,NULL,false);
				}
			}
			return 0;
		}


		return DefWindowProc(hwnd,uMsg,wParam,lParam);
	}
};


int WINAPI WinMain(HINSTANCE hThisInst,HINSTANCE hPrevInst,
				   LPSTR lpszArgs,int nWinMode)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Juna::WindowClass<WindowClass> Window;

	Juna::WindowClass<WindowClass>::RParam rparam;
	Juna::WindowClass<WindowClass>::CParam cparam;
	const wchar_t *class_name = L"LyricsViewTest";

	HWND hwnd = Window.RegisterCreate(hThisInst,class_name,class_name,rparam,cparam);
	if (hwnd == NULL)
		return 0;

//	::ShowWindow(hwnd,SW_SHOW);
CHOOSEFONT cf;
  ZeroMemory(&cf, sizeof(cf));
  cf.lStructSize = sizeof(cf);
  cf.hwndOwner = hwnd;
  cf.lpLogFont = 0;
  cf.rgbColors = 0;
  cf.Flags = CF_BOTH | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
//	ChooseFont(&cf);
//  EnumFontFamiliesEx()API関数
	return Window.GetMessageLoop();

}



