
#include "Graphic2D.h"
#include "../Utils/BasicMath.h"

namespace gge { namespace graphics {

	void RectangularGraphic2D::DrawRotated( ImageTarget2D& Target, utils::Point p, float angle, utils::Point origin/*=utils::Point(0,0)*/ ) const
	{

		using namespace gge::utils;

		//angle*=Pi/180;

		int W=getwidth();
		int H=getheight();

		origin+=p;
		utils::Point2D p1=p;
		utils::Point2D p2=p;
		utils::Point2D p3=p;
		utils::Point2D p4=p;

		p2.x+=W;
		p3.x+=W;
		p3.y+=H;
		p4.y+=H;

		p1.Rotate(angle, origin);
		p2.Rotate(angle, origin);
		p3.Rotate(angle, origin);
		p4.Rotate(angle, origin);

		Draw(Target, (int)Round(p1.x),(int)Round(p1.y),(int)Round(p2.x),(int)Round(p2.y),(int)Round(p3.x),(int)Round(p3.y),(int)Round(p4.x),(int)Round(p4.y));
	}

} }