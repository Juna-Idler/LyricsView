
#include "../TextDraw.h"


namespace Juna {
namespace Lyrics {


void PlainText::SetText(const wchar_t *text)
{
	Text.AssignString(text);
	if (Font)
		LineBreak.SetWords(Text,Font->GetFont());
	else
		LineBreak.SetWords(Text);
}
void PlainText::SetText(const Juna::RubyString &rubytext)
{
	Text = rubytext;
	if (Font)
		LineBreak.SetWords(Text,Font->GetFont());
	else
		LineBreak.SetWords(Text);
}

bool PlainText::SetFont(const Juna::Picture::BufferedFont &font,const Juna::Picture::BufferedExpandedFont *outfont)
{
	if (!font.IsValid() || (outfont && !outfont->IsValid()))
		return false;
	Font = &font;
	OutFont = outfont;
	LineBreak.SetWidth(Text,Font->GetFont());
	return true;
}
bool PlainText::SetFont(const Juna::Picture::BufferedFont &font,const Juna::Picture::BufferedFont &rubyfont,
						const Juna::Picture::BufferedExpandedFont *outfont,
						const Juna::Picture::BufferedExpandedFont *rubyoutfont)
{
	if (!font.IsValid() || (outfont && !outfont->IsValid()) ||
		!rubyfont.IsValid() || (rubyoutfont && !rubyoutfont->IsValid()))
		return false;
	Font = &font;
	RubyFont = &rubyfont;
	OutFont = outfont;
	RubyOutFont = rubyoutfont;
	LineBreak.SetWidth(Text,Font->GetFont());
	return true;
}


void PlainText::Draw(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect)
{
	if (Text.Empty())
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

int PlainText::DrawLineBlock(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,
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


void PlainText::DrawAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,
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

	const std::wstring &text = Text.GetText();
	if (OutFont)
	{
		int xoffset = x;
		for (size_t i = word_start; i < word_start + word_length; i++)
		{
			const Juna::LineBreakWords::Word &word = words[i];
			if (word.length == 0)
				continue;
			OutFont->DrawFirstCharColor(target,xoffset, y,text[word.offset], OutlineColor,&target_rect);
			int width = Font->DrawFirstCharColor(target,xoffset, y,  text[word.offset], Color,&target_rect);
			xoffset += width;
			for (int j = 1; j < word.length; j++)
			{
				OutFont->DrawCharColor(target,xoffset, y, text[word.offset + j], OutlineColor,&target_rect);
				int width = Font->DrawCharColor(target,xoffset, y,  text[word.offset + j], Color,&target_rect);
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
			int width = Font->DrawFirstCharColor(target,xoffset, y,  text[word.offset], Color,&target_rect);
			xoffset += width;
			for (int j = 1; j < word.length; j++)
			{
				int width = Font->DrawCharColor(target,xoffset, y,  text[word.offset + j], Color,&target_rect);
				xoffset += width;
			}
		}
	}


//ƒ‹ƒr
	if (!RubyFont)
		return;

	const Juna::RubyString::RubyContainer &rubys = Text.GetRuby();
	if (rubys.empty())
		return;

	size_t word_offset = word_start;
	size_t char_offset = 0;
	size_t offset = words[word_start].offset;
	int offset_x = x;

	Juna::RubyString::RubyContainer::const_iterator it = rubys.begin();
	for (; it != rubys.end(); ++it)
	{
		if (it->offset >= words[word_start].offset)
			break;
	}


	const Juna::Picture::BufferedFont &rfont = *RubyFont;
	const Juna::Picture::BufferedExpandedFont *routfont = RubyOutFont;

	for (; it != rubys.end(); ++it)
	{
		for (;word_offset < word_start + word_length; word_offset++)
		{
			const LineBreakWords::Word &word = words[word_offset];
			if (char_offset == 0)
			{
				if (offset == it->offset)
					goto RubySeachEnd;
				offset_x += Font->GetFirstCharCellIncX(text[word.offset]);
				offset++;
				char_offset = 1;
			}
			for (; char_offset < word.length; char_offset++)
			{
				if (offset == it->offset)
					goto RubySeachEnd;
				offset_x += Font->GetCharCellIncX(text[word.offset + char_offset]);
				offset++;
			}
			char_offset = 0;
		}
		continue;

RubySeachEnd:;
//ƒ‹ƒr•`‰æ
		int start_x = offset_x;
		int width = 0;
		for (size_t i = 0;i < it->length; i++)
		{
			width += Font->GetCharCellIncX(text[it->offset + i]);
		}

		int rubywidth = 0;
		for (size_t i = 0; i < it->ruby.size(); i++)
		{
			rubywidth += rfont.GetCharCellIncX(it->ruby[i]);
		}
		int x = start_x + (width - rubywidth) / 2;
		if (routfont)
		{
			for (size_t i = 0; i < it->ruby.size(); i++)
			{
				routfont->DrawCharColor(target,x, y, it->ruby[i], OutlineColor,&target_rect);
				x += rfont.DrawCharColor(target,x,y,it->ruby[i],Color,&target_rect);
			}
		}
		else
		{
			for (size_t i = 0; i < it->ruby.size(); i++)
			{
				x += rfont.DrawCharColor(target,x,y,it->ruby[i],Color,&target_rect);
			}
		}
	}


}



int PlainText::CountLine(int linewidth,int char_space)
{
	if (Text.Empty())
		return 1;

	const std::wstring &text = Text.GetText();
	int count = 0;
	const wchar_t *line = text.c_str();
	size_t start = 0;
	size_t offset = text.find('\n',0);
	while (offset != text.npos)
	{
		offset--;
		count += CountLine(line,offset - start,linewidth,char_space);
		start = offset + 2;
		line = text.c_str() + start;
		offset = text.find('\n',start);
	}
	offset = text.length();
	count += CountLine(line,offset - start,linewidth,char_space);
	return count;
}

int PlainText::CountLine(const wchar_t *line,size_t length, int linewidth,int char_space)
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


}//namespace Lyrics
}//namespace Juna

