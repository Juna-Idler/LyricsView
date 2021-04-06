
#ifndef Juna_Picture_Image_H
#define Juna_Picture_Image_H

#include <assert.h>

namespace Juna {
namespace Picture {

template<typename PixelType>
class basic_I_Image
{
public:
	int width(void) const {return Width;};
	int height(void) const {return Height;};
	int distance(void) const {return Distance;};

	PixelType *operator[](int y) {return Buffer + Distance * y;}
	const PixelType *operator[](int y) const {return Buffer + Distance * y;}

	bool isValid(void) const {return Buffer != 0;}
protected:
	basic_I_Image(void) : Buffer(0),Width(0),Height(0),Distance(0) {}
	~basic_I_Image() {}

	PixelType *Buffer;	//画素配列
	int Width;		//幅
	int Height;		//高さ
	int Distance;	//次のラインへの距離

//	デフォルト初期化
	void Initialize(int width,int height)
	{
		assert(width >= 0 && height >= 0);
		Terminalize();
		Buffer = new PixelType[width * height];
//		if (!Buffer)
//			throw std::bad_alloc();
		Distance = Width = width;
		Height = height;
	}
//	デフォルト破棄
	void Terminalize(void)
	{
		delete[] Buffer;
		Buffer = 0;
		Width = Height = Distance = 0;
	}
private:
	basic_I_Image(basic_I_Image &);
	void operator=(basic_I_Image &);
};

template<typename PixelType>
class basic_Image : public basic_I_Image<PixelType>
{
public:
	basic_Image(void) {}
	basic_Image(int width,int height) {Initialize(width,height);}
	~basic_Image() {Terminalize();}

	using basic_I_Image<PixelType>::Initialize;
	using basic_I_Image<PixelType>::Terminalize;
};


//windowsだとRGBをdefineしやがって使えない
struct Color
{
public:
	typedef unsigned char element_t;
	element_t b,g,r;
protected:
	element_t a;
	Color(element_t alpha,element_t red,element_t green,element_t blue)
		: b(blue),g(green),r(red),a(alpha) {}
public:

	Color(void) : b(0), g(0), r(0), a(255) {}
	Color(element_t red,element_t green,element_t blue)
		: b(blue),g(green),r(red),a(255) {}

	void Set(element_t red,element_t green,element_t blue)
		{b = blue;g = green;r = red;a = 255;}
};


struct ARGB : public Color
{
public:
	using Color::a;

	ARGB(void) : Color() {}
	ARGB(element_t red,element_t green,element_t blue)
		: Color(red,green,blue) {}
	ARGB(element_t alpha,element_t red,element_t green,element_t blue)
		: Color(alpha,red,green,blue) {}

	void Set(element_t alpha,element_t red,element_t green,element_t blue)
		{b = blue;g = green;r = red;a = alpha;}

	ARGB(Color color) : Color(color) {}
};


inline void static_assert_ARGB_size(void)
{
	int assert_element_size[sizeof(ARGB::element_t) == 1];
	int assert_struct_size[sizeof(ARGB) == 4];
	assert_element_size[0] = assert_struct_size[0] = 0;
}

typedef basic_I_Image<ARGB> I_Image;
typedef basic_Image<ARGB> Image;

typedef basic_I_Image<ARGB::element_t> I_GrayImage;
typedef basic_Image<ARGB::element_t> GrayImage;

}//namespace Picture
}//namespace Juna

#endif
