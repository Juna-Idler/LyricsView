
#include "../ExpandedFont.h"
#include "../Blit.h"

#include <windows.h>
#include <assert.h>


namespace Juna {
namespace Picture {


bool BufferedExpandedFont::Initialize(const BufferedFont &font,unsigned slip,ExpandDirect ed)
{
	Terminalize();
	if (!font.IsValid())
		return false;
	BaseFont = &font;
	Slip = slip;
	EDirect = ed;
	return true;
}

void BufferedExpandedFont::Terminalize(void)
{
	Slip = 0;
	EDirect = ED_Null;
	BaseFont = 0;

	for (map::iterator it = Buffer->begin();it != Buffer->end();++it)
	{
		delete it->second;
	}
	Buffer->clear();
}


short BufferedExpandedFont::DrawCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
		return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + image.xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImageColor(target,image.Image,color,dx,dy,&rect);
	return image.CellIncX;
}
short BufferedExpandedFont::DrawCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
		return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + image.xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImageAlpha(target,image.Image,dx,dy,&rect);
	return image.CellIncX;
}
short BufferedExpandedFont::DrawCharPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
		return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + image.xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImagePreMultipliedAlpha(target,image.Image,color,dx,dy,&rect);
	return image.CellIncX;
}

short BufferedExpandedFont::DrawCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
		return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + image.xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2GrayBlit(target,image.Image,dx,dy,&rect);
	return image.CellIncX;
}



short BufferedExpandedFont::DrawFirstCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
		return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;
	int xoffset = image.xoffset;
	int cellincx = image.CellIncX;
	if (xoffset + Slip < 0)
	{
		cellincx += -(xoffset + Slip);
		xoffset -= xoffset + Slip;
	}

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImageColor(target,image.Image,color,dx,dy,&rect);
	return cellincx;
}
short BufferedExpandedFont::DrawFirstCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
		return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;
	int xoffset = image.xoffset;
	int cellincx = image.CellIncX;
	if (xoffset + Slip < 0)
	{
		cellincx += -(xoffset + Slip);
		xoffset -= xoffset + Slip;
	}

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImageAlpha(target,image.Image,dx,dy,&rect);
	return cellincx;
}
short BufferedExpandedFont::DrawFirstCharPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
		return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;
	int xoffset = image.xoffset;
	int cellincx = image.CellIncX;
	if (xoffset + Slip < 0)
	{
		cellincx += -(xoffset + Slip);
		xoffset -= xoffset + Slip;
	}

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImagePreMultipliedAlpha(target,image.Image,color,dx,dy,&rect);
	return cellincx;
}

short BufferedExpandedFont::DrawFirstCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
		return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;
	int xoffset = image.xoffset;
	int cellincx = image.CellIncX;
	if (xoffset + Slip < 0)
	{
		cellincx += -(xoffset + Slip);
		xoffset -= xoffset + Slip;
	}

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2GrayBlit(target,image.Image,dx,dy,&rect);
	return cellincx;
}




bool BufferedExpandedFont::SetBuffer(wchar_t c) const
{
	const FontImage *base_image = BaseFont->GetFontImage(c);
	if (!base_image)
		return false;
	const I_GrayImage &b = base_image->Image;

	FontImage *image = new FontImage();

	GrayImage &t = image->Image;
	if (b.width() == 0 || b.height() == 0)
	{
	}
	else
	{
		t.Initialize(b.width() + Slip * 2,b.height() + Slip * 2);
		GrayFill(t,0);

		Gray2GrayAlpha(t,b,Slip,Slip);

		int naname = (Slip * 1000) / 1414;
		if (EDirect | ED_UpLeft)	Gray2GrayAlpha(t,b,Slip - naname, Slip - naname);
		if (EDirect | ED_Up)		Gray2GrayAlpha(t,b,Slip,0);
		if (EDirect | ED_UpRight)	Gray2GrayAlpha(t,b,Slip + naname, Slip - naname);
		if (EDirect | ED_Left)		Gray2GrayAlpha(t,b,0,Slip);
		if (EDirect | ED_Right)		Gray2GrayAlpha(t,b,Slip * 2,Slip);
		if (EDirect | ED_DownLeft)	Gray2GrayAlpha(t,b, Slip - naname,Slip + naname);
		if (EDirect | ED_Down)		Gray2GrayAlpha(t,b,Slip,Slip * 2);
		if (EDirect | ED_DownRight)	Gray2GrayAlpha(t,b,Slip + naname,Slip + naname);
	}
	image->xoffset = base_image->xoffset - Slip;
	image->yoffset = base_image->yoffset - Slip;
	image->CellIncX = base_image->CellIncX + Slip * 2;

	(*Buffer)[c] = image;
	return true;
}

const FontImage *BufferedExpandedFont::GetFontImage(wchar_t c) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
			return 0;
		it = Buffer->find(c);
	}
	return it->second;
}

}//namespace Picture
}//namespace Juna

