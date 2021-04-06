
#ifndef Juna_Picture_LayerImage_H
#define Juna_Picture_LayerImage_H

#include "Image.h"
#include "../2d_coordinate.h"
#include "../intrusive_list.hpp"

namespace Juna {
namespace Picture {

typedef D2::Point<long> Point;
typedef D2::Rectangle<long> Rect;


class I_Layer : public intrusive_list_node
{
public:
	unsigned int GetOrder(void) const {return Order;}
	const Rect &GetPosition(void) const {return Position;}

protected:
	I_Layer(void) {}
	I_Layer(int x,int y,unsigned width,unsigned height,unsigned order = 0)
		: Position(x,y,x+width,y+height),Order(order)
	{}
	~I_Layer() {};

//書き込み位置移動型更新
	void Move(int x,int y);

//更新領域の設定
	void SetUpdateRect(const Rect *iUpdateRect = 0);

//多少危険操作。普通の移動はMoveで。
	void SetPosition(const Rect &pos) {Position = pos;}
	void SetOrder(unsigned order) {Order = order;}

//背面透過判定
	virtual bool isTransparent(const Rect &iRect) const = 0;	//判定範囲

private:
//描画先領域
	Rect Position;
//描画レイヤー順位
	unsigned Order;

private:
friend class LayersSet;
//LayersSetから必要に応じて呼ばれる
	virtual void Update(I_Image &rLayered,				//描画先
						const Rect &iRect) const = 0;	//描画領域

//更新フラグ
	enum UPDATE_FLAG{UDF_NONE = 0,UDF_UPDATE = 1,UDF_MOVE = 2};
	UPDATE_FLAG UpdateFlag;
//更新領域
	Rect UpdateRect;
};


class LayersSet
{
public:
	LayersSet(void) : UpdateFlag(I_Layer::UDF_NONE) {}
	~LayersSet() {}

	void Clear(void) {Layers.clear();}

//レイヤーを登録
	bool Enter(I_Layer &setLayer,unsigned int iOrder);
	bool Enter(I_Layer &setLayer) {return Enter(setLayer,setLayer.Order);}
//レイヤー登録解除
	bool Exit(I_Layer &iLayer);

//登録レイヤーのz軸移動
//めんどくさいので簡単に。これでやらなくてもLayer単体でも出来るけど
	bool MoveOrder(I_Layer &iLayer,unsigned int iOrder)
		{return Exit(iLayer) && Enter(iLayer,iOrder);}

//登録されたレイヤーの更新状況に合わせて更新
//更新部分があればtrue、無ければfalse
	bool Update(I_Image &oTarget,Rect &oUpdateRect);

//とにかく全部描き直す
	void Blit(I_Image &oTarget);

private:
	typedef intrusive_list<I_Layer> list_t;
	list_t Layers;

	I_Layer::UPDATE_FLAG UpdateFlag;
	Rect UpdateRect;

//なんとなく仮引数を作業領域に使用するために値渡し
	void PrivateUpdate(I_Image &oTarget,Rect iRect,Rect &oUpdateRect);

private:
	LayersSet(LayersSet &);
	void operator=(LayersSet &);
};




class ColorLayer : public I_Layer
{
public:
	ColorLayer(void) {}
	ColorLayer(int x,int y,unsigned width,unsigned height,ARGB color,unsigned transrate = 256)
		: I_Layer(x,y,width,height),Color(color),TransparentRate(transrate) {}
	virtual ~ColorLayer() {}
	void Initialize(int x,int y,unsigned width,unsigned height,ARGB color,unsigned transrate = 256)
	{
		SetPosition(Rect(x,y,x+width,y+height));
		SetColor(color);
		SetTransparentRate(transrate);
	}

	void SetColor(ARGB color) {Color = color;}
	ARGB GetColor(void) {return Color;}

	void SetTransparentRate(int rate) {TransparentRate = rate;}
	unsigned GetTransparentRate(void) {return TransparentRate;}

	using I_Layer::SetUpdateRect;
	using I_Layer::Move;

	virtual bool isTransparent(const Rect &) const {return TransparentRate < 256;}
private:
	virtual void Update(I_Image &rLayered,const Rect &iRect) const;
	ARGB Color;
	unsigned TransparentRate;
};

class ImageLayer : public I_Layer
{
public:
	ImageLayer(void) : I_Layer(),Image(),TransparentRate(256) {}
	ImageLayer(int x,int y,unsigned width,unsigned height,unsigned transrate)
		: I_Layer(x,y,width,height),Image(width,height),TransparentRate(transrate) {}
	virtual ~ImageLayer() {}
	void Initialize(int x,int y,unsigned width,unsigned height)
	{
		Image.Initialize(width,height);
		SetPosition(Rect(x,y,x+width,y+height));
	}
	void Terminalize(void)
	{
		Image.Terminalize();
	}

	I_Image &GetImage(void) {return Image;}

	using I_Layer::SetUpdateRect;
	using I_Layer::Move;

	void SetTransparentRate(unsigned int rate) {TransparentRate = rate;}
	unsigned GetTransparentRate(void) {return TransparentRate;}

	virtual bool isTransparent(const Rect &) const {return TransparentRate < 256;}
private:
	virtual void Update(I_Image &rLayered,const Rect &iRect) const;

	Image Image;
	unsigned TransparentRate;
};


}//namespace Picture
}//namespace Juna

#endif
