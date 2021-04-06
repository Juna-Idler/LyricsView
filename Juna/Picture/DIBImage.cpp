
#include "DIBImage.h"

#include <Windows.h>
#include <new>


namespace Juna {
namespace Picture {


bool DIBImage::Initialize(int width,int height)
{
	Terminalize();
	BITMAPINFO bi,*lpbi = &bi;

	lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbi->bmiHeader.biWidth = width;
	lpbi->bmiHeader.biHeight = height;
	lpbi->bmiHeader.biBitCount = 32;
	lpbi->bmiHeader.biSizeImage = width * height * 32 / 8;
	lpbi->bmiHeader.biPlanes = 1;
	lpbi->bmiHeader.biCompression = BI_RGB;

	void *mem;
	HBITMAP hBmp = ::CreateDIBSection(NULL,lpbi,DIB_RGB_COLORS,
									  &mem,NULL,0);
	if (hBmp == NULL)
		return false;

	Buffer = static_cast<ARGB *>(mem) + width * (height - 1);

	hDC = ::CreateCompatibleDC(NULL);
	if (hDC == NULL)
	{
		::DeleteObject(hBmp);
		Terminalize();
		return false;
	}
	hOldBitmap = ::SelectObject(static_cast<HDC>(hDC),hBmp);
	if (hOldBitmap == NULL)
	{
		::DeleteObject(hBmp);
		Terminalize();
		return false;
	}

	hBitmap = hBmp;
	Width = width;
	Height = height;
	Distance = -width;

	return true;
}

void DIBImage::Terminalize(void)
{
	if (hDC)
	{
		HDC hdc = static_cast<HDC>(hDC);
		if (hBitmap)
		{
			::SelectObject(hdc,hOldBitmap);
			::DeleteObject(hBitmap);
			hOldBitmap = hBitmap = NULL;
		}
		::DeleteDC(hdc);
		hDC = NULL;
	}
	Buffer = 0;
	Width = Height = Distance = 0;
}

void DIBImage::DeselectBitmap(void) const
{
	HDC hdc = static_cast<HDC>(hDC);
	::SelectObject(hdc,hOldBitmap);
}
void DIBImage::SelectBitmap(void) const
{
	HDC hdc = static_cast<HDC>(hDC);
	::SelectObject(hdc,hBitmap);
}


}//namespace Picture
}//namespace Juna

