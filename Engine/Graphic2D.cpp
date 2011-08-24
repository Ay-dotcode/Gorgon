
#include "Graphic2D.h"

namespace gge { namespace graphics {

	void RectangularGraphic2D::DrawRotated( ImageTarget2D& Target, utils::Point p, float angle, utils::Point origin/*=utils::Point(0,0)*/ ) const
	{
		angle*=Pi/180;

		int W=getwidth();
		int H=getheight();

		origin+=p;
		utils::Point p1=p;
		utils::Point p2=p;
		utils::Point p3=p;
		utils::Point p4=p;

		p2.x+=W;
		p3.x+=W;
		p3.y+=H;
		p4.y+=H;

		p1.Rotate(angle, origin);
		p2.Rotate(angle, origin);
		p3.Rotate(angle, origin);
		p4.Rotate(angle, origin);

		Draw(Target, p1,p2,p3,p4);
	}

} }