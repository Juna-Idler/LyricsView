
#pragma once

#include "Font.h"


namespace Juna {
namespace Picture {


class BufferedExpandedFont
{
	typedef std::map<wchar_t,FontImage *> map;
public:
	BufferedExpandedFont(void) : BaseFont(0), Slip(1), EDirect(ED_Null), Buffer(new map()) {}
	~BufferedExpandedFont() {Terminalize(); delete Buffer;}

	enum ExpandDirect {
		ED_UpLeft	 = 1,
		ED_Up		 = 2,
		ED_UpRight	 = 4,
		ED_Left		 = 8,
		ED_Null		 = 0,
		ED_Right	 = 16,
		ED_DownLeft	 = 32,
		ED_Down		 = 64,
		ED_DownRight = 128,
		ED_4 = ED_Up | ED_Down | ED_Left | ED_Right,
		ED_Tilted4 = ED_UpLeft | ED_UpRight | ED_DownLeft | ED_DownLeft,
		ED_8 = ED_4 | ED_Tilted4,
	};

	bool Initialize(const BufferedFont &font,unsigned slip,ExpandDirect ed = ED_4);

	void Terminalize(void);

	bool IsValid(void) const {return BaseFont != 0;}
	unsigned short GetSlip(void) const {return Slip;}

	const BufferedFont *GetFont(void) const {return BaseFont;}

	long GetHeight(void) const {return BaseFont->GetHeight() + Slip * 2;}

	short GetCharCellIncX(wchar_t c) const
	{
		short r = BaseFont->GetCharCellIncX(c);
		if (r < 0)
			return r;
		return r + Slip * 2;
	}

	short DrawCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;
	short DrawCharPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;

	short DrawFirstCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawFirstCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;
	short DrawFirstCharPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawFirstCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;


	const FontImage *GetFontImage(wchar_t c) const;

private:
	bool SetBuffer(wchar_t c) const;

	const BufferedFont *BaseFont;
	unsigned short Slip;
	ExpandDirect EDirect;
	map *Buffer;

private:
	BufferedExpandedFont(BufferedExpandedFont &);
	void operator=(BufferedExpandedFont &);
};


}//namespace Picture
}//namespace Juna

