#include "DisplayText.h"



void DisplayText::SetText(const wchar_t *text)
{
	Text.assign(text);
	if (Font)
		LineBreak.SetWords(Text,Font->GetFont());
	else
		LineBreak.SetWords(Text);
}
void DisplayText::SetFont(const Juna::Picture::BufferedFont &font,const Juna::Picture::BufferedExpandedFont *outfont)
{
	Font = &font;
	OutFont = outfont;
	LineBreak.SetWords(Text,Font->GetFont());
}

void DisplayText::Draw(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect)
{
	if (Text.length() == 0)
		return;

	const int linewidth = target_rect.width();
	int lineheight = Font->GetHeight();
	int linecount = CountLine(linewidth,0);

	int y = 0;
	if (VAlignment > 0)
		y = target_rect.y1;
	else if (VAlignment < 0)
		y = target_rect.y2 - lineheight * linecount;
	else
		y = target_rect.y1 + (target_rect.height() - lineheight * linecount) / 2;



	const Juna::LineBreakWords::Words &words = LineBreak.GetWords();
	size_t start = 0;
	size_t length = 0;
	for (size_t i = 0; i < words.size(); i++)
	{
		const Juna::LineBreakWords::Word &word = words[i];
		long width = word.width;
		if (width == Juna::LineBreakWords::ReturnMagicNumber)
		{
			if (length > 0)
				y += DrawLineBlock(target,target_rect,y,start,length) * lineheight;
			else
				y += lineheight;
			start += length + 1;
			length = 0;
		}
		else
			length++;
	}
	if (length > 0)
		DrawLineBlock(target,target_rect,y,start,length);
}

int DisplayText::DrawLineBlock(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,
								   size_t word_start,size_t word_length)
{
	const int linewidth = target_rect.width();
	int lineheight = Font->GetHeight();

	const Juna::LineBreakWords::Words &words = LineBreak.GetWords();
	size_t start = word_start;
	size_t length = 1;
	int xoffset = words[word_start].width;
	int linecount = 1;
	for (size_t i = word_start + 1; i < word_start + word_length; i++)
	{
		const Juna::LineBreakWords::Word &word = words[i];
		int width = word.width;
		if (xoffset + width > target_rect.width())
		{
			DrawAlignedLine(target,target_rect,y,start,length);
			start += length;
			length = 1;
			y += lineheight;
			linecount++;
			xoffset = width;
		}
		else
		{
			xoffset += width;
			length++;
		}
	}
	DrawAlignedLine(target,target_rect,y,start,length);
	return linecount;
}


void DisplayText::DrawAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,
								  size_t word_start,size_t word_length)
{
	const Juna::LineBreakWords::Words &words = LineBreak.GetWords();

	int width = words[word_start].width;
	for (size_t i = word_start + 1;i < word_start + word_length;i++)
		width += words[i].width;

	int x = target_rect.x1;
	if (HAlignment > 0)
		x = target_rect.x1;
	else if (HAlignment < 0)
		x = target_rect.x2 - width;
	else
		x = target_rect.x1 + (target_rect.width() - width) / 2;

	if (OutFont)
	{
		int xoffset = x;
		for (size_t i = word_start; i < word_start + word_length; i++)
		{
			const Juna::LineBreakWords::Word &word = words[i];
			if (word.length == 0)
				continue;
			OutFont->DrawFirstCharColor(target,xoffset, y,Text[word.offset], OutlineColor,&target_rect);
			int width = Font->DrawFirstCharColor(target,xoffset, y,  Text[word.offset], Color,&target_rect);
			xoffset += width;
			for (int j = 1; j < word.length; j++)
			{
				OutFont->DrawCharColor(target,xoffset, y, Text[word.offset + j], OutlineColor,&target_rect);
				int width = Font->DrawCharColor(target,xoffset, y,  Text[word.offset + j], Color,&target_rect);
				xoffset += width;
			}
		}
	}
	else
	{
		int xoffset = x;
		for (size_t i = word_start; i < word_start + word_length; i++)
		{
			const Juna::LineBreakWords::Word &word = words[i];
			if (word.length == 0)
				continue;
			int width = Font->DrawFirstCharColor(target,xoffset, y,  Text[word.offset], Color,&target_rect);
			xoffset += width;
			for (int j = 1; j < word.length; j++)
			{
				int width = Font->DrawCharColor(target,xoffset, y,  Text[word.offset + j], Color,&target_rect);
				xoffset += width;
			}
		}

	}
}



int DisplayText::CountLine(int linewidth,int char_space)
{
	if (Text.size() == 0)
		return 1;

	int count = 0;
	const wchar_t *line = Text.c_str();
	size_t start = 0;
	size_t offset = Text.find('\n',0);
	while (offset != Text.npos)
	{
		offset--;
		count += CountLine(line,offset - start,linewidth,char_space);
		start = offset + 2;
		line = Text.c_str() + start;
		offset = Text.find('\n',start);
	}
	offset = Text.length();
	count += CountLine(line,offset - start,linewidth,char_space);
	return count;
}

int DisplayText::CountLine(const wchar_t *line,size_t length, int linewidth,int char_space)
{
	if (length == 0)
		return 1;
	int count = 1;
	int size = Font->GetFirstCharCellIncX(line[0]);
	for (size_t i = 1; i < length; i++)
	{
		int width = Font->GetCharCellIncX(line[i]) + char_space;
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