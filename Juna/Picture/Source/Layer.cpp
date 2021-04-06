
#include "../Layer.h"
#include "../Blit.h"

namespace Juna {
namespace Picture {


void I_Layer::SetUpdateRect(const Rect *iUpdateRect)
{
	if (UpdateFlag == UDF_MOVE)
		return;

	if (iUpdateRect)
	{
		Point point(Position.x1,Position.y1);
		if (UpdateFlag == UDF_NONE)
		{
			UpdateRect = *iUpdateRect;
			UpdateRect += point;
		}
		else
		{
			Rect rect = *iUpdateRect + point;
			UpdateRect |= rect;
		}
	}
	else
		UpdateRect = Position;

	UpdateFlag = UDF_UPDATE;
}

void I_Layer::Move(int x,int y)
{
	if (UpdateFlag == UDF_MOVE)
//		UpdateRect |= Position;
		;
	else
		UpdateRect = Position;

	Position.Set(x,y,x + Position.width(),y + Position.height());
	UpdateFlag = UDF_MOVE;
}





namespace {
struct less {
	bool operator()(const I_Layer &lhs,const I_Layer &rhs) const
		{return lhs.GetOrder() < rhs.GetOrder();}
};
}

bool LayersSet::Enter(I_Layer &setLayer,unsigned int iOrder)
{
	if (setLayer.linked())
		return false;
	setLayer.Order = iOrder;
	list_t::iterator it = Layers.insert(setLayer,less());
	it->UpdateFlag = I_Layer::UDF_UPDATE;
	it->UpdateRect = it->Position;

	return true;
}

bool LayersSet::Exit(I_Layer &iLayer)
{
	for (list_t::iterator it = Layers.begin();it != Layers.end();++it)
		if (&*it == &iLayer)
		{
			Rect *rect;
			if (it->UpdateFlag == I_Layer::UDF_MOVE)
				rect = &it->UpdateRect;
			else
				rect = &it->Position;
			if (UpdateFlag == I_Layer::UDF_NONE)
			{
				UpdateRect = *rect;
				UpdateFlag = I_Layer::UDF_UPDATE;
			}
			else
				UpdateRect |= *rect;

			Layers.erase(it);
			return true;
		}
	return false;
}

bool LayersSet::Update(I_Image &oTarget,Rect &oUpdateRect)
{
	oUpdateRect.Set(0,0,0,0);
	if (UpdateFlag == I_Layer::UDF_UPDATE)
	{
		PrivateUpdate(oTarget,UpdateRect,oUpdateRect);
		UpdateFlag = I_Layer::UDF_NONE;
	}
	for (list_t::reverse_iterator it = Layers.rbegin();it != Layers.rend();++it)
	{
		switch (it->UpdateFlag)
		{
		case I_Layer::UDF_NONE:
			break;
		case I_Layer::UDF_UPDATE:
			PrivateUpdate(oTarget,it->UpdateRect,oUpdateRect);
			break;
		case I_Layer::UDF_MOVE:
			{
				Rect rect;
				if (rect.SetIntersectRect(it->UpdateRect,it->Position))
				{
					unsigned int and_area = rect.area();
					unsigned int or_area = (it->UpdateRect.area() + it->Position.area()) - and_area;
					rect.SetUnionRect(it->UpdateRect,it->Position);
					if (and_area > rect.area() - or_area)
					{
						PrivateUpdate(oTarget,rect,oUpdateRect);
						break;
					}
				}
				PrivateUpdate(oTarget,it->UpdateRect,oUpdateRect);
				PrivateUpdate(oTarget,it->Position,oUpdateRect);
			}
			break;
		}
		it->UpdateFlag = I_Layer::UDF_NONE;
	}
	return oUpdateRect.x2 != 0;
}
void LayersSet::Blit(I_Image &oTarget)
{
	for (list_t::iterator it = Layers.begin();it != Layers.end();++it)
	{
		Rect rect;
		if (rect.SetIntersectRect(Rect(0,0,oTarget.width(),oTarget.height()),it->Position))
			it->Update(oTarget,rect);
	}
}

void LayersSet::PrivateUpdate(I_Image &oTarget,Rect iRect,Rect &oUpdateRect)
{
	if (!iRect.SetIntersectRect(iRect,Rect(0,0,oTarget.width(),oTarget.height())))
		return;
	for (list_t::iterator it = Layers.begin();it != Layers.end();++it)
	{
		Rect rect;
		if (rect.SetIntersectRect(iRect,it->Position))
			it->Update(oTarget,rect);
	}
	if (oUpdateRect.x2 == 0)	//ほぼマジックナンバーで判定するのはアレだけど
		oUpdateRect = iRect;
	else
		oUpdateRect |= iRect;
}





void ColorLayer::Update(I_Image &rLayered,const Rect &iRect) const
{
	TranslucentFillColor(rLayered,Color,TransparentRate,&iRect);
//	FillColor(rLayered,Color,&iRect);
}


void ImageLayer::Update(I_Image &rLayered,const Rect &iRect) const
{
	Rect rect;
	rect.x2 = (rect.x1 = iRect.x1 - GetPosition().x1) + iRect.width();
	rect.y2 = (rect.y1 = iRect.y1 - GetPosition().y1) + iRect.height();
	TranslucentAlphaBlit(rLayered,Image,TransparentRate,iRect.x1,iRect.y1,&rect);
}


}//namespace Picture
}//namespace Juna


