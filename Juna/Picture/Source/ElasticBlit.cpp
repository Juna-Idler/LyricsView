
#include "../ElasticBlit.h"

#include <math.h>

namespace Juna {
namespace Picture {

ARGB GetBilinearColor(const I_Image &image,double x,double y)
{
	double rx = ::floor(x);
	double ry = ::floor(y);
	x -= rx;
	y -= ry;

	int ix = int(rx),iy = int(ry);
	int x0 = ix + (ix < 0);
	int y0 = iy + (iy < 0);
	int x1 = ix + (ix < image.width()-1);
	int y1 = iy + (iy < image.height()-1);
	ARGB c[2][2] = {
		{image[y0][x0],image[y0][x1]},
		{image[y1][x0],image[y1][x1]}
	};

	return ARGB(
		unsigned char((1.0 - y) * ( (1.0 - x) * c[0][0].a + x * c[0][1].a ) + y * ( (1.0 - x) * c[1][0].a + x * c[1][1].a)),
		unsigned char((1.0 - y) * ( (1.0 - x) * c[0][0].r + x * c[0][1].r ) + y * ( (1.0 - x) * c[1][0].r + x * c[1][1].r)),
		unsigned char((1.0 - y) * ( (1.0 - x) * c[0][0].g + x * c[0][1].g ) + y * ( (1.0 - x) * c[1][0].g + x * c[1][1].g)),
		unsigned char((1.0 - y) * ( (1.0 - x) * c[0][0].b + x * c[0][1].b ) + y * ( (1.0 - x) * c[1][0].b + x * c[1][1].b)));
}

//Lk“]‘—
void ElasticBlit(I_Image &rDst,const I_Image &iSrc,
				 const Rect *iDstRect,const Rect *iSrcRect)
{
	Rect drect,srect;
	if (iDstRect)
		drect = *iDstRect;
	else
		drect.Set(0,0,rDst.width(),rDst.height());
	if (iSrcRect)
		srect = *iSrcRect;
	else
		srect.Set(0,0,iSrc.width(),iSrc.height());

	ARGB *dst = rDst[drect.y1];

	double x_rate = double(drect.width()) / srect.width();
	double y_rate = double(drect.height()) / srect.height();

	for(int i = drect.y1; i < drect.y2; i++)
	{
		double y = (i - drect.y1 + 0.5) / y_rate + srect.y1 - 0.5;
		for(int j = drect.x1; j < drect.x2; j++)
		{
 			double x = (j - drect.x1 + 0.5) / x_rate + srect.x1 - 0.5;
			rDst[i][j] = GetBilinearColor(iSrc,x,y);
        }
    }

}



ARGB GetAverageColor(const I_Image &image,double x1,double x2,double y1,double y2)
{
	double x1r_;
	double x1_rate = 1 - ::modf(x1,&x1r_);
	int x1r = (int)x1r_;
	double x2r_;
	double x2_rate = ::modf(x2,&x2r_);
	int x2r = (int)x2r_;

	double y1r_;
	double y1_rate = 1 - ::modf(y1,&y1r_);
	int y1r = (int)y1r_;
	double y2r_;
	double y2_rate = ::modf(y2,&y2r_);
	int y2r = (int)y2r_;

	double count1 = 0;
	double count = 0;
	double count2 = 0;
	double r1 = 0,g1 = 0,b1 = 0;
	double r = 0,g = 0,b = 0;
	double r2 = 0,g2 = 0,b2 = 0;

	if (y1r == y2r)
	{
		if (x1r == x2r)
		{
			double x_rate = (x2_rate - (1 - x1_rate));
			ARGB c = image[(int)y1r][(int)x1r];
			r = c.r;
			g = c.g;
			b = c.b;
			count = 1;
		}
		else
		{
			ARGB c = image[(int)y1r][(int)x1r];
			r = c.r * x1_rate;
			g = c.g * x1_rate;
			b = c.b * x1_rate;

			count = x1_rate;

			for (int i = x1r;i < x2r;i++)
			{
				c = image[(int)y1r][i];
				r += c.r;
				g += c.g;
				b += c.b;
				count += 1;
			}
			if (x2_rate > 0)
			{
				c = image[(int)y1r][(int)x2r];
				r += c.r * x2_rate;
				g += c.g * x2_rate;
				b += c.b * x2_rate;
				count += x2_rate;
			}
		}

		r /= count;
		g /= count;
		b /= count;
	}
	else
	{
		if (x1r == x2r)
		{
			ARGB c = image[(int)y1r][(int)x1r];
			r1 = c.r;
			g1 = c.g;
			b1 = c.b;
			count1 = 1;
		}
		else
		{
			ARGB c = image[(int)y1r][(int)x1r];
			r1 = c.r * x1_rate;
			g1 = c.g * x1_rate;
			b1 = c.b * x1_rate;
			count1 = x1_rate;

			for (int i = x1r + 1;i < x2r;i++)
			{
				c = image[(int)y1r][i];
				r1 += c.r;
				g1 += c.g;
				b1 += c.b;
				count1 += 1;
			}
			if (x2_rate > 0)
			{
				c = image[(int)y1r][(int)x2r];
				r1 += c.r * x2_rate;
				g1 += c.g * x2_rate;
				b1 += c.b * x2_rate;
				count1 += x2_rate;
			}
		}

		for (int j = y1r + 1;j < y2r;j++)
		{
			if (x1r == x2r)
			{
				ARGB c = image[j][(int)x1r];
				r += c.r;
				g += c.g;
				b += c.b;
				count += 1;
			}
			else
			{
				ARGB c = image[j][(int)x1r];
				r += c.r * x1_rate;
				g += c.g * x1_rate;
				b += c.b * x1_rate;
				count += x1_rate;

				for (int i = x1r + 1;i < x2r;i++)
				{
					c = image[j][i];
					r += c.r;
					g += c.g;
					b += c.b;
					count += 1;
				}
				if (x2_rate > 0)
				{
					c = image[j][(int)x2r];
					r += c.r * x2_rate;
					g += c.g * x2_rate;
					b += c.b * x2_rate;
					count += x2_rate;
				}
			}
		}

		if (y2_rate > 0)
		{
			if (x1r == x2r)
			{
				ARGB c = image[(int)y2r][(int)x1r];
				r2 = c.r;
				g2 = c.g;
				b2 = c.b;
				count2 = 1;
			}
			else
			{
				ARGB c = image[(int)y2r][(int)x1r];
				r2 = c.r * x1_rate;
				g2 = c.g * x1_rate;
				b2 = c.b * x1_rate;
				count2 += x1_rate;

				for (int i = x1r + 1;i < x2r;i++)
				{
					c = image[(int)y2r][i];
					r2 += c.r;
					g2 += c.g;
					b2 += c.b;
					count2 += 1;
				}
				if (x2_rate > 0)
				{
					c = image[(int)y2r][(int)x2r];
					r2 += c.r * x2_rate;
					g2 += c.g * x2_rate;
					b2 += c.b * x2_rate;
					count2 += x2_rate;
				}
			}
		}
		r += r1 * y1_rate;
		g += g1 * y1_rate;
		b += b1 * y1_rate;
		r += r2 * y2_rate;
		g += g2 * y2_rate;
		b += b2 * y2_rate;

		r /= count + count1 * y1_rate + count2 * y2_rate;
		g /= count + count1 * y1_rate + count2 * y2_rate;
		b /= count + count1 * y1_rate + count2 * y2_rate;

	}


	return ARGB(
		255,
		unsigned char(r),
		unsigned char(g),
		unsigned char(b));
}

void ElasticAvBlit(I_Image &rDst,const I_Image &iSrc,
				 const Rect *iDstRect,const Rect *iSrcRect)
{
	Rect drect,srect;
	if (iDstRect)
		drect = *iDstRect;
	else
		drect.Set(0,0,rDst.width(),rDst.height());
	if (iSrcRect)
		srect = *iSrcRect;
	else
		srect.Set(0,0,iSrc.width(),iSrc.height());

	ARGB *dst = rDst[drect.y1];

	double x_rate = double(drect.width()) / srect.width();
	double y_rate = double(drect.height()) / srect.height();

	for(int i = drect.y1; i < drect.y2 -1; i++)
	{
		double y1 = (i - drect.y1) / y_rate + srect.y1;
		double y2 = y1 + 1 / y_rate;
		for(int j = drect.x1; j < drect.x2 - 1; j++)
		{
 			double x1 = (j - drect.x1) / x_rate + srect.x1;
			double x2 = x1 + 1 / x_rate;
			rDst[i][j] = GetAverageColor(iSrc,x1,x2,y1,y2);
        }
		{
 			double x1 = srect.x2 - 1 / x_rate;
			double x2 = srect.x2;
			rDst[i][drect.x2 - 1] = GetAverageColor(iSrc,x1,x2,y1,y2);
		}
	}
	{
		double y1 = srect.y2 - 1 / y_rate;
		double y2 = srect.y2;
		for(int j = drect.x1; j < drect.x2 - 1; j++)
		{
 			double x1 = (j - drect.x1) / x_rate + srect.x1;
			double x2 = x1 + 1 / x_rate;
			rDst[drect.y2 -1][j] = GetAverageColor(iSrc,x1,x2,y1,y2);
        }
 		double x1 = srect.x2 - 1 / x_rate;
		double x2 = srect.x2;
		rDst[drect.y2 -1][drect.x2 - 1] = GetAverageColor(iSrc,x1,x2,y1,y2);
	}

}

}//namespace Picture
}//namespace Juna
