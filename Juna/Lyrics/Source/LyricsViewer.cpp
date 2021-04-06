
//#include <algorithm>

#include "../LyricsViewer.h"
#include "../TimeTag.h"

#include "../../Picture/Blit.h"
#include "../../Picture/ElasticBlit.h"

#include "../RubyKaraokeDraw.h"

namespace Juna {
namespace Lyrics {


void LyricsViewer::SetLyrics(const std::wstring &text)
{
	Lyrics.SetLyrics( text, Font.IsValid() ? &Font.GetFont() : 0 );
	LyricsMode = LM_Lyrics;
	Karaoke.Terminalize();
	Text.Terminalize();
	RubyKaraoke.Terminalize();
}

void LyricsViewer::SetKaraoke(const std::wstring &text,bool keephead)
{
	Karaoke.SetKaraoke( text, Font.IsValid() ? &Font.GetFont() : 0 , keephead );
	LyricsMode = LM_Karaoke;
	Lyrics.Terminalize();
	Text.Terminalize();
	RubyKaraoke.Terminalize();
}
void LyricsViewer::SetText(const std::wstring &text)
{
	Text.SetText( text, Font.IsValid() ? &Font.GetFont() : 0 );
	LyricsMode = LM_Text;
	Lyrics.Terminalize();
	Karaoke.Terminalize();
	RubyKaraoke.Terminalize();
}
void LyricsViewer::SetRubyKaraoke(const std::wstring &text,bool keephead)
{
	RubyKaraoke.SetRubyKaraoke( text, Font.IsValid() ? &Font.GetFont() : 0 , keephead );
	LyricsMode = LM_RubyKaraoke;
	Lyrics.Terminalize();
	Text.Terminalize();
	Karaoke.Terminalize();
}



bool LyricsViewer::SetFont(unsigned int fontquality, Juna::Picture::OutlineFont &font,int outlinethickness)
{
	if (!font.IsValid())
		return false;

	Juna::Picture::OutlineFont tmpfont;
	if (!tmpfont.Initialize(font,font.GetHeight()))
		return false;
	Juna::Picture::OutlineFont rubyfont;
	if (!rubyfont.Initialize(font,font.GetHeight() / 2))
		return false;

	int rubythickness = ((outlinethickness == 1) ? 1 : outlinethickness / 2);

	if (!(Font.Initialize(tmpfont, outlinethickness) && RubyFont.Initialize(rubyfont, rubythickness)))
	{
		Font.Terminalize();
		return false;
	}
	ForceRedraw = true;

	switch (LyricsMode)
	{
	case LM_Lyrics: Lyrics.SetFontWidth(Font.GetFont()); break;
	case LM_Karaoke: Karaoke.SetFontWidth(Font.GetFont()); break;
	case LM_Text: Text.SetFontWidth(Font.GetFont()); break;
	case LM_RubyKaraoke: RubyKaraoke.SetFontWidth(Font.GetFont()); break;
	}


	return Parameter.SetFont(&Font,&RubyFont);
}

void LyricsViewer::Resize(int width,int height)
{
	Canvas.Initialize(width,height);
	LyricsLayoutRect.Set(0,0,width,height);
	BGImageLayoutRect.Set(0,0,width,height);
	TextLayoutRect.Set(0,0,width,height);
	ForceRedraw = true;
}



bool LyricsViewer::Update(int playtime_ms)
{
	switch (LyricsMode)
	{
	case LM_Lyrics:
		{
			Juna::Lyrics::ScrollDraw::DrawPoint dp;
			if (!ScrollDraw::GetDrawPointLyrics(dp,Lyrics,Parameter,playtime_ms))
				return false;
			if ( ForceRedraw ||
				dp.index != LastDrawPoint.index || dp.scroll_y != LastDrawPoint.scroll_y || dp.scroll_rate != LastDrawPoint.scroll_rate)
			{
				FillBGColor();
				if (BGImage.SourceImage.isValid())
					BGImage.Update(Canvas,BGImageLayoutRect);

				ScrollDraw::DrawLyrics(Canvas,LyricsLayoutRect,Lyrics,Parameter,dp);
				LastDrawPoint = dp;

				if (!InfoText.GetText().GetLines().empty())
					DrawText(Canvas,TextLayoutRect,InfoText.Color,InfoText.OutlineColor,InfoText.GetText(),InfoText.Param);
				if (FrameImage.SourceImage.isValid())
					FrameImage.Draw(Canvas);

				ForceRedraw = false;

				return true;
			}
		}
		return false;
	case LM_Karaoke:
		{
			FillBGColor();
			if (BGImage.SourceImage.isValid())
				BGImage.Update(Canvas,BGImageLayoutRect);
			ScrollDraw::DrawKaraoke(Canvas,LyricsLayoutRect,Karaoke,Parameter,playtime_ms);
			if (!InfoText.GetText().GetLines().empty())
				DrawText(Canvas,TextLayoutRect,InfoText.Color,InfoText.OutlineColor,InfoText.GetText(),InfoText.Param);
			if (FrameImage.SourceImage.isValid())
				FrameImage.Draw(Canvas);
		}
		return true;
	case LM_Text:
		{
			FillBGColor();
			if (BGImage.SourceImage.isValid())
				BGImage.Update(Canvas,BGImageLayoutRect);

			DrawText(Canvas,LyricsLayoutRect, NTTTextColor, NTTTextOutlineColor, Text,Parameter);

			if (!InfoText.GetText().GetLines().empty())
				DrawText(Canvas,TextLayoutRect,InfoText.Color,InfoText.OutlineColor,InfoText.GetText(),InfoText.Param);
			if (FrameImage.SourceImage.isValid())
				FrameImage.Draw(Canvas);
		}
		return true;
	case LM_RubyKaraoke:
		{
			FillBGColor();
			if (BGImage.SourceImage.isValid())
				BGImage.Update(Canvas,BGImageLayoutRect);
			ScrollDraw::DrawRubyKaraoke(Canvas,LyricsLayoutRect,RubyKaraoke,Parameter,playtime_ms);
			if (!InfoText.GetText().GetLines().empty())
				DrawText(Canvas,TextLayoutRect,InfoText.Color,InfoText.OutlineColor,InfoText.GetText(),InfoText.Param);
			if (FrameImage.SourceImage.isValid())
				FrameImage.Draw(Canvas);
		}
		return true;
	}
	return false;
}
bool LyricsViewer::UpdateContinuousScroll(int playtime_ms)
{
	FillBGColor();
	if (BGImage.SourceImage.isValid())
		BGImage.Update(Canvas,BGImageLayoutRect);

	switch (LyricsMode)
	{
	case LM_Lyrics:
		ScrollDraw::DrawCSLyrics(Canvas,LyricsLayoutRect,Lyrics,Parameter,playtime_ms);break;
	case LM_Karaoke:
		ScrollDraw::DrawCSKaraoke(Canvas,LyricsLayoutRect,Karaoke,Parameter,playtime_ms);break;
	case LM_Text:
		DrawText(Canvas,LyricsLayoutRect, NTTTextColor, NTTTextOutlineColor, Text,Parameter);break;
	case LM_RubyKaraoke:
		ScrollDraw::DrawCSRubyKaraoke(Canvas,LyricsLayoutRect,RubyKaraoke,Parameter,playtime_ms);break;
	}

	if (!InfoText.GetText().GetLines().empty())
		DrawText(Canvas,TextLayoutRect,InfoText.Color,InfoText.OutlineColor,InfoText.GetText(),InfoText.Param);

	if (FrameImage.SourceImage.isValid())
		FrameImage.Draw(Canvas);

	return true;
}



bool LyricsViewer::UpdateKaraoke(int playtime_ms,int bottom_margin)
{
	FillBGColor();
	if (BGImage.SourceImage.isValid())
		BGImage.Update(Canvas,BGImageLayoutRect);

	switch (LyricsMode)
	{
	default:
		break;
	case LM_Karaoke:
		KDrawer.Draw(Canvas,LyricsLayoutRect,bottom_margin,Karaoke,Parameter,playtime_ms);break;
	}

	if (!InfoText.GetText().GetLines().empty())
		DrawText(Canvas,TextLayoutRect,InfoText.Color,InfoText.OutlineColor,InfoText.GetText(),InfoText.Param);

	if (FrameImage.SourceImage.isValid())
		FrameImage.Draw(Canvas);

	return true;

}



void LyricsViewer::C_BackGroundImage::Update(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect)
{
	int imagewidth = target_rect.width();
	int imageheight = target_rect.height();

	if (imagewidth <= 0 || imageheight <= 0)
		return;

	int targetwidth = INT_MAX;
	if (LimitDisplayX)
		targetwidth = imagewidth;
	if (LimitSourceX)
		targetwidth = SourceImage.width() < targetwidth ? SourceImage.width() : targetwidth;
	if (LimitMaxX)
		targetwidth = MaxX < targetwidth ? MaxX : targetwidth;
	if (targetwidth == INT_MAX)
		return;

	int targetheight = INT_MAX;
	if (LimitDisplayY)
		targetheight = imageheight;
	if (LimitSourceY)
		targetheight = SourceImage.height() < targetheight ? SourceImage.height() : targetheight;
	if (LimitMaxY)
		targetheight = MaxY < targetheight ? MaxY : targetheight;
	if (targetheight == INT_MAX)
		return;

	float targetrate = float(targetwidth) / targetheight;
	float bgrate = float(SourceImage.width()) / SourceImage.height();
	if (targetrate > bgrate)
	{
		if (targetheight != DisplayImage.height())
		{
			int width = SourceImage.width() * targetheight / SourceImage.height();
			DisplayImage.Initialize(width,targetheight);
			Juna::Picture::ElasticAvBlit(DisplayImage,SourceImage);
			if (Filter.a)
			{
				Juna::Picture::TranslucentFillColor(DisplayImage,Filter,Filter.a + (Filter.a == 255));
			}
			Juna::Picture::PreMultipliedAlpha::ConvertTranslucent(DisplayImage,TranslucentAlpha);
		}
	}
	else
	{
		if (targetwidth != DisplayImage.width())
		{
			int height = SourceImage.height() * targetwidth / SourceImage.width();
			DisplayImage.Initialize(targetwidth,height);
			Juna::Picture::ElasticAvBlit(DisplayImage,SourceImage);
			if (Filter.a)
			{
				Juna::Picture::TranslucentFillColor(DisplayImage,Filter,Filter.a + (Filter.a == 255));
			}
			Juna::Picture::PreMultipliedAlpha::ConvertTranslucent(DisplayImage,TranslucentAlpha);
		}
	}

	int bgx = target_rect.x1;
	int bgy = target_rect.y1;
	switch (Position % 3)
	{
	case 1: break;
	case 2: bgx += (imagewidth - DisplayImage.width()) / 2; break;
	case 0: bgx += imagewidth - DisplayImage.width(); break;
	}
	switch ((Position + 2) / 3)
	{
	case 3: break;
	case 2: bgy += (imageheight - DisplayImage.height()) / 2; break;
	case 1: bgy += imageheight - DisplayImage.height(); break;
	}

	Juna::Picture::Copy(target,DisplayImage,bgx,bgy);
}

bool LyricsViewer::C_InformationText::SetFont(unsigned int fontquality,Juna::Picture::OutlineFont &font, unsigned outlinethickness)
{
	if (!font.IsValid())
		return false;

	Juna::Picture::OutlineFont tmpfont;
	if (!tmpfont.Initialize(font,font.GetHeight()))
		return false;
//	Juna::Picture::Font rubyfont;
	if (!Font.Initialize(tmpfont,outlinethickness))
	{
		Font.Terminalize();
		return false;
	}

	Text.SetFontWidth(Font.GetFont());
	return Param.SetFont(&Font,0);
}

void LyricsViewer::C_InformationText::SetText(const std::wstring &text)
{
	if (Font.IsValid())
		Text.SetText(text,&Font.GetFont());
	else
		Text.SetText(text,0);
}


void LyricsViewer::C_FrameImage::Draw(Juna::Picture::I_Image &target)
{
	int width = SourceImage.width() / 2;
	int height = SourceImage.height() / 2;

	Juna::Picture::Rect rect(0,0,width,height);
	Juna::Picture::PreMultipliedAlpha::Blit(target,SourceImage,0,0,&rect);
	rect.x1 = SourceImage.width() - width; rect.x2 = SourceImage.width();
	Juna::Picture::PreMultipliedAlpha::Blit(target,SourceImage,target.width() - width,0,&rect);
	rect.y1 = SourceImage.height() - height; rect.y2 = SourceImage.height();
	Juna::Picture::PreMultipliedAlpha::Blit(target,SourceImage,target.width() - width,target.height() - height,&rect);
	rect.x1 = 0; rect.x2 = width;
	Juna::Picture::PreMultipliedAlpha::Blit(target,SourceImage,0,target.height() - height,&rect);

	int x_length = target.width() - width * 2;
	int y_length = target.height() - height * 2;

	for (int i = 0;i < height;i++)
	{
		Juna::Picture::Rect r(width,i,target.width() - width,i + 1);
		Juna::Picture::PreMultipliedAlpha::TranslucentFillColor(target,SourceImage[i][width],&r);

		r.y1 = target.height() - height + i; r.y2 = r.y1 + 1;
		Juna::Picture::PreMultipliedAlpha::TranslucentFillColor(target,SourceImage[SourceImage.height() - height + i][width],&r);
	}
	for (int i = 0;i < width;i++)
	{
		Juna::Picture::Rect r(i,height,i + 1,target.height() - height);
		Juna::Picture::PreMultipliedAlpha::TranslucentFillColor(target,SourceImage[height][i],&r);

		r.x1 = target.width() - width + i; r.x2 = r.x1 + 1;
		Juna::Picture::PreMultipliedAlpha::TranslucentFillColor(target,SourceImage[height][SourceImage.width() - width + i],&r);
	}

}

void LyricsViewer::FillBGColor(void)
{
	if (!FrameImage.SourceImage.isValid())
	{
		Juna::Picture::PreMultipliedAlpha::FillColor(Canvas,BGColor);
		return;
	}
	int width = FrameImage.SourceImage.width() / 4;
	int height = FrameImage.SourceImage.height() / 4;

	Juna::Picture::ARGB t(0,0,0,0);
	Juna::Picture::Rect r(0,0,Canvas.width(),height);
	Juna::Picture::FillColor(Canvas,t,&r);
	r.y1 = Canvas.height() - height; r.y2 = Canvas.height();
	Juna::Picture::FillColor(Canvas,t,&r);

	r.Set(0,height,width,Canvas.height() - height);
	Juna::Picture::FillColor(Canvas,t,&r);
	r.x1 = Canvas.width() - width; r.x2 = Canvas.width();
	Juna::Picture::FillColor(Canvas,t,&r);

	r.Set(width,height,Canvas.width() - width,Canvas.height() - height);
	Juna::Picture::PreMultipliedAlpha::FillColor(Canvas,BGColor,&r);
}

}//namespace Lyrics
}//namespace Juna
