#pragma once

#include "LyricsContainer.h"
#include "../Picture/image.h"
#include "../Picture/Font.h"


namespace Juna {
namespace Lyrics {

enum VerticalAlignment {VA_Top = 1,VA_Center = 0,VA_Bottom = -1};
enum HorizontalAlignment {HA_Left = 1,HA_Center = 0,HA_Right = -1};
enum RubyAlignment {RA_Center = 0,RA_121 = 1,RA_010 = 2,RA_Left = 3,RA_Right = 4};


class TextDrawParameter
{
private:

	const Juna::Picture::BufferedOutlineFont *Font;
	const Juna::Picture::BufferedOutlineFont *RubyFont;

public:

	bool SetFont(const Juna::Picture::BufferedOutlineFont *font,const Juna::Picture::BufferedOutlineFont *rubyfont)
	{
		if (!font || !font->IsValid() ||
			(rubyfont && !rubyfont->IsValid())
			)
			return false;
		Font = font;
		RubyFont = rubyfont;
		return true;
	}

	const Juna::Picture::BufferedOutlineFont &GetFont(void) const {return *Font;}
	const Juna::Picture::BufferedOutlineFont*GetRubyFont(void) const {return RubyFont;}

	VerticalAlignment VAlignment;
	int CurrentLineYOffset;

	HorizontalAlignment HAlignment;

	int LineSpace;
	int CharSpace;
	int NoRubyLineTopSpace;
	int RubyBottomSpace;

	RubyAlignment RAlignment;

	int LeftMargin;
	int TopMargin;
	int RightMargin;
	int BottomMargin;


	bool IsValid(void) const
	{
		return Font != 0;
	}


 	TextDrawParameter(void) :
		Font(0),
		RubyFont(0),

		VAlignment(VA_Center),
		CurrentLineYOffset(0),
		HAlignment(HA_Center),

		LineSpace(0),
		CharSpace(0),
		NoRubyLineTopSpace(0),
		RubyBottomSpace(0),
		RAlignment(RA_Center),

		LeftMargin(0),
		TopMargin(0),
		RightMargin(0),
		BottomMargin(0)
	{}

};
	

class LyricsDrawParameter : public TextDrawParameter
{
public:
	Juna::Picture::Color CurrentTextColor;	//現在行
	Juna::Picture::Color CurrentTextOutlineColor;

	Juna::Picture::Color OtherTextColor;	//その他の行
	Juna::Picture::Color OtherTextOutlineColor;

	Juna::Picture::Color StandbyTextColor;	//（カラオケ）待機現在行
	Juna::Picture::Color StandbyTextOutlineColor;


	Juna::Picture::ARGB CurrentLineBackColor;

	int CurrentLineBackPlusUp;
	int CurrentLineBackPlusDown;

	int ScrollTime;
	int FadeTime;
	

	LyricsDrawParameter(void) :
		TextDrawParameter(),
		CurrentTextColor(255,255,255),
		OtherTextColor(192,192,192),
		StandbyTextColor(255,128,128),

		CurrentTextOutlineColor(255,0,0),
		OtherTextOutlineColor(0,0,255),
		StandbyTextOutlineColor(0,0,255),

		CurrentLineBackColor(0,0,0,0),

		CurrentLineBackPlusUp(0),
		CurrentLineBackPlusDown(0),


		ScrollTime(500),
		FadeTime(200)
	{}


};

}//namespace Lyrics
}//namespace Juna

