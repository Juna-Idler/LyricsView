
#ifndef Juna_Picture_Image_ElasticBlit_H
#define Juna_Picture_Image_ElasticBlit_H

#include "Blit.h"

namespace Juna {
namespace Picture {

//êLèkì]ëó
void ElasticBlit(I_Image &rDst,const I_Image &iSrc,
				 const Rect *iDstRect = 0,const Rect *iSrcRect = 0);
void ElasticAvBlit(I_Image &rDst,const I_Image &iSrc,
				 const Rect *iDstRect = 0,const Rect *iSrcRect = 0);


}//namespace Picture
}//namespace Juna

#endif
