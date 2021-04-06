#pragma once

#include "Lyrics/RubyString.h"

#include "../Juna/Picture/Font.h"

class RubyDrawer
{
private:
	Juna::Picture::BufferedFont Font;
	Juna::Picture::BufferedFont RubyFont;
public:
	RubyDrawer(void);
	~RubyDrawer(void);

	bool Initialize(Juna::Picture::Font &font,Juna::Picture::Font &rubyfont);
	void Terminalize(void)
	{
		RubyFont.Terminalize();
		Font.Terminalize();
	}

	int GetFontHeight(void) const {return Font.GetHeight();}
	int GetRubyFontHeight(void) const {return RubyFont.GetHeight();}

	void Draw(Juna::Picture::I_Image &target,int x,int y,const Juna::RubyString &string,Juna::Picture::Color c);

};

