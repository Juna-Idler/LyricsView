
#include "../Blit.h"

#include <string.h>
#include <algorithm>
//memcpy

namespace Juna {
namespace Picture {


void Copy(I_Image &rDst,const I_Image &iSrc,int iDstX,int iDstY,const Rect *iSrcRect)
{
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,iDstX,iDstY,iSrcRect,rDst,iSrc);

	ARGB *dst = rDst[dy];
	const ARGB *src = iSrc[sy];
	width *= sizeof(ARGB);
	for (int i = 0;i < height;i++)
	{
		::memcpy(dst + dx,src + sx,width);
		dst += rDst.distance();
		src += iSrc.distance();
//		::memcpy(rDst[i + dy] + dx,iSrc[i + sy] + sx,width);
//		for (int j = 0;j < width;j++)
//		{
//			dst[i + dy][j + dx] = src[i + sy][j + sx];
//		}
	}
}

void Blit(I_Image &rDst,const I_Image &iSrc,int iDstX,int iDstY,const Rect *iSrcRect)
{
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,iDstX,iDstY,iSrcRect,rDst,iSrc);

	ARGB *dst = rDst[dy];
	const ARGB *src = iSrc[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			const ARGB &spix = src[j + sx];
			ARGB &dpix = dst[j + dx];
			const int rate = spix.a + (spix.a != 0);
//*
			dpix.r = ((dpix.r << 8) + (spix.r - dpix.r) * rate) >> 8;
			dpix.g = ((dpix.g << 8) + (spix.g - dpix.g) * rate) >> 8;
			dpix.b = ((dpix.b << 8) + (spix.b - dpix.b) * rate) >> 8;
			dpix.a = (dpix.a * (256 - rate) >> 8) + spix.a;
/*/
			dpix.r = (dpix.r * (256 - rate) >> 8) + (spix.r * rate >> 8);
			dpix.g = (dpix.g * (256 - rate) >> 8) + (spix.g * rate >> 8);
			dpix.b = (dpix.b * (256 - rate) >> 8) + (spix.b * rate >> 8);
			dpix.a = (dpix.a * (256 - rate) >> 8) + spix.a;
//*/
		}
		dst += rDst.distance();
		src += iSrc.distance();
	}
}


void TranslucentBlit(I_Image &rDst,const I_Image &iSrc,int iRate,int iDstX,int iDstY,const Rect *iSrcRect)
{
	if (iRate >= 256)
	{
		Copy(rDst,iSrc,iDstX,iDstY,iSrcRect);
		return;
	}
	if (iRate <= 0)
		return;
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,iDstX,iDstY,iSrcRect,rDst,iSrc);

	ARGB *dst = rDst[dy];
	const ARGB *src = iSrc[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			const ARGB &spix = src[j + sx];
			ARGB &dpix = dst[j + dx];
//*
			dpix.r = ((dpix.r << 8) + (spix.r - dpix.r) * iRate) >> 8;
			dpix.g = ((dpix.g << 8) + (spix.g - dpix.g) * iRate) >> 8;
			dpix.b = ((dpix.b << 8) + (spix.b - dpix.b) * iRate) >> 8;
			dpix.a = ((dpix.a << 8) + (spix.a - dpix.a) * iRate) >> 8;
/*/
			dpix.r = (dpix.r * (256 - iRate) >> 8) + (spix.r * iRate >> 8);
			dpix.g = (dpix.g * (256 - iRate) >> 8) + (spix.g * iRate >> 8);
			dpix.b = (dpix.b * (256 - iRate) >> 8) + (spix.b * iRate >> 8);
			dpix.a = (dpix.a * (256 - iRate) >> 8) + (spix.a * iRate >> 8);
//*/
		}
		dst += rDst.distance();
		src += iSrc.distance();
	}
}


void FillColor(I_Image &rDst,ARGB iColor,const Rect *iDstRect)
{
	int dx,dy,width,height;
	ClipRect(dx,dy,width,height,rDst,iDstRect);

	ARGB *dst = rDst[dy];
	for (int i = 0;i < height;i++,dst += rDst.distance())
		for (int j = 0;j < width;j++)
			dst[j + dx] = iColor;
}

void TranslucentFillColor(I_Image &rDst,Color iColor,int iRate,const Rect *iDstRect)
{
	if (iRate >= 256)
	{
		FillColor(rDst,iColor,iDstRect);
		return;
	}
	else if (iRate <= 0)
		return;

	int dx,dy,width,height;
	ClipRect(dx,dy,width,height,rDst,iDstRect);

	ARGB *dst = rDst[dy];
	for (int i = 0;i < height;i++,dst += rDst.distance())
		for (int j = 0;j < width;j++)
		{
			ARGB &dpix = dst[j + dx];
			dpix.r = ((dpix.r << 8) + (iColor.r - dpix.r) * iRate) >> 8;
			dpix.g = ((dpix.g << 8) + (iColor.g - dpix.g) * iRate) >> 8;
			dpix.b = ((dpix.b << 8) + (iColor.b - dpix.b) * iRate) >> 8;
		}
}

void FillColor(I_Image &rDst,ARGB color,ARGB mask,const Rect *iDstRect)
{
	int dx,dy,width,height;
	ClipRect(dx,dy,width,height,rDst,iDstRect);

	color.a = color.a & ~mask.a;
	color.r = color.r & ~mask.r;
	color.g = color.g & ~mask.g;
	color.b = color.b & ~mask.b;

	ARGB *dst = rDst[dy];
	for (int i = 0;i < height;i++,dst += rDst.distance())
		for (int j = 0;j < width;j++)
		{
			ARGB &d = dst[j + dx];
			d.a = (d.a & mask.a) | color.a;
			d.r = (d.r & mask.r) | color.r;
			d.g = (d.g & mask.g) | color.g;
			d.b = (d.b & mask.b) | color.b;
		}
}




void MixBlit(I_Image &rDst,const I_Image &iSrc,int iRate,ARGB iColor,
			 int iDstX,int iDstY,const Rect *iSrcRect)
{
	if (iRate >= 256)
	{
		Copy(rDst,iSrc,iDstX,iDstY,iSrcRect);
		return;
	}
	if (iRate <= 0)
	{
		int width,height;
		if (iSrcRect)
			{width = iSrcRect->width();height = iSrcRect->height();}
		else
			{width = iSrc.width();height = iSrc.height();}
		const Rect rect(iDstX,iDstY,Size(width,height));
		FillColor(rDst,iColor,&rect);
		return;
	}

	int dx,dy,sx,sy,width,height;
	ClipRect(dx,dy,sx,sy,width,height,iDstX,iDstY,iSrcRect,rDst,iSrc);


	ARGB *dst = rDst[dy];
	const ARGB *src = iSrc[sy];

	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			ARGB &d = dst[j + dx];
			const ARGB &s = src[j + sx];
			d.r = ((iColor.r << 8) + (s.r - iColor.r) * iRate) >> 8;
			d.g = ((iColor.g << 8) + (s.g - iColor.g) * iRate) >> 8;
			d.b = ((iColor.b << 8) + (s.b - iColor.b) * iRate) >> 8;
//			d.a = ((iColor.a << 8) + (s.a - iColor.a) * iRate) >> 8;
		}
		dst += rDst.distance();
		src += iSrc.distance();
	}
}
void MixFadeBlit(I_Image &rDst,const I_Image &iSrc1,const I_Image &iSrc2,
				 int iSrc1Rate)
{
	if (iSrc1Rate >= 256)
	{
		Copy(rDst,iSrc1);return;
	}
	if (iSrc1Rate <= 0)
	{
		Copy(rDst,iSrc2);return;
	}

	int width = std::min(std::min(iSrc1.width(),iSrc2.width()),rDst.width());
	int height = std::min(std::min(iSrc1.height(),iSrc2.height()),rDst.height());

	ARGB *dst = rDst[0];
	const ARGB *src1 = iSrc1[0];
	const ARGB *src2 = iSrc2[0];

	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			ARGB &d = dst[j];
			const ARGB &s1 = src1[j];
			const ARGB &s2 = src2[j];
			d.r = ((s2.r << 8) + (s1.r - s2.r) * iSrc1Rate) >> 8;
			d.g = ((s2.g << 8) + (s1.g - s2.g) * iSrc1Rate) >> 8;
			d.b = ((s2.b << 8) + (s1.b - s2.b) * iSrc1Rate) >> 8;
//			d.a = ((s2.a << 8) + (s1.a - s2.a) * iSrc1Rate) >> 8;
		}
		dst += rDst.distance();
		src1 += iSrc1.distance();
		src2 += iSrc2.distance();
	}
}





void GrayFill(I_GrayImage &rDst,unsigned char gray,const Rect *iDstRect)
{
	int dx,dy,width,height;
	ClipRect(dx,dy,width,height,rDst,iDstRect);

	unsigned char *dst = rDst[dy];
	for (int i = 0;i < height;i++,dst += rDst.distance())
		for (int j = 0;j < width;j++)
			dst[j + dx] = gray;
}


void Gray2GrayBlit(I_GrayImage &rDst,const I_GrayImage &iSrc,
				   int iDstX,int iDstY,const Rect *iSrcRect)

{
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,iDstX,iDstY,iSrcRect,rDst,iSrc);

	unsigned char *dst = rDst[dy];
	const unsigned char *src = iSrc[sy];
	for (int i = 0;i < height;i++)
	{
		::memcpy(dst + dx,src + sx,width);
		dst += rDst.distance();
		src += iSrc.distance();
	}
}

void Gray2GrayAlpha(I_GrayImage &rDst,const I_GrayImage &iSrc,
					int iDstX,int iDstY,const Rect *iSrcRect)
{
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,iDstX,iDstY,iSrcRect,rDst,iSrc);

	unsigned char *dst = rDst[dy];
	const unsigned char *src = iSrc[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			int a = src[j + sx]; a += (a != 0);
			dst[j + dx] = (dst[j + dx] * (256 - a) >> 8) + src[j + sx];
		}
		dst += rDst.distance();
		src += iSrc.distance();
	}
}



void Gray2ImageAlpha(I_Image &rDst,const I_GrayImage &iSrc,
					 int iDstX,int iDstY,const Rect *iSrcRect)
{
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,iDstX,iDstY,iSrcRect,rDst,iSrc);

	ARGB *dst = rDst[dy];
	const unsigned char *src = iSrc[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			dst[j + dx].a = src[j + sx];
		}
		dst += rDst.distance();
		src += iSrc.distance();
	}
}
void Gray2ImageColor(I_Image &rDst,const I_GrayImage &iSrc,Color iColor,
					 int iDstX,int iDstY,const Rect *iSrcRect)
{
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,iDstX,iDstY,iSrcRect,rDst,iSrc);

	ARGB *dst = rDst[dy];
	const unsigned char *src = iSrc[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			int a = src[j + sx];
			a += (a != 0);
			ARGB &dpix = dst[j + dx];
			dpix.r = ((dpix.r << 8) + (iColor.r - dpix.r) * a) >> 8;
			dpix.g = ((dpix.g << 8) + (iColor.g - dpix.g) * a) >> 8;
			dpix.b = ((dpix.b << 8) + (iColor.b - dpix.b) * a) >> 8;
		}
		dst += rDst.distance();
		src += iSrc.distance();
	}
}

void Gray2ImageStraightAlpha(I_Image &rDst,const I_GrayImage &iSrc,Color iColor,
						  int iDstX,int iDstY,const Rect *iSrcRect)
{
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,iDstX,iDstY,iSrcRect,rDst,iSrc);

	ARGB c = iColor;
	ARGB *dst = rDst[dy];
	const unsigned char *src = iSrc[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			c.a = src[j + sx];
			dst[j + dx] = c;

			ARGB &dargb = dst[j + dx];
			unsigned rate = src[j + sx];rate += (rate > 0);
			dargb.a = (dargb.a * (256 - rate) >> 8) + (255 * rate >> 8);
			if (dargb.a > 0)
			{
				unsigned char da = dargb.a + 1;
				dargb.r = ((dargb.r * da * (256 - rate) >> 8) + (c.r * rate)) / da;
				dargb.g = ((dargb.g * da * (256 - rate) >> 8) + (c.g * rate)) / da;
				dargb.b = ((dargb.b * da * (256 - rate) >> 8) + (c.b * rate)) / da;
			}
		}
		dst += rDst.distance();
		src += iSrc.distance();
	}
}

void Gray2ImagePreMultipliedAlpha(I_Image &rDst,const I_GrayImage &iSrc,Color iColor,
						  int iDstX,int iDstY,const Rect *iSrcRect)
{
	int dx,dy,sx,sy,height,width;
	ClipRect(dx,dy,sx,sy,width,height,iDstX,iDstY,iSrcRect,rDst,iSrc);

	ARGB c = iColor;
	ARGB *dst = rDst[dy];
	const unsigned char *src = iSrc[sy];
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			ARGB &dargb = dst[j + dx];
			unsigned rate = src[j + sx];rate += (rate > 0);
			dargb.r = (dargb.r * (256 - rate) >> 8) + (c.r * rate >> 8);
			dargb.g = (dargb.g * (256 - rate) >> 8) + (c.g * rate >> 8);
			dargb.b = (dargb.b * (256 - rate) >> 8) + (c.b * rate >> 8);
			dargb.a = (dargb.a * (256 - rate) >> 8) + (255 * rate >> 8);


		}
		dst += rDst.distance();
		src += iSrc.distance();
	}
}

}//namespace Picture
}//namespace Juna

