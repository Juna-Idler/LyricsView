#pragma once

#include <string>

#include "../Juna/Picture/Font.h"
#include "../Juna/Picture/ExpandedFont.h"
#include "../Juna/LineBreak.h"

class DisplayText
{
private:
	std::wstring Text;
	Juna::LineBreakWords LineBreak;

	const Juna::Picture::BufferedFont *Font;
	const Juna::Picture::BufferedExpandedFont *OutFont;

public:
	Juna::Picture::Color Color;
	Juna::Picture::Color OutlineColor;
	int HAlignment;
	int VAlignment;


	bool Empty(void) const {return Text.empty();}
	void Clear(void)
	{
		Text.clear();
		LineBreak.words.clear();
		Font = 0;
		OutFont = 0;
	}

	DisplayText(const Juna::Picture::BufferedFont &font,
				const Juna::Picture::BufferedExpandedFont *outfont)
		: Text(),LineBreak(),Font(&font),OutFont(outfont),
				Color(255,255,255),OutlineColor(0,0,0),
				HAlignment(0),VAlignment(1)
	{}
	~DisplayText(void)
	{}


	void SetText(const wchar_t *text);

	void SetFont(const Juna::Picture::BufferedFont &font,const Juna::Picture::BufferedExpandedFont *outfont);


	void Draw(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect);


private:
	int DrawLineBlock(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,
					  size_t word_start,size_t word_length);

	void DrawAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,
					 size_t word_start,size_t word_length);

	int CountLine(int linewidth,int char_space);
	int CountLine(const wchar_t *line,size_t length ,int linewidth,int char_space);

};

