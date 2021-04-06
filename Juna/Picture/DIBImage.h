
#ifndef Juna_Picture_DIB_Image_H
#define Juna_Picture_DIB_Image_H

#include "Image.h"


namespace Juna {
namespace Picture {


class DIBImage : public I_Image
{
public:
	DIBImage(void) : I_Image(),hDC(0),hBitmap(0),hOldBitmap(0) {}
	~DIBImage() {Terminalize();}

	bool Initialize(int width,int heigh);
	void Terminalize(void);


	void *GetDC(void) const {return hDC;}
	void *GetBitmap(void) const {return hBitmap;}

	void DeselectBitmap(void) const;
	void SelectBitmap(void) const;

private:
	void *hDC;
	void *hBitmap;
	void *hOldBitmap;
};

}//namespace Picture
}//namespace Juna

#endif
