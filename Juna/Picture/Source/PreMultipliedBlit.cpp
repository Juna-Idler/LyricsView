
#include "../Blit.h"

#include <string.h>
#include <algorithm>
//memcpy

namespace Juna {
namespace Picture {
namespace PreMultipliedAlpha {


void ConvertFromStraight(I_Image &straight_image)
{
	int width = straight_image.width();
	int height = straight_image.height();
	ARGB *dst = straight_image[0];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			ARGB &dpix = dst[j];
			const int rate = dpix.a + (dpix.a > 0);
			dpix.r = dpix.r * rate >> 8;
			dpix.g = dpix.g * rate >> 8;
			dpix.b = dpix.b * rate >> 8;
		}
		dst += straight_image.distance();
	}
}


void BlitFromStraight(I_Image &pma_dest,const I_Image &straight_src,
					  int dest_x,int dest_y,const Rect *src_rect)
{
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,dest_x,dest_y,src_rect,pma_dest,straight_src);

	ARGB *dst = pma_dest[dy];
	const ARGB *src = straight_src[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			const ARGB &spix = src[j + sx];
			ARGB &dpix = dst[j + dx];
			const int rate = spix.a + (spix.a > 0);
			dpix.r = spix.r * rate >> 8;
			dpix.g = spix.g * rate >> 8;
			dpix.b = spix.b * rate >> 8;
			dpix.a = spix.a;
		}
		dst += pma_dest.distance();
		src += straight_src.distance();
	}
}

void TranslucentCopy(I_Image &pma_dest,const I_Image &pma_src,int rate,
					 int dest_x,int dest_y,const Rect *src_rect)
{
	if (rate >= 256)
	{
		Copy(pma_dest,pma_src,dest_x,dest_y,src_rect);
		return;
	}
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,dest_x,dest_y,src_rect,pma_dest,pma_src);
	if (rate <= 0)
	{
		Rect r(dx,dy,Size(width,height));
		Picture::FillColor(pma_dest,ARGB(0,0,0,0),&r);
		return;
	}

	ARGB *dst = pma_dest[dy];
	const ARGB *src = pma_src[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			const ARGB &spix = src[j + sx];
			ARGB &dpix = dst[j + dx];
			dpix.r = spix.r * rate >> 8;
			dpix.g = spix.g * rate >> 8;
			dpix.b = spix.b * rate >> 8;
			dpix.a = spix.a * rate >> 8;
		}
		dst += pma_dest.distance();
		src += pma_src.distance();
	}
}
void ConvertTranslucent(I_Image &pma_dest,int rate,const Rect *dest_rect)
{
	if (rate >= 256)
		return;
	if (rate <= 0)
	{
		Picture::FillColor(pma_dest,ARGB(0,0,0,0),dest_rect);
		return;
	}

	int dx,dy,height,width;
	ClipRect(dx,dy,width,height,pma_dest,dest_rect);

	ARGB *dst = pma_dest[dy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			ARGB &dpix = dst[j + dx];
			dpix.r = dpix.r * rate >> 8;
			dpix.g = dpix.g * rate >> 8;
			dpix.b = dpix.b * rate >> 8;
			dpix.a = dpix.a * rate >> 8;
		}
		dst += pma_dest.distance();
	}
}

void Blit(I_Image &pma_dest,const I_Image &pma_src,
		  int dest_x,int dest_y,const Rect *src_rect)
{
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,dest_x,dest_y,src_rect,pma_dest,pma_src);

	ARGB *dst = pma_dest[dy];
	const ARGB *src = pma_src[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			const ARGB &spix = src[j + sx];
			ARGB &dpix = dst[j + dx];
			const int rate = spix.a + (spix.a > 0);
			dpix.r = (dpix.r * (256 - rate) >> 8) + spix.r;
			dpix.g = (dpix.g * (256 - rate) >> 8) + spix.g;
			dpix.b = (dpix.b * (256 - rate) >> 8) + spix.b;
			dpix.a = (dpix.a * (256 - rate) >> 8) + spix.a;
		}
		dst += pma_dest.distance();
		src += pma_src.distance();
	}
}


void TranslucentBlit(I_Image &pma_dest,const I_Image &pma_src,int rate,
					 int dest_x,int dest_y,const Rect *src_rect)
{
	if (rate >= 256)
	{
		PreMultipliedAlpha::Blit(pma_dest,pma_src,dest_x,dest_y,src_rect);
		return;
	}
	if (rate <= 0)
		return;
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,dest_x,dest_y,src_rect,pma_dest,pma_src);

	ARGB *dst = pma_dest[dy];
	const ARGB *src = pma_src[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			const ARGB &spix = src[j + sx];
			ARGB &dpix = dst[j + dx];
//*
			dpix.r = ((dpix.r << 8) + (spix.r - dpix.r) * rate) >> 8;
			dpix.g = ((dpix.g << 8) + (spix.g - dpix.g) * rate) >> 8;
			dpix.b = ((dpix.b << 8) + (spix.b - dpix.b) * rate) >> 8;
			dpix.a = ((dpix.a << 8) + (spix.a - dpix.a) * rate) >> 8;
/*/
			dpix.r = (dpix.r * (256 - rate) >> 8) + (spix.r * rate >> 8);
			dpix.g = (dpix.g * (256 - rate) >> 8) + (spix.g * rate >> 8);
			dpix.b = (dpix.b * (256 - rate) >> 8) + (spix.b * rate >> 8);
			dpix.a = (dpix.a * (256 - rate) >> 8) + (spix.a * rate >> 8);
//*/
		}
		dst += pma_dest.distance();
		src += pma_src.distance();
	}
}


void TranslucentFillColor(I_Image &pma_dest,Color color,int rate,const Rect *dest_rect)
{
	if (rate >= 256)
	{
		PreMultipliedAlpha::FillColor(pma_dest,color,dest_rect);
		return;
	}
	else if (rate <= 0)
		return;

	int dx,dy,width,height;
	ClipRect(dx,dy,width,height,pma_dest,dest_rect);

	ARGB *dst = pma_dest[dy];
	for (int i = 0;i < height;i++,dst += pma_dest.distance())
		for (int j = 0;j < width;j++)
		{
			ARGB &dargb = dst[j + dx];
			dargb.r = (dargb.r * (256 - rate) >> 8) + (color.r * rate >> 8);
			dargb.g = (dargb.g * (256 - rate) >> 8) + (color.g * rate >> 8);
			dargb.b = (dargb.b * (256 - rate) >> 8) + (color.b * rate >> 8);
			dargb.a = (dargb.a * (256 - rate) >> 8) + (255 * rate >> 8);
		}
}
void TranslucentFillColor(I_Image &pma_dest,ARGB pma_color,const Rect *dest_rect)
{
	if (pma_color.a == 255)
	{
		Picture::FillColor(pma_dest,pma_color,dest_rect);
		return;
	}
	else if (pma_color.a == 0)
		return;

	int dx,dy,width,height;
	ClipRect(dx,dy,width,height,pma_dest,dest_rect);

	const int rate = pma_color.a + (pma_color.a > 0);
	ARGB *dst = pma_dest[dy];
	for (int i = 0;i < height;i++,dst += pma_dest.distance())
		for (int j = 0;j < width;j++)
		{
			ARGB &dpix = dst[j + dx];
			dpix.r = (dpix.r * (256 - rate) >> 8) + pma_color.r;
			dpix.g = (dpix.g * (256 - rate) >> 8) + pma_color.g;
			dpix.b = (dpix.b * (256 - rate) >> 8) + pma_color.b;
			dpix.a = (dpix.a * (256 - rate) >> 8) + pma_color.a;

		}
}



}//namespace PreMultipliedAlpha
}//namespace Picture
}//namespace Juna

