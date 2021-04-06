
#include "../Juna/Picture/ExpandedFont.h"
#include "../Juna/Picture/Blit.h"


void DrawAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,
					 const wchar_t *line,size_t length,int alignment,
					 const Juna::Picture::BufferedFont &font,const Juna::Picture::BufferedExpandedFont *outfont,
					 Juna::Picture::Color c,Juna::Picture::Color outc);


int CountLine(const wchar_t *line,size_t length,const Juna::Picture::BufferedFont &font, int linewidth,int char_space)
{
	if (length == 0)
		return 1;
	int count = 1;
	int size = font.GetFirstCharCellIncX(line[0]);
	for (size_t i = 1; i < length; i++)
	{
		int width = font.GetCharCellIncX(line[i]) + char_space;
		if (size + width > linewidth)
		{
			count++;
			size = width - char_space;
		}
		else
			size += width;
	}
	return count;
}

int CountLine(const std::wstring &text,const Juna::Picture::BufferedFont &font, int linewidth,int char_space)
{
	if (text.size() == 0)
		return 1;

	int count = 0;
	const wchar_t *line = text.c_str();
	size_t start = 0;
	size_t offset = text.find('\n',0);
	while (offset != text.npos)
	{
		offset--;
		count += CountLine(line,offset - start,font,linewidth,char_space);
		start = offset + 2;
		line = text.c_str() + start;
		offset = text.find('\n',start);
	}
	offset = text.length();
	count += CountLine(line,offset - start,font,linewidth,char_space);
	return count;
}


void DrawOutlinedText(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
					  const std::wstring &text,int halignment,int valignment,
					  const Juna::Picture::BufferedFont &font,const Juna::Picture::BufferedExpandedFont *outfont,
					  Juna::Picture::Color c,Juna::Picture::Color outc)
{
	if (text.length() == 0)
		return;

	const int linewidth = target_rect.width();
	int lineheight = font.GetHeight();
	int linecount = CountLine(text,font,linewidth,0);

	int y = 0;
	if (valignment > 0)
		y = target_rect.y1;
	else if (valignment < 0)
		y = target_rect.y2 - lineheight * linecount;
	else
		y = target_rect.y1 + (target_rect.height() - lineheight * linecount) / 2;

	size_t start = 0;
	size_t offset = text.find('\n',0);
	while (offset != text.npos)
	{
		offset--;

		int size = font.GetFirstCharCellIncX(text[start]);
		for (size_t i = start + 1;i < offset;i++)
		{
			int width = font.GetCharCellIncX(text[i]) + 0;
			if (size + width > linewidth)
			{
				DrawAlignedLine(target,target_rect,y,text.c_str() + start,i - start,halignment,font,outfont,c,outc);
				y += lineheight;
				start = i;
				size = width - 0;
			}
			else
				size += width;
		}
		DrawAlignedLine(target,target_rect,y,text.c_str() + start,offset - start,halignment,font,outfont,c,outc);
		y += lineheight;
		start = offset + 2;
		offset = text.find('\n',start);
	}
	offset = text.length();
	{
		int size = font.GetFirstCharCellIncX(text[start]);
		for (size_t i = start + 1;i < offset;i++)
		{
			int width = font.GetCharCellIncX(text[i]) + 0;
			if (size + width > linewidth)
			{
				DrawAlignedLine(target,target_rect,y,text.c_str() + start,i - start,halignment,font,outfont,c,outc);
				y += lineheight;
				start = i;
				size = width - 0;
			}
			else
				size += width;
		}
		DrawAlignedLine(target,target_rect,y,text.c_str() + start,offset - start,halignment,font,outfont,c,outc);
	}
}



void DrawAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,
					 const wchar_t *line,size_t length,int alignment,
					 const Juna::Picture::BufferedFont &font,const Juna::Picture::BufferedExpandedFont *outfont,
					 Juna::Picture::Color c,Juna::Picture::Color outc)
{
	if (length == 0)
		return;
	int width = font.GetFirstCharCellIncX(*line);
	for (size_t i = 1;i < length;i++)
		width += font.GetCharCellIncX(line[i]);

	int x = target_rect.x1;
	if (alignment > 0)
		x = target_rect.x1;
	else if (alignment < 0)
		x = target_rect.x2 - width;
	else
		x = target_rect.x1 + (target_rect.width() - width) / 2;

	if (outfont)
	{
		outfont->DrawFirstCharColor(target,x, y, line[0], outc);
		int xoffset = x + font.DrawFirstCharColor(target,x, y,  line[0], c);
		for (size_t i = 1; i < length; i++)
		{
			outfont->DrawCharColor(target,xoffset, y, line[i], outc);
			xoffset += font.DrawCharColor(target,xoffset, y,  line[i], c);
		}
	}
	else
	{
		int xoffset = x + font.DrawFirstCharColor(target,x, y,  line[0], c);
		for (size_t i = 1; i < length; i++)
		{
			xoffset += font.DrawCharColor(target,xoffset, y,  line[i], c);
		}

	}
}