
#ifndef Juna_Picture_Image_Blit_H
#define Juna_Picture_Image_Blit_H

#include "Image.h"
#include "../2d_coordinate.h"

namespace Juna {
namespace Picture {

typedef D2::Point<long> Point;
typedef D2::Rectangle<long> Rect;
typedef D2::Size<long> Size;


//単純転送
void Copy(I_Image &rDst,const I_Image &iSrc,
		  int iDstX = 0,int iDstY = 0,const Rect *iSrcRect = 0);
//ピクセル単位アルファ転送
void Blit(I_Image &rDst,const I_Image &iSrc,
		  int iDstX = 0,int iDstY = 0,const Rect *iSrcRect = 0);

//半透明転送
void TranslucentBlit(I_Image &rDst,const I_Image &iSrc,int iRate,
					 int iDstX = 0,int iDstY = 0,const Rect *iSrcRect = 0);


//	塗りつぶし
void FillColor(I_Image &rDst,ARGB iColor,const Rect *iDstRect = 0);
//	特定色との半透明転送
void TranslucentFillColor(I_Image &rDst,Color iColor,int iRate,const Rect *iDstRect = 0);
//	特定の色要素を塗りつぶし
//	dest = (dest & iMask.argb) | iColor.argb;
void FillColor(I_Image &rDst,ARGB iColor,ARGB iMask,const Rect *iDstRect = 0);


namespace PreMultipliedAlpha {

//ストレートアルファからプリマルチプライドアルファに変換
	void ConvertFromStraight(I_Image &straight_image);

//ストレートアルファからプリマルチプライドアルファ化してコピー
	void CopyFromStraight(I_Image &pma_dest,const I_Image &straight_src,
						  int dest_x = 0,int dest_y = 0,const Rect *src_rect = 0);
//半透明化してコピー
	void TranslucentCopy(I_Image &pma_dest,const I_Image &pma_src,int rate,
						 int dest_x = 0,int dest_y = 0,const Rect *src_rect = 0);
//指定の半透明化状態に変換する
	void ConvertTranslucent(I_Image &pma_image,int rate,const Rect *dest_rect = 0);

	void Blit(I_Image &pma_dest,const I_Image &pma_src,
			  int dest_X = 0,int dest_y = 0,const Rect *src_rect = 0);

	void TranslucentBlit(I_Image &pma_dest,const I_Image &pma_src,int rate,
						 int dest_X = 0,int dest_y = 0,const Rect *src_rect = 0);

	//	塗りつぶし
	inline void FillColor(I_Image &pma_dest,ARGB color,const Rect *dest_rect = 0)
	{
		ARGB c(color.a,color.r * color.a / 255,color.g * color.a / 255,color.b * color.a / 255);
		Juna::Picture::FillColor(pma_dest,c,dest_rect);
	}
	//	特定色の半透明転送
	void TranslucentFillColor(I_Image &pma_dest,Color color,int rate,const Rect *dest_rect = 0);

	//	特定のPMAな色の半透明転送
	void TranslucentFillColor(I_Image &pma_dest,ARGB pma_color,const Rect *dest_rect = 0);

}//namespace PreMultipliedBlit


/*
	@pre rDst.width() == iSrc.width() && rDst.height() == iSrc.height()
	@param iSrc1Rate iSrc1の合成比率(0-256) (src2rate = 256 - rate)
*/
void MixBlit(I_Image &rDst,const I_Image &iSrc,int iRate,ARGB iColor,
			 int iDstX = 0,int iDstY = 0,const Rect *iSrcRect = 0);
/*
	@param iSrc1Rate iSrc1の合成比率(0-256) (src2rate = 256 - rate)
*/
void MixFadeBlit(I_Image &rDst,const I_Image &iSrc1,const I_Image &iSrc2,
				 int iSrc1Rate);


class I_ImageBliter
{
public:
	virtual void Blit(I_Image &rDst,const I_Image &iSrc,
					  int iDstX,int iDstY,const Rect *iSrcRect) = 0;
};

class I_ImageRateBliter
{
public:
	virtual void RateBlit(I_Image &rDst,const I_Image &iSrc,int iRate,
						  int iDstX,int iDstY,const Rect *iSrcRect) = 0;
};




void GrayFill(I_GrayImage &rDst,unsigned char gray,const Rect *iDstRect = 0);

void Gray2GrayBlit(I_GrayImage &rDst,const I_GrayImage &iSrc,
				   int iDstX = 0,int iDstY = 0,const Rect *iSrcRect = 0);

void Gray2GrayAlpha(I_GrayImage &rDst,const I_GrayImage &iSrc,
					int iDstX = 0,int iDstY = 0,const Rect *iSrcRect = 0);



void Gray2ImageAlpha(I_Image &rDst,const I_GrayImage &iSrc,
					 int iDstX = 0,int iDstY = 0,const Rect *iSrcRect = 0);
void Gray2ImageColor(I_Image &rDst,const I_GrayImage &iSrc,Color iColor,
					 int iDstX = 0,int iDstY = 0,const Rect *iSrcRect = 0);

void Gray2ImageStraightAlpha(I_Image &straight_dest,const I_GrayImage &iSrc,Color iColor,
							 int iDstX = 0,int iDstY = 0,const Rect *iSrcRect = 0);

void Gray2ImagePreMultipliedAlpha(I_Image &pma_dest,const I_GrayImage &iSrc,Color iColor,
								  int iDstX = 0,int iDstY = 0,const Rect *iSrcRect = 0);



template<class TDestImage,class TSrcImage>
inline void ClipRect(int &dx,int &dy,int &sx,int &sy,int &width,int &height,
					 int x,int y,const Rect *srcrect,
					 const TDestImage &dest,const TSrcImage &src)
{
	Rect rect;
	if (!srcrect)
	{
		rect.Set(0,0,src.width(),src.height());
		srcrect = &rect;
	}
	if (x < 0)
	{dx = 0;sx = srcrect->x1 - x;}
	else
	{dx = x;sx = srcrect->x1;}

	if (y < 0)
	{dy = 0;sy = srcrect->y1 - y;}
	else
	{dy = y;sy = srcrect->y1;}

	if (dest.width() - dx < srcrect->x2 - sx)
		width = dest.width() - dx;
	else
		width = srcrect->x2 - sx;

	if (dest.height() - dy < srcrect->y2 - sy)
		height = dest.height() - dy;
	else
		height = srcrect->y2 - sy;
}
template<class TImage>
inline void ClipRect(int &dx,int &dy,int &width,int &height,
					 const TImage &dst,const Rect *dstrect)
{
	dx = 0;
	dy = 0;
	width = dst.width();
	height = dst.height();

	if (dstrect)
	{
		if (dstrect->x1 > 0)
			dx = dstrect->x1;
		if (dstrect->y1 > 0)
			dy = dstrect->y1;
		if (dstrect->x2 < dst.width())
			width = dstrect->x2;
		width -= dx;
		if (dstrect->y2 < dst.height())
			height = dstrect->y2;
		height -= dy;
	}
}

template<class TImage>
inline void ClipSourceRect(Rect &src_rect,int &dx,int &dy,int x,int y,const TImage &src,const Rect *clip_rect)
{
	src_rect.Set(0,0,src.width(),src.height());
	dx = x;	dy = y;
	if (clip_rect)
	{
		if (x < clip_rect->x1)
		{
			src_rect.x1 = clip_rect->x1 - x;
			dx = clip_rect->x1;
		}
		if (y < clip_rect->y1)
		{
			src_rect.y1 = clip_rect->y1 - y;
			dy = clip_rect->y1;
		}
		if (x + src.width() > clip_rect->x2)
		{
			src_rect.x2 = clip_rect->x2 - x;
		}
		if (y + src.height() > clip_rect->y2)
		{
			src_rect.y2 = clip_rect->y2 - y;
		}
	}
}

}//namespace Picture
}//namespace Juna

#endif
