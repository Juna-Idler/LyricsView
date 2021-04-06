
#ifndef Juna_2D_Coordinate_H
#define Juna_2D_Coordinate_H

namespace Juna {
namespace D2 {

//	二次元ベクトルクラステンプレート
template<typename T>
struct Vector
{
//	要素
	T x,y;
//	コンストラクタ
	Vector(void) {}
	Vector(T x,T y) : x(x),y(y) {}
//	Vector(const Vector &src) : x(src.x),y(src.y) {}

//	代入
	Vector &Set(T x,T y) {this->x = x;this->y = y;return *this;}
//	Vector &operator=(const Vector &rhs) {x = rhs.x;y = rhs.y;return *this;}
//	加算
	Vector &operator+=(const Vector &rhs) {x += rhs.x;y += rhs.y;return *this;}
	Vector operator+(const Vector &rhs) const {return Vector2D(x + rhs.x,y + rhs.y);}
//	減算
	Vector &operator-=(const Vector &rhs) {x -= rhs.x;y -= rhs.y;return *this;}
	Vector operator-(const Vector &rhs) const {return Vector2D(x - rhs.x,y - rhs.y);}
//	スカラー倍
	Vector &operator*=(T rhs) {x *= rhs;y *= rhs;return *this;}
	Vector operator*(T rhs) const {return Vector(x * rhs,y * rhs);}
	Vector &operator/=(T rhs) {x /= rhs;y /= rhs;return *this;}
	Vector operator/(T rhs) const {return Vector(x / rhs,y / rhs);}
//	逆ベクトル
	Vector operator-(void) const {return Vector(-x,-y);}
	Vector operator+(void) const {return Vector(x,y);}

	bool operator ==(const Vector &rhs) const {return x == rhs.x && y == rhs.y;}
	bool operator !=(const Vector &rhs) const {return x != rhs.x || y != rhs.y;}

//	内積
	T DotProduct(const Vector &rhs) const {return x * rhs.x + y * rhs.y;}
	T InnerProduct(const Vector &rhs) const {return x * rhs.x + y * rhs.y;}
	T ScalarProduct(const Vector &rhs) const {return x * rhs.x + y * rhs.y;}
//	外積
	T CrossProduct(const Vector &rhs) const {return x * rhs.y - y * rhs.x;}
	T OuterProduct(const Vector &rhs) const {return x * rhs.y - y * rhs.x;}
	T VectorProduct(const Vector &rhs) const {return x * rhs.y - y * rhs.x;}
//	大きさの2乗
	T Power2(void) const {return x * x + y * y;}
};


//	ほぼ単なるvectorのstrong typedef
template<typename T>
struct Point : public Vector<T>
{
	Point(void) {}
	Point(T x,T y) : Vector<T>(x,y) {}
	Point(const Vector<T> &src) : Vector<T>(src) {}
	Point &Set(T x,T y) {Vector<T>::Set(x,y);return *this;}
	Point &operator=(const Vector<T> &rhs) {*this = rhs;return *this;}

	Point &operator+=(const Vector<T> &rhs) {Vector<T>::operator+=(rhs);return *this;}
	Point operator+(const Vector<T> &rhs) const {return Point(this->x + rhs.x,this->y + rhs.y);}
	Point &operator-=(const Vector<T> &rhs) {Vector<T>::operator-=(rhs);return *this;}
	Point operator-(const Vector<T> &rhs) const {return Point(this->x - rhs.x,this->y - rhs.y);}

	Point &operator*=(T rhs) {Vector<T>::operator*=(rhs);return *this;}
	Point operator*(T rhs) const {return Point(this->x * rhs,this->y * rhs);}
	Point &operator/=(T rhs) {Vector<T>::operator/=(rhs);return *this;}
	Point operator/(T rhs) const {return Point(this->x / rhs,this->y / rhs);}

	Point operator-(void) const {return Point(-this->x,-this->y);}
	Point operator+(void) const {return Point(this->x,this->y);}

	bool operator==(const Point &rhs) const {return this->x == rhs.x && this->y == rhs.y;}
	bool operator!=(const Point &rhs) const {return this->x != rhs.x || this->y != rhs.y;}
};

template<typename T>
struct Size
{
	T width;
	T height;

	Size(void) {}
	Size(T iWidth,T iHeight) {Set(iWidth,iHeight);}
	Size &Set(T iWidth,T iHeight) {width = iWidth;height = iHeight;return *this;}

	Size &operator+=(const Size &rhs) {return Set(width + rhs.width,height + rhs.height);}
	Size operator+(const Size &rhs) const {return Size(width + rhs.width,height + rhs.height);}
	Size &operator-=(const Size &rhs) {return Set(width - rhs.width,height - rhs.height);}
	Size operator-(const Size &rhs) const {return Size(width - rhs.width,height - rhs.height);}

	T area(void) const {return width * height;}
};

template<typename T>
struct Rectangle
{
	T x1;
	T y1;
	T x2;
	T y2;

	Rectangle(void) {Set(0, 0, 0, 0); }
	Rectangle(T x1,T y1,T x2,T y2) {Set(x1,y1,x2,y2);}
	Rectangle(const Point<T> &point1,const Point<T> &point2) {Set(point1,point2);}
	Rectangle(T x,T y,const Size<T> &size) {Set(x,y,size);}
	Rectangle(const Point<T> &point,const Size<T> &size) {Set(point,size);}

	Rectangle &Set(T X1,T Y1,T X2,T Y2) {x1 = X1;y1 = Y1;x2 = X2;y2 = Y2;return *this;}
	Rectangle &Set(const Point<T> &point1,const Point<T> &point2) {return Set(point1.x,point1.y,point2.x,point2.y);}
	Rectangle &Set(T x,T y,const Size<T> &size) {return SetBySize(x,y,size.width,size.height);}
	Rectangle &Set(const Point<T> &point,const Size<T> &size) {return SetBySize(point.x,point.y,size.width,size.height);}
	Rectangle &SetBySize(T x,T y,T width,T height) {x2 = (x1 = x) + width;y2 = (y1 = y) + height;return *this;}

	T width(void) const {return x2 - x1;}
	T height(void) const {return y2 - y1;}
	T centerX(void) const {return (x1 + x2) / 2;}
	T centerY(void) const {return (y1 + y2) / 2;}
	T area(void) const {return width() * height();}

	Rectangle &operator+=(const Point<T> &rhs)
		{return Set(x1 + rhs.x,y1 + rhs.y,x2 + rhs.x,y2 + rhs.y);}
	Rectangle operator+(const Point<T> &rhs) const
		{return Rectangle(x1 + rhs.x,y1 + rhs.y,x2 + rhs.x,y2 + rhs.y);}

	Rectangle &operator-=(const Point<T> &rhs)
		{return Set(x1 - rhs.x,y1 - rhs.y,x2 - rhs.x,y2 - rhs.y);}
	Rectangle operator-(const Point<T> &rhs) const
		{return Rectangle(x1 - rhs.x,y1 - rhs.y,x2 - rhs.x,y2 - rhs.y);}

	bool operator&(const Rectangle &rhs) const {return isIntersect(rhs);}
	Rectangle operator|(const Rectangle &rhs) const {return CreateUnionRect(*this,rhs);}
	Rectangle &operator|=(const Rectangle &rhs) {return SetUnionRect(*this,rhs);}

	bool SetIntersectRect(const Rectangle &iRect1,const Rectangle &iRect2)
	{
		if ((iRect1.x1 < iRect2.x2) & (iRect1.x2 > iRect2.x1) &
			(iRect1.y1 < iRect2.y2) & (iRect1.y2 > iRect2.y1))
		{
			Set(iRect1.x1 > iRect2.x1 ? iRect1.x1 : iRect2.x1,
				iRect1.y1 > iRect2.y1 ? iRect1.y1 : iRect2.y1,
				iRect1.x2 < iRect2.x2 ? iRect1.x2 : iRect2.x2,
				iRect1.y2 < iRect2.y2 ? iRect1.y2 : iRect2.y2);
			return true;
		}
		return false;
	}
	Rectangle &SetUnionRect(const Rectangle &iRect1,const Rectangle &iRect2)
	{
		return Set(iRect1.x1 < iRect2.x1 ? iRect1.x1 : iRect2.x1,
				   iRect1.y1 < iRect2.y1 ? iRect1.y1 : iRect2.y1,
				   iRect1.x2 > iRect2.x2 ? iRect1.x2 : iRect2.x2,
				   iRect1.y2 > iRect2.y2 ? iRect1.y2 : iRect2.y2);
	}

	bool isIntersect(const Rectangle &iRect) const 
	{
		return ((x1 < iRect.x2) & (x2 > iRect.x1) &
				(y1 < iRect.y2) & (y2 > iRect.y1));
	}

	bool isIncluded(const Rectangle &iLargeRect) const
	{
		return ((x1 >= iLargeRect.x1) & (x2 <= iLargeRect.x2) &
				(y1 >= iLargeRect.y1) & (y2 <= iLargeRect.y2));
	}

	static Rectangle CreateUnionRect(const Rectangle &iRect1,const Rectangle &iRect2)
	{
		return Rectangle(iRect1.x1 < iRect2.x1 ? iRect1.x1 : iRect2.x1,
						 iRect1.y1 < iRect2.y1 ? iRect1.y1 : iRect2.y1,
						 iRect1.x2 > iRect2.x2 ? iRect1.x2 : iRect2.x2,
						 iRect1.y2 > iRect2.y2 ? iRect1.y2 : iRect2.y2);
	}
};


template<typename T>
class Matrix
{
	T m[2][2];
	Matrix(void) {}
	Matrix(const T arr[4]) {Set(arr[0],arr[1],arr[2],arr[3]);}
	Matrix(T m11,T m12,T m21,T m22) {Set(m11,m12,m21,m22);}

	Matrix &Set(T m11,T m12,T m21,T m22)
		{m[0][0] = m11;m[0][1] = m12;m[1][0] = m21;m[1][1] = m22;return this;}

// メンバアクセス
	T *operator[](int row) {return m[row];}
	const T *operator[](int row) const {return m[row];}

// 代入演算子
	Matrix &operator*=(const Matrix &rhs);
	Matrix &operator+=(const Matrix &rhs);
	Matrix &operator-=(const Matrix &rhs);
	Matrix &operator*=(T rhs);
	Matrix &operator/=(T rhs);

// 単項演算子
	Matrix operator+(void) const;
	Matrix operator-(void) const;

// 2 項演算子
	Matrix operator*(const Matrix &rhs) const;
	Matrix operator+(const Matrix &rhs) const;
	Matrix operator-(const Matrix &rhs) const;
	Matrix operator*(T rhs) const;
	Matrix operator/(T rhs) const;

	friend Matrix operator*(T lhs,const Matrix &rhs);

	bool operator==(const Matrix &rhs) const;
	bool operator!=(const Matrix &rhs) const;

};

}//	namespace D2
}//	namespace Juna

#endif
