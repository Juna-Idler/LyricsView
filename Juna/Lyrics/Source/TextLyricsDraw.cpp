
#include "../TextLyricsDraw.h"

namespace Juna {
namespace Lyrics {


long GetLineHeight(const TextContainer::TextLine &line,const TextDrawParameter &param)
{
	int norubylineheight = param.GetFont().GetHeight() + param.LineSpace + param.NoRubyLineTopSpace;
	if (param.GetRubyFont())
	{
		int rubyedlineheight = param.GetFont().GetHeight() + param.LineSpace + param.GetRubyFont()->GetHeight() + param.RubyBottomSpace;
		return line.string.GetRubyPositions().empty() ? norubylineheight : rubyedlineheight;
	}
	return norubylineheight;
}


void DrawAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,int drawwidth,
					 const TextContainer::TextLine &line,size_t word_start,size_t word_length,
					 Juna::Picture::Color c,Juna::Picture::Color outc,const TextDrawParameter &param)
{
	Juna::Picture::Rect clip_rect(target_rect.x1,// + LeftMargin,
								  target_rect.y1 + param.TopMargin,
								  target_rect.x2,// - RightMargin,
								  target_rect.y2 - param.BottomMargin);
	int x = 0;
	switch (param.HAlignment)
	{
	case HA_Left:
		x = target_rect.x1 + param.LeftMargin;
		break;
	case HA_Right:
		x = target_rect.x2 - param.RightMargin - drawwidth;
		break;
	case HA_Center:
		x = target_rect.x1 + (target_rect.width() - param.LeftMargin - param.RightMargin - drawwidth) / 2 + param.LeftMargin;
		break;
	}

	const LineBreakWords::Words &words = line.words.GetWords();
	const std::wstring &string = line.string.GetString();

	int base_y;
	if (param.GetRubyFont() && !line.string.GetRubyPositions().empty())
		base_y = y + param.GetRubyFont()->GetHeight() + param.RubyBottomSpace;
	else
		base_y = y + param.NoRubyLineTopSpace;

	const Juna::Picture::BufferedOutlineFont &font = param.GetFont();

	if (font.GetThickness() > 0)
	{
		int xoffset = x;
		for (size_t i = word_start; i < word_start + word_length; i++)
		{
			const LineBreakWords::Word &word = words[i];
			if (word.length == 0)
				continue;
			font.DrawOutlineFirstCharPreMultipliedAlpha(target,xoffset, base_y, string[word.offset], outc,&clip_rect);
			int width = font.DrawFirstCharColor(target,xoffset, base_y,  string[word.offset], c,&clip_rect);
			xoffset += width + param.CharSpace;
			for (int j = 1; j < word.length; j++)
			{
				font.DrawOutlineCharPreMultipliedAlpha(target,xoffset, base_y, string[word.offset + j], outc,&clip_rect);
				int width = font.DrawCharColor(target,xoffset, base_y,  string[word.offset + j], c,&clip_rect);
				xoffset += width + param.CharSpace;
			}
		}
	}
	else
	{
		int xoffset = x;
		for (size_t i = word_start; i < word_start + word_length; i++)
		{
			const LineBreakWords::Word &word = words[i];
			if (word.length == 0)
				continue;
			int width = font.DrawFirstCharPreMultipliedAlpha(target,xoffset, base_y,  string[word.offset], c,&clip_rect);
			xoffset += width + param.CharSpace;
			for (int j = 1; j < word.length; j++)
			{
				int width = font.DrawCharPreMultipliedAlpha(target,xoffset, base_y,  string[word.offset + j], c,&clip_rect);
				xoffset += width + param.CharSpace;
			}
		}

	}

//ƒ‹ƒr
	if (!param.GetRubyFont())
		return;

	const Juna::RubyString::RubyPositions &rubys = line.string.GetRubyPositions();
	if (rubys.empty())
		return;

	size_t word_offset = word_start;
	size_t char_offset = 0;
	size_t offset = words[word_start].offset;
	int offset_x = x;

	Juna::RubyString::RubyPositions::const_iterator it = rubys.begin();
	for (; it != rubys.end(); ++it)
	{
		if (it->parent_offset >= words[word_start].offset)
			break;
	}

	const Juna::Picture::BufferedOutlineFont &rfont = *param.GetRubyFont();

	for (; it != rubys.end(); ++it)
	{
		for (;word_offset < word_start + word_length; word_offset++)
		{
			const LineBreakWords::Word &word = words[word_offset];
			if (char_offset == 0)
			{
				if (offset == it->parent_offset)
					goto RubySeachEnd;
				offset_x += font.GetFirstCharCellIncX(string[word.offset]) + param.CharSpace;
				offset++;
				char_offset = 1;
			}
			for (; char_offset < word.length; char_offset++)
			{
				if (offset == it->parent_offset)
					goto RubySeachEnd;
				offset_x += font.GetCharCellIncX(string[word.offset + char_offset]) + param.CharSpace;
				offset++;
			}
			char_offset = 0;
		}
		continue;

RubySeachEnd:;
//ƒ‹ƒr•`‰æ
		int start_x = offset_x;
		int width = 0;
		for (size_t i = 0;i < it->parent_length; i++)
		{
			width += font.GetCharCellIncX(string[it->parent_offset + i]) + param.CharSpace;
		}
		width -= param.CharSpace;

		int rubywidth = 0;
		for (size_t i = 0; i < it->phonetic_length; i++)
		{
			rubywidth += rfont.GetCharCellIncX(line.string.GetPhonetic()[it->phonetic_offset + i]);
		}

		int x;
		int space;
		RubyAlignment ra = param.RAlignment;
		if (rubywidth >= width && (ra == RA_121 || ra == RA_010))
		{
			ra = RA_Center;
		}
		switch (ra)
		{
		default:
		case RA_Center:
			x = start_x + (width - rubywidth) / 2;
			space = 0;
			break;
		case RA_121:
			space = (width - rubywidth) / it->phonetic_length;
			x = start_x + ((width - rubywidth) - space * (it->phonetic_length - 1)) / 2;
			break;
		case RA_010:
			if (it->phonetic_length > 1)
			{
				space = (width - rubywidth) / (it->phonetic_length - 1);
				x = start_x;
			}
			else
			{
				x = start_x + (width - rubywidth) / 2;
				space = 0;
			}
			break;
		case RA_Left:
			x = start_x;
			space = 0;
			break;
		case RA_Right:
			x = start_x + (width - rubywidth);
			space = 0;
			break;
		}


		if (rfont.GetThickness() > 0)
		{
			for (size_t i = 0; i < it->phonetic_length; i++)
			{
				rfont.DrawOutlineCharPreMultipliedAlpha(target,x, y, line.string.GetPhonetic()[it->phonetic_offset + i], outc,&clip_rect);
				x += rfont.DrawCharColor(target,x,y,line.string.GetPhonetic()[it->phonetic_offset + i],c,&clip_rect) + space;
			}
		}
		else
		{
			for (size_t i = 0; i < it->phonetic_length; i++)
			{
				x += rfont.DrawCharPreMultipliedAlpha(target,x,y,line.string.GetPhonetic()[it->phonetic_offset + i],c,&clip_rect) + space;
			}
		}
	}

}

void DrawLine(Juna::Picture::I_Image &target, const Juna::Picture::Rect &target_rect,
			  int y,int drawwidth,Juna::Picture::Color c,Juna::Picture::Color outc,
			  const TextContainer::TextLine &line,const TextDrawParameter &param)
{
	if (line.string.GetString().empty())
		return;

	int lineheight = GetLineHeight(line,param);

	const LineBreakWords::Words &words = line.words.GetWords();
	size_t start = 0;
	size_t length = 1;
	int xoffset = words[0].width + param.CharSpace * (words[0].length - 1);
	int yoffset = 0;
	for (size_t j = 1; j < words.size(); j++)
	{
		const LineBreakWords::Word &word = words[j];
		int width = word.width + param.CharSpace * (word.length - 1);
		if (xoffset + param.CharSpace + width > drawwidth)
		{
			DrawAlignedLine(target,target_rect,y + yoffset,xoffset,line,start,length,c,outc,param);
			start += length;
			length = 1;
			yoffset += lineheight;
			xoffset = width;
		}
		else
		{
			xoffset += param.CharSpace + width;
			length++;
		}
	}
	DrawAlignedLine(target,target_rect,y + yoffset,xoffset,line,start,length,c,outc,param);
}


void DrawText(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
			  Juna::Picture::Color c,Juna::Picture::Color outc,
			  const TextContainer &text,const TextDrawParameter &param)
{
	if (text.GetLines().empty())
		return;

	//ƒ‰ƒCƒ“•‹K§
	int drawwidth = target_rect.width() - param.LeftMargin - param.RightMargin;

	int y = 0;
	switch (param.VAlignment)
	{
	case VA_Top:
		y = target_rect.y1 + param.TopMargin;
		break;
	case VA_Center:
		y = target_rect.y1 + param.TopMargin + (target_rect.height() - (param.TopMargin + param.BottomMargin)) / 2;
		break;
	case VA_Bottom:
		{
		y = target_rect.y2 - param.BottomMargin;
		y += param.CurrentLineYOffset;

		for (TextContainer::TextLines::const_reverse_iterator it = text.GetLines().rbegin();
			it != text.GetLines().rend() && y > target_rect.y1 + param.TopMargin ; ++it )
		{
			y -= it->words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(*it,param);
			DrawLine(target, target_rect,y,drawwidth,c, outc, *it,param);
		}

		}
		return;
	}
	y += param.CurrentLineYOffset;

	size_t i;
	for ( i = 0 ; i < text.GetLines().size() ; i++)
	{
		int lineheight = text.GetLines()[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(text.GetLines()[i],param);
		if (y + lineheight > target_rect.y1 + param.TopMargin)
			break;
		y += lineheight;
	}

	for ( ; i < text.GetLines().size() && y < target_rect.y2 - param.BottomMargin ; i++ )
	{
		DrawLine(target, target_rect,y,drawwidth,c, outc, text.GetLines()[i],param);
		y += text.GetLines()[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(text.GetLines()[i],param);
	}
}


}//namespace Lyrics
}//namespace Juna

