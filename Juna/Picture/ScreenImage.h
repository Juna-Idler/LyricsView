
#ifndef Juna_Picture_ScreenImage_H
#define Juna_Picture_ScreenImage_H

#include "Image.h"
#include "DIBImage.h"

#include "../2d_coordinate.h"

namespace Juna {
namespace Picture {

typedef D2::Point<long> Point;
typedef D2::Rectangle<long> Rect;

//これを作ったウインドウのサイズを変更しUpdate()で全面を対象に描画します
class ScreenImage
{
public:
	ScreenImage(void) : Image(),hWindow(0) {}
	~ScreenImage() {Terminalize();}

	bool Initialize(int width,int height);
	void Terminalize(void);

	I_Image &GetImage(void) {return Image;}
	const I_Image &GetImage(void) const {return Image;}

	void *GetDC(void) {return Image.GetDC();}

	void Update(const Rect *rect = 0);

private:
	DIBImage Image;
	void *hWindow;
};

//make_private_screen = true で描画専用ウインドウを作ります
class ScreenImage2
{
public:
	ScreenImage2(void) : Image() {}
	~ScreenImage2() {Terminalize();}

	bool Initialize(int width,int height,bool make_private_screen = false);
	void Terminalize(void);

	I_Image &GetImage(void) {return Image;}
	const I_Image &GetImage(void) const {return Image;}

	bool IsPrivate(void) const {return Private;}
	void *GetDC(void) {return Image.GetDC();}

	void Update(const Rect *rect = 0);

private:
	DIBImage Image;
	void *hWindow;
	bool Private;
};

}//namespace Picture
}//namespace Juna

#endif
