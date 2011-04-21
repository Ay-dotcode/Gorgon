#pragma warning(disable:4244)

#include "Flip.h"

using namespace gge::resource;
using namespace gge::graphics;

namespace gge { namespace effects {
	void FlipEffect::Draw(I2DGraphicsTarget *Layer,int X,int Y) {
		if(isFinished()) {
			if(Backside) {
				Back->Draw(Layer, X, Y);
			} else {
				Front->Draw(Layer, X, Y);
			}
		} else {
			float ang=((float)AnimatorBase::currentTime()/ETA)*PI;
			bool rev=false;
			if(ang>PI/2) {
				ang=PI-ang;
				rev=true;
			}

			Buffered2DGraphic *img=NULL;
			int cx,cy;
			if(Backside ^ rev) {
				img=Back;
			} else {
				img=Front;
			}
			if(img==NULL)
				return;

			cx=img->Texture.W;
			cy=img->Texture.H;

			FlipSide cside=Side;
			if(Backside ^ rev) {
				switch(Side) {
					case Top:
						cside=Bottom;
						break;
					case Left:
						cside=Right;
						break;
					case Bottom:
						cside=Top;
						break;
					case Right:
						cside=Left;
						break;
				}
			}

			Point pivot;
			if(Backside)
				pivot=PivotBack;
			else
				pivot=PivotFront;

			int x1=0,x2=cx,x3=cx,x4=0,y1=0,y2=0,y3=cy,y4=cy;
			float tmp;

			switch(cside) {
				case Top:
					tmp=(sin(ang)*cx)*Perspective;

					y1-=pivot.y;
					y3-=pivot.y;
	
					y1*=cosf(ang);
					y3*=cosf(ang);

					y1+=pivot.y;
					y3+=pivot.y;
					y2=y1;
					y4=y3;

					x3+=tmp;
					x4-=tmp;
					break;
				case Left:
					tmp=(sin(ang)*cy)*Perspective;

					x1-=pivot.x;
					x2-=pivot.x;

					x1*=cosf(ang);
					x2*=cosf(ang);

					x1+=pivot.x;
					x2+=pivot.x;
					x4=x1;
					x3=x2;

					y2-=tmp;
					y3+=tmp;
					break;
				case Bottom:
					tmp=(sin(ang)*cx)*Perspective;

					y1-=pivot.y;
					y3-=pivot.y;
	
					y1*=cosf(ang);
					y3*=cosf(ang);

					y1+=pivot.y;
					y3+=pivot.y;
					y2=y1;
					y4=y3;

					x1-=tmp;
					x2+=tmp;
					break;
				case Right:
					tmp=(sin(ang)*cy)*Perspective;

					x1-=pivot.x;
					x2-=pivot.x;

					x1*=cosf(ang);
					x2*=cosf(ang);

					x1+=pivot.x;
					x2+=pivot.x;
					x4=x1;
					x3=x2;

					y1-=tmp;
					y4+=tmp;
					break;
			}
			
			img->Draw(Layer,x1+X,y1+Y,x2+X,y2+Y,x3+X,y3+Y,x4+X,y4+Y);
		}
	}

	void FlipEffect::Flip(int ETA) {
		if(this->ETA!=0) {
			Backside=!Backside;
			progressed=this->ETA-progressed;
		}

		this->ETA=ETA;
		Play();
		Flipping=true;
	}

	void FlipEffect::Initialize() {
		Front=Back=NULL;
		Perspective=0.0125;
		Flipping=false;
		Backside=false;
		ETA=0;
		Side=Top;

		PivotFront.x=0;
		PivotFront.y=0;
		PivotBack.x=0;
		PivotBack.y=0;

		Pause();
	}

	void FlipEffect::CenterPivot() {
		if(Front) {
			PivotFront.x=Front->Texture.W /2;
			PivotFront.y=Front->Texture.H /2;
		}
		else if(Back) {
			PivotFront.x=Back->Texture.W /2;
			PivotFront.y=Back->Texture.H /2;
		}

		PivotBack=PivotFront;
	}

	void FlipEffect::Process(int Time) {
		if(Time>=ETA) {
			ETA=0;
			Pause();
			progressed=0;
			Flipping=false;
			Backside=!Backside;
		}
	}
} }
