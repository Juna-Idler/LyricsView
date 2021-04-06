#include "RubyDrawer.h"


RubyDrawer::RubyDrawer(void)
{
}


RubyDrawer::~RubyDrawer(void)
{
	Terminalize();
}


bool RubyDrawer::Initialize(Juna::Picture::Font &font,Juna::Picture::Font &rubyfont)
{
	Terminalize();
	if (!Font.Initialize(font))
		return false;
	if (!RubyFont.Initialize(rubyfont))
	{
		Font.Terminalize();
		return false;
	}
	return true;
}

void RubyDrawer::Draw(Juna::Picture::I_Image &target,int x,int y,const Juna::RubyString &string,Juna::Picture::Color c)
{
	int main_y = y + RubyFont.GetHeight();
	int charx = x;
	for (size_t i = 0; i < string.text.size(); i++)
	{
		int incx = Font.DrawCharColor(target,charx,main_y,string.text[i],c);
		charx += incx;
	}

	size_t offset = 0;
	int offset_x = 0;
	for (Juna::RubyString::RubyContainer::const_iterator it = string.rubys.begin(); it != string.rubys.end(); ++it)
	{
		for (size_t j = offset;j < it->offset; j++)
		{
			offset_x += Font.GetCharCellIncX(string.text[j]);
		}
		int startx = offset_x;

		int width = 0;
		for (size_t j = 0;j < it->length; j++)
		{
			width += Font.GetCharCellIncX(string.text[it->offset + j]);
		}
		offset_x += width;
		offset = it->offset + it->length;

		int rubywidth = 0;
		for (size_t j = 0; j < it->ruby.size(); j++)
		{
			rubywidth += RubyFont.GetCharCellIncX(it->ruby[j]);
		}
		int x = startx + (width - rubywidth) / 2;
		for (size_t j = 0; j < it->ruby.size(); j++)
		{
			x += RubyFont.DrawCharColor(target,x,y,it->ruby[j],c);
		}
	}
}

