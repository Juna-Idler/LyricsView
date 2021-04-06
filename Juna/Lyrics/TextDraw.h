#pragma once

#include "../RubyString.h"
#include "RubyCommon.h"

#include "../Picture/Font.h"
#include "../Picture/ExpandedFont.h"


namespace Juna {
namespace Lyrics {


class PlainText
{
private:
	Juna::RubyString Text;
	Juna::RubyLineBreakWords LineBreak;

	const Juna::Picture::BufferedFont *Font;
	const Juna::Picture::BufferedExpandedFont *OutFont;

	const Juna::Picture::BufferedFont *RubyFont;
	const Juna::Picture::BufferedExpandedFont *RubyOutFont;

public:
	enum VerticalAlignment {VA_Top = 1,VA_Center = 0,VA_Bottom = -1};
	enum HorizontalAlignment {HA_Left = 1,HA_Center = 0,HA_Right = -1};

	Juna::Picture::Color Color;
	Juna::Picture::Color OutlineColor;
	VerticalAlignment VAlignment;
	HorizontalAlignment HAlignment;


	bool Empty(void) const {return Text.GetText().empty();}
	void Clear(void) {Text.Clear();LineBreak.Clear();}

	PlainText(void)
	  : Text(),LineBreak(),
		Font(0),OutFont(0),
		RubyFont(0),RubyOutFont(0),
		Color(255,255,255),OutlineColor(0,0,0),
		HAlignment(HA_Center),VAlignment(VA_Top)
	{}

	PlainText(const Juna::Picture::BufferedFont &font,
			  const Juna::Picture::BufferedExpandedFont *outfont)
	  : Text(),LineBreak(),
		Font(&font),OutFont(outfont),
		RubyFont(0),RubyOutFont(0),
		Color(255,255,255),OutlineColor(0,0,0),
		HAlignment(HA_Center),VAlignment(VA_Top)
	{}
	~PlainText(void)
	{}


	void SetText(const wchar_t *text);
	void SetText(const Juna::RubyString &rubytext);

	bool SetFont(const Juna::Picture::BufferedFont &font,const Juna::Picture::BufferedExpandedFont *outfont);
	bool SetFont(const Juna::Picture::BufferedFont &font,const Juna::Picture::BufferedFont &rubyfont,
				 const Juna::Picture::BufferedExpandedFont *outfont,
				 const Juna::Picture::BufferedExpandedFont *rubyoutfont);


	void Draw(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect);


private:
	int DrawLineBlock(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,
					  size_t word_start,size_t word_length);

	void DrawAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,
					 size_t word_start,size_t word_length);

	int CountLine(int linewidth,int char_space);
	int CountLine(const wchar_t *line,size_t length ,int linewidth,int char_space);

};

}//namespace Lyrics
}//namespace Juna

