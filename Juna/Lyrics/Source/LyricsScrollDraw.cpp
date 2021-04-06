
#include "../LyricsScrollDraw.h"

#include "../TimeTag.h"
#include "../../Picture/Blit.h"

#include "../TextLyricsDraw.h"

#include "../KaraokeDraw.h"

namespace Juna {
namespace Lyrics {
namespace ScrollDraw {

namespace {
	inline Juna::Picture::Color BlendColor(Juna::Picture::Color a,Juna::Picture::Color b,float a_rate)
	{
		return Juna::Picture::Color(
			unsigned char(b.r - (b.r - a.r) * a_rate) ,
			unsigned char(b.g - (b.g - a.g) * a_rate) ,
			unsigned char(b.b - (b.b - a.b) * a_rate) );
	}
}

bool GetDrawPointLyrics(DrawPoint &dest,const LyricsContainer &lyrics,const LyricsDrawParameter &param,int playtime_ms)
{
	if ( lyrics.GetFont() == 0 || lyrics.GetFont() != &param.GetFont().GetFont() )
		return false;

	const LyricsContainer::LyricsLines &Lines = lyrics.GetLines();
	if (Lines.empty())
		return false;

	size_t index;
	for ( index = 0 ; index < Lines.size() - 1 ; index++ )
	{
		if ( playtime_ms < Lines[index + 1].starttime )
			break;
	}
	if ( index == Lines.size() - 1 )
		return false;

	int scroll_time = param.ScrollTime;
	if ( scroll_time < 0 )
		scroll_time = TimeTag::MaxTime;

	//スクロール判定
	float scroll_y = 0;
	float scroll_rate = 0;

	int next = Lines[index + 1].starttime;
	if ( playtime_ms + scroll_time > next )
	{
		int scroll = next - Lines[index].starttime;
		if ( scroll_time < scroll )
		{
			scroll = scroll_time;
		}
		int sub = next - playtime_ms;
		scroll_y = 1.0f - float(sub) / scroll;

		if ( param.FadeTime >= 0 && scroll > param.FadeTime )
		{
			if ( sub > param.FadeTime )
				scroll_rate = 0;
			else
				scroll_rate = 1.0f - sub / (float)param.FadeTime;
		}
		else
			scroll_rate = 1.0f - (float)sub / scroll;
	}
	dest.index = index;
	dest.scroll_y = scroll_y;
	dest.scroll_rate = scroll_rate;
	return true;
}



void DrawLyrics(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const LyricsContainer &lyrics,const LyricsDrawParameter &param,const DrawPoint &drawpoint)
{
	const LyricsContainer::LyricsLines &Lines = lyrics.GetLines();

	size_t index = drawpoint.index;

	//ライン幅規制
	int drawwidth = target_rect.width() - param.LeftMargin - param.RightMargin;

	int currentlinecount = Lines[index].words.CountLine(drawwidth,param.CharSpace);
	int nextlinecount = Lines[index + 1].words.CountLine(drawwidth,param.CharSpace);

	long currentheight = currentlinecount * GetLineHeight(Lines[index],param);
	long nextheight = nextlinecount * GetLineHeight(Lines[index + 1],param);

	long scrollheight = 0;
	int current_y = 0;
	switch (param.VAlignment)
	{
	case VA_Top:
		current_y = target_rect.y1 + param.TopMargin;
		scrollheight = currentheight;
		break;
	case VA_Center:
		current_y = target_rect.y1 + param.TopMargin + (target_rect.height() - (param.TopMargin + param.BottomMargin) - currentheight) / 2;
		scrollheight = (nextheight + currentheight) / 2;
		break;
	case VA_Bottom:
		current_y = target_rect.y2 - param.BottomMargin - currentheight;
		scrollheight = nextheight;
		break;
	}
	current_y += param.CurrentLineYOffset;

	long scroll_y = long(scrollheight *  drawpoint.scroll_y);
	float scroll_rate = drawpoint.scroll_rate;

	if (param.CurrentLineBackColor.a > 0)
	{
		long cy = current_y - scroll_y - param.LineSpace;
		long ny = cy + currentheight;
		long cy2 = ny + param.LineSpace;
		long ny2 = ny + nextheight + param.LineSpace;
		
		Juna::Picture::Rect rect(target_rect.x1,
								 long(cy - (cy - ny) * scroll_rate) - param.CurrentLineBackPlusUp,
								 target_rect.x2,
								 long(cy2 - (cy2 - ny2) * scroll_rate) + param.CurrentLineBackPlusDown);
		if (rect.SetIntersectRect(rect,target_rect))
			Juna::Picture::PreMultipliedAlpha::TranslucentFillColor(target,param.CurrentLineBackColor,param.CurrentLineBackColor.a + 1,&rect);
	}


//現在行
	int y = current_y - scroll_y;
	{
		Juna::Picture::Color c = BlendColor(param.OtherTextColor,param.CurrentTextColor,scroll_rate);
		Juna::Picture::Color sidec = BlendColor(param.OtherTextOutlineColor,param.CurrentTextOutlineColor,scroll_rate);
		DrawLine(target, target_rect, y, drawwidth, c, sidec, Lines[index],param);
	}
//次の行
	y = current_y - scroll_y + currentheight;
	{
		Juna::Picture::Color c = BlendColor(param.CurrentTextColor,param.OtherTextColor,scroll_rate);
		Juna::Picture::Color sidec = BlendColor(param.CurrentTextOutlineColor,param.OtherTextOutlineColor,scroll_rate);
		DrawLine(target, target_rect, y, drawwidth, c, sidec, Lines[index + 1],param);
	}

//過去行
	y = current_y - scroll_y;
	for ( int i = index - 1 ; i >= 0 && y > param.TopMargin ; i-- )
	{
		y -= Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i],param);
		DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i],param);
	}

//未来行
	y = current_y - scroll_y + currentheight + nextheight;
	for ( size_t i = index + 2 ; i < Lines.size() && y < target.height() - param.BottomMargin ; i++ )
	{
		DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i],param);
		y += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i],param);
	}
}

void DrawCSLyrics(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
				  const LyricsContainer &lyrics,const LyricsDrawParameter &param,int playtime_ms)
{
	const LyricsContainer::LyricsLines &Lines = lyrics.GetLines();

	if (Lines.empty())
		return;

	size_t index;
	for ( index = 0 ; index < Lines.size() - 1 ; index++ )
	{
		if ( playtime_ms < Lines[index + 1].starttime )
			break;
	}
	if ( index == Lines.size() - 1 )
		return;

	float fscroll_y = 0;
	float scroll_rate = 0;

	int next = Lines[index + 1].starttime;

	int scroll = next - Lines[index].starttime;
	int sub = next - playtime_ms;
	fscroll_y = 1.0f - float(sub) / scroll;

	if ( scroll > param.FadeTime )
	{
		if ( sub > param.FadeTime )
			scroll_rate = 0;
		else
			scroll_rate = 1.0f - sub / (float)param.FadeTime;
	}
	else
		scroll_rate = 1.0f - (float)sub / scroll;

	//ライン幅規制
	int drawwidth = target_rect.width() - param.LeftMargin - param.RightMargin;

	int currentlinecount = Lines[index].words.CountLine(drawwidth,param.CharSpace);
	int nextlinecount = Lines[index + 1].words.CountLine(drawwidth,param.CharSpace);

	long currentheight = currentlinecount * GetLineHeight(Lines[index],param);
	long nextheight = nextlinecount * GetLineHeight(Lines[index + 1],param);

	int current_y = 0;
	switch (param.VAlignment)
	{
	case VA_Top:
		current_y = target_rect.y1 + param.TopMargin;
		break;
	case VA_Center:
		current_y = target_rect.y1 + param.TopMargin + (target_rect.height() - (param.TopMargin + param.BottomMargin)) / 2;
		break;
	case VA_Bottom:
		current_y = target_rect.y2 - param.BottomMargin;
		break;
	}
	current_y += param.CurrentLineYOffset;

	long scroll_y = long(currentheight *  fscroll_y);

	if (param.CurrentLineBackColor.a > 0)
	{
		long cy = current_y - scroll_y - param.LineSpace;
		long ny = cy + currentheight;
		long cy2 = ny + param.LineSpace;
		long ny2 = ny + nextheight + param.LineSpace;
		
		Juna::Picture::Rect rect(target_rect.x1,
								 long(cy - (cy - ny) * scroll_rate) - param.CurrentLineBackPlusUp,
								 target_rect.x2,
								 long(cy2 - (cy2 - ny2) * scroll_rate) + param.CurrentLineBackPlusDown);
		if (rect.SetIntersectRect(rect,target_rect))
			Juna::Picture::PreMultipliedAlpha::TranslucentFillColor(target,param.CurrentLineBackColor,param.CurrentLineBackColor.a + 1,&rect);
	}


//現在行
	int y = current_y - scroll_y;
	{
		Juna::Picture::Color c = BlendColor(param.OtherTextColor,param.CurrentTextColor,scroll_rate);
		Juna::Picture::Color sidec = BlendColor(param.OtherTextOutlineColor,param.CurrentTextOutlineColor,scroll_rate);
		DrawLine(target, target_rect, y, drawwidth, c, sidec, Lines[index],param);
	}
//次の行
	y = current_y - scroll_y + currentheight;
	{
		Juna::Picture::Color c = BlendColor(param.CurrentTextColor,param.OtherTextColor,scroll_rate);
		Juna::Picture::Color sidec = BlendColor(param.CurrentTextOutlineColor,param.OtherTextOutlineColor,scroll_rate);
		DrawLine(target, target_rect, y, drawwidth, c, sidec, Lines[index + 1],param);
	}

//過去行
	y = current_y - scroll_y;
	for ( int i = index - 1 ; i >= 0 && y > param.TopMargin ; i-- )
	{
		y -= Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i],param);
		DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i],param);
	}

//未来行
	y = current_y - scroll_y + currentheight + nextheight;
	for ( size_t i = index + 2 ; i < Lines.size() && y < target.height() - param.BottomMargin ; i++ )
	{
		DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i],param);
		y += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i],param);
	}
}

//=================================================================================================

//										カラオケ

//=================================================================================================






void DrawKaraoke(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const KaraokeContainer &karaoke,const LyricsDrawParameter &param,int playtime_ms)
{
	const KaraokeContainer::KaraokeLines &Lines = karaoke.GetLines();
	if ( Lines.empty() || karaoke.GetFont() == 0 || karaoke.GetFont() != &param.GetFont().GetFont() )
		return;

	size_t index;
	for ( index = 0 ; index < Lines.size() - 1 ; index++ )
	{
		if ( playtime_ms < Lines[index + 1].starttime )
			break;
	}
	if ( index == Lines.size() - 1 )
		return;

	size_t nextindex = index + 1;

	size_t currentlines = 1;
	if (index > 0)
	{
		for (size_t i = index - 1;Lines[i].starttime == Lines[index].starttime;i--)
		{
			currentlines++;
			if (i == 0)
				break;
		}
		index -= (currentlines - 1);
	}
	size_t nextlines = 1;
	if (nextindex + 1 < Lines.size())
	{
		for (size_t i = nextindex + 1;Lines[i].starttime == Lines[nextindex].starttime;i++)
		{
			nextlines++;
		}
	}

	int scroll_time = param.ScrollTime;
	if ( scroll_time < 0 )
		scroll_time = TimeTag::MaxTime;

	//スクロール判定
	float scroll_rate = 0;
	float fade_rate = 0;

	int next = Lines[nextindex].starttime;
	if ( playtime_ms + scroll_time > next )
	{
		int scroll = next - Lines[index].starttime;
		if ( scroll_time < scroll )
		{
			scroll = scroll_time;
		}
		int sub = next - playtime_ms;
		scroll_rate = 1.0f - float(sub) / scroll;

		if ( param.FadeTime >= 0 && scroll > param.FadeTime )
		{
			if ( sub > param.FadeTime )
				fade_rate = 0;
			else
				fade_rate = 1.0f - sub / (float)param.FadeTime;
		}
		else
			fade_rate = 1.0f - (float)sub / scroll;
	}


	//ライン幅規制
	int drawwidth = target_rect.width() - param.LeftMargin - param.RightMargin;

	long currentheight = Lines[index].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[index].GetLyricsLine(),param);
	for (size_t i = index + 1;i < index + currentlines;i++)
		currentheight += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);

	long nextheight = Lines[nextindex].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[nextindex].GetLyricsLine(),param);
	for (size_t i = nextindex + 1;i < nextindex + nextlines;i++)
		nextheight += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);


	int current_y = 0;
	long scrollheight = 0;
	switch (param.VAlignment)
	{
	case VA_Top:
		current_y = target_rect.y1 + param.TopMargin;
		scrollheight = currentheight;
		break;
	case VA_Center:
		current_y = target_rect.y1 + param.TopMargin + (target_rect.height() - (param.TopMargin + param.BottomMargin) - currentheight) / 2;
		scrollheight = (nextheight + currentheight) / 2;
		break;
	case VA_Bottom:
		current_y = target_rect.y2 - param.BottomMargin - currentheight;
		scrollheight = nextheight;
		break;
	}
	current_y += param.CurrentLineYOffset;

	long scroll_y = long(scrollheight *  scroll_rate);
	if (param.CurrentLineBackColor.a > 0)
	{
		long cy = current_y - scroll_y - param.LineSpace;
		long ny = cy + currentheight;
		long cy2 = ny + param.LineSpace;
		long ny2 = ny + nextheight + param.LineSpace;
		
		Juna::Picture::Rect rect(target_rect.x1,
								 long(cy - (cy - ny) * fade_rate) - param.CurrentLineBackPlusUp,
								 target_rect.x2,
								 long(cy2 - (cy2 - ny2) * fade_rate) + param.CurrentLineBackPlusDown);
		if (rect.SetIntersectRect(rect,target_rect))
			Juna::Picture::PreMultipliedAlpha::TranslucentFillColor(target,param.CurrentLineBackColor,param.CurrentLineBackColor.a + 1,&rect);
	}


//現在行
	int y = current_y - scroll_y;
	for (size_t i = index;i < index + currentlines;i++)
	{
		if (playtime_ms >= Lines[i].endtime)
		{
			if (playtime_ms < Lines[i].endtime + param.FadeTime)
			{
				float rate = float(playtime_ms - Lines[i].endtime) / param.FadeTime;
				Juna::Picture::Color c = BlendColor(param.OtherTextColor,param.CurrentTextColor,rate);
				Juna::Picture::Color oc = BlendColor(param.OtherTextOutlineColor,param.CurrentTextOutlineColor,rate);
				DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
			}
			else
			{
				DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
			}
		}
		else
			DrawActiveLine(target, target_rect, y, drawwidth, Lines[i], playtime_ms,param);
		y += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
	}
//過去行
	y = current_y - scroll_y;
	for ( int i = index - 1 ; i >= 0 && y > param.TopMargin ; i-- )
	{
		y -= Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
		if (playtime_ms >= Lines[i].endtime)
		{
			if (playtime_ms < Lines[i].endtime + param.FadeTime)
			{
				float rate = float(playtime_ms - Lines[i].endtime) / param.FadeTime;
				Juna::Picture::Color c = BlendColor(param.OtherTextColor,param.CurrentTextColor,rate);
				Juna::Picture::Color oc = BlendColor(param.OtherTextOutlineColor,param.CurrentTextOutlineColor,rate);
				DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
			}
			else
			{
				DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
			}
		}
		else
			DrawActiveLine(target, target_rect, y, drawwidth, Lines[i], playtime_ms,param);

	}

//未来行
	y = current_y - scroll_y + currentheight;
	for ( size_t i = nextindex ; i < Lines.size() && y < target.height() - param.BottomMargin ; i++ )
	{
		if (playtime_ms >= Lines[i].starttime - param.FadeTime)
		{
			float rate = float(playtime_ms + param.FadeTime - Lines[i].starttime) / param.FadeTime;
			Juna::Picture::Color c = BlendColor(param.StandbyTextColor,param.OtherTextColor,rate);
			Juna::Picture::Color oc = BlendColor(param.StandbyTextOutlineColor,param.OtherTextOutlineColor,rate);
			DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
		}
		else
		{
			DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
		}
		y += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
	}
}

void DrawCSKaraoke(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const KaraokeContainer &karaoke,const LyricsDrawParameter &param,int playtime_ms)
{
	const KaraokeContainer::KaraokeLines &Lines = karaoke.GetLines();
	if (Lines.empty())
		return;

	size_t index;
	for ( index = 0 ; index < Lines.size() - 1 ; index++ )
	{
		if ( playtime_ms < Lines[index + 1].starttime )
			break;
	}
	if ( index == Lines.size() - 1 )
		return;

	size_t nextindex = index + 1;

	size_t currentlines = 1;
	if (index > 0)
	{
		for (size_t i = index - 1;Lines[i].starttime == Lines[index].starttime;i--)
		{
			currentlines++;
			if (i == 0)
				break;
		}
		index -= (currentlines - 1);
	}
	size_t nextlines = 1;
	if (nextindex + 1 < Lines.size())
	{
		for (size_t i = nextindex + 1;Lines[i].starttime == Lines[nextindex].starttime;i++)
		{
			nextlines++;
		}
	}


	float fscroll_y = 0;
	float scroll_rate = 0;

	int next = Lines[nextindex].starttime;

	int scroll = next - Lines[index].starttime;
	int sub = next - playtime_ms;
	fscroll_y = 1.0f - float(sub) / scroll;

	if ( scroll > param.FadeTime )
	{
		if ( sub > param.FadeTime )
			scroll_rate = 0;
		else
			scroll_rate = 1.0f - sub / (float)param.FadeTime;
	}
	else
		scroll_rate = 1.0f - (float)sub / scroll;

	//ライン幅規制
	int drawwidth = target_rect.width() - param.LeftMargin - param.RightMargin;

	long currentheight = Lines[index].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[index].GetLyricsLine(),param);
	for (size_t i = index + 1;i < index + currentlines;i++)
		currentheight += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);

	long nextheight = Lines[nextindex].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[nextindex].GetLyricsLine(),param);
	for (size_t i = nextindex + 1;i < nextindex + nextlines;i++)
		nextheight += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);

	int current_y = 0;
	switch (param.VAlignment)
	{
	case VA_Top:
		current_y = target_rect.y1 + param.TopMargin;
		break;
	case VA_Center:
		current_y = target_rect.y1 + param.TopMargin + (target_rect.height() - (param.TopMargin + param.BottomMargin)) / 2;
		break;
	case VA_Bottom:
		current_y = target_rect.y2 - param.BottomMargin;
		break;
	}
	current_y += param.CurrentLineYOffset;

	long scroll_y = long(currentheight *  fscroll_y);

	if (param.CurrentLineBackColor.a > 0)
	{
		long cy = current_y - scroll_y - param.LineSpace;
		long ny = cy + currentheight;
		long cy2 = ny + param.LineSpace;
		long ny2 = ny + nextheight + param.LineSpace;
		
		Juna::Picture::Rect rect(target_rect.x1,
								 long(cy - (cy - ny) * scroll_rate) - param.CurrentLineBackPlusUp,
								 target_rect.x2,
								 long(cy2 - (cy2 - ny2) * scroll_rate) + param.CurrentLineBackPlusDown);
		if (rect.SetIntersectRect(rect,target_rect))
			Juna::Picture::PreMultipliedAlpha::TranslucentFillColor(target,param.CurrentLineBackColor,param.CurrentLineBackColor.a + 1,&rect);
	}



//現在行
	int y = current_y - scroll_y;
	for (size_t i = index;i < index + currentlines;i++)
	{
		if (playtime_ms >= Lines[i].endtime)
		{
			if (playtime_ms < Lines[i].endtime + param.FadeTime)
			{
				float rate = float(playtime_ms - Lines[i].endtime) / param.FadeTime;
				Juna::Picture::Color c = BlendColor(param.OtherTextColor,param.CurrentTextColor,rate);
				Juna::Picture::Color oc = BlendColor(param.OtherTextOutlineColor,param.CurrentTextOutlineColor,rate);
				DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
			}
			else
			{
				DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
			}
		}
		else
			DrawActiveLine(target, target_rect, y, drawwidth, Lines[i], playtime_ms,param);
		y += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
	}
//過去行
	y = current_y - scroll_y;
	for ( int i = index - 1 ; i >= 0 && y > param.TopMargin ; i-- )
	{
		y -= Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
		if (playtime_ms >= Lines[i].endtime)
		{
			if (playtime_ms < Lines[i].endtime + param.FadeTime)
			{
				float rate = float(playtime_ms - Lines[i].endtime) / param.FadeTime;
				Juna::Picture::Color c = BlendColor(param.OtherTextColor,param.CurrentTextColor,rate);
				Juna::Picture::Color oc = BlendColor(param.OtherTextOutlineColor,param.CurrentTextOutlineColor,rate);
				DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
			}
			else
			{
				DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
			}
		}
		else
			DrawActiveLine(target, target_rect, y, drawwidth, Lines[i], playtime_ms,param);

	}

//未来行
	y = current_y - scroll_y + currentheight;
	for ( size_t i = nextindex ; i < Lines.size() && y < target.height() - param.BottomMargin ; i++ )
	{
		if (playtime_ms >= Lines[i].starttime - param.FadeTime)
		{
			float rate = float(playtime_ms + param.FadeTime - Lines[i].starttime) / param.FadeTime;
			Juna::Picture::Color c = BlendColor(param.StandbyTextColor,param.OtherTextColor,rate);
			Juna::Picture::Color oc = BlendColor(param.StandbyTextOutlineColor,param.OtherTextOutlineColor,rate);
			DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
		}
		else
		{
			DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
		}
		y += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
	}
}

}//namespace ScrollDraw	
}//namespace Lyrics
}//namespace Juna

