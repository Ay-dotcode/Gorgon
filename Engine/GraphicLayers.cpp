#include "GraphicLayers.h"
#include "OpenGL.h"
#include "../ShaderCode/ShaderCode.h"
#include "GGEMain.h"

#pragma warning(disable:4244)
#pragma GCC diagnostic ignored "-Wuninitialized"

using namespace gge::utils;
using namespace gge::input;
using namespace gge::graphics::system;

namespace gge { namespace graphics {
	RGBfloat CurrentLayerColor;
	Point translate;
	Bounds scissors;

	// ShaderType has to inherit from ShaderBase
	template<class ShaderType>
	void RenderSurface(const BasicSurface& surface)
	{
		ShaderType::Use();

		glm::mat4 vertex_coords;
		for (int i = 0; i < 4; ++i) vertex_coords[i] = glm::vec4(surface.VertexCoords[i].x, surface.VertexCoords[i].y, surface.VertexCoords[i].z, 1.0f);
		vertex_coords = ProjectionMatrixStack.Top() * ModelViewMatrixStack.Top() * vertex_coords;
		for (int i = 0; i < 4; ++i) vertex_coords[i] /= vertex_coords[i].w;
		glm::mat4x3 vertex_coords_3d = glm::mat4x3(glm::vec3(vertex_coords[0]), glm::vec3(vertex_coords[1]), glm::vec3(vertex_coords[2]), glm::vec3(vertex_coords[3]));

		glm::mat4x2 tex_coords;
		for (int i = 0; i < 4; ++i) tex_coords[i] = glm::vec2(surface.TextureCoords[i].s, surface.TextureCoords[i].t);

		ShaderType::Get().UpdateUniform("vertex_coords", vertex_coords_3d);
		ShaderType::Get().UpdateUniform("tex_coords", tex_coords);

		glActiveTexture(GL_TEXTURE0 + graphics::TextureUnit::Diffuse);
		glBindTexture(GL_TEXTURE_2D, surface.getTexture()->ID);
		UnitQuad::Draw();	
	}

	void Basic2DLayer::Draw(const GLTexture *Image, float X1, float Y1, float X2, float Y2, float X3, float Y3, float X4, float Y4) {
		BasicSurface *surface=Surfaces.Add();

		surface->setTexture(Image);
		surface->VertexCoords[0].x=X1;
		surface->VertexCoords[0].y=Y1;

		surface->VertexCoords[1].x=X2;
		surface->VertexCoords[1].y=Y2;

		surface->VertexCoords[2].x=X3;
		surface->VertexCoords[2].y=Y3;

		surface->VertexCoords[3].x=X4;
		surface->VertexCoords[3].y=Y4;
		surface->Mode=DrawMode;		
	}

	void Basic2DLayer::Draw(const GLTexture *Image, float X1, float Y1, float X2, float Y2, float X3, float Y3, float X4, float Y4,  float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) {
		BasicSurface *surface=Surfaces.Add();

		surface->setTexture(Image);
		surface->VertexCoords[0].x=X1;
		surface->VertexCoords[0].y=Y1;

		surface->VertexCoords[1].x=X2;
		surface->VertexCoords[1].y=Y2;

		surface->VertexCoords[2].x=X3;
		surface->VertexCoords[2].y=Y3;

		surface->VertexCoords[3].x=X4;
		surface->VertexCoords[3].y=Y4;

		surface->CreateTextureCoords();

		float w=Image->W, h=Image->H;
		surface->TextureCoords[0].s=S1/w;
		surface->TextureCoords[0].t=U1/h;

		surface->TextureCoords[1].s=S2/w;
		surface->TextureCoords[1].t=U2/h;

		surface->TextureCoords[2].s=S3/w;
		surface->TextureCoords[2].t=U3/h;

		surface->TextureCoords[3].s=S4/w;
		surface->TextureCoords[3].t=U4/h;
		surface->Mode=DrawMode;
	}

	void Basic2DLayer::DrawTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		BasicSurface *surface=Surfaces.Add();

		surface->setTexture(Image);
		surface->CreateTextureCoords();

		surface->TextureCoords[0].s=0;
		surface->TextureCoords[0].t=0;
		surface->VertexCoords[0].x=X;
		surface->VertexCoords[0].y=Y;

		surface->TextureCoords[1].s=(float)W/Image->W;
		surface->TextureCoords[1].t=0;
		surface->VertexCoords[1].x=X+W;
		surface->VertexCoords[1].y=Y;

		surface->TextureCoords[2].s=(float)W/Image->W;
		surface->TextureCoords[2].t=(float)H/Image->H;
		surface->VertexCoords[2].x=X+W;
		surface->VertexCoords[2].y=Y+H;

		surface->TextureCoords[3].s=0;
		surface->TextureCoords[3].t=(float)H/Image->H;
		surface->VertexCoords[3].x=X;
		surface->VertexCoords[3].y=Y+H;
		surface->Mode=DrawMode;		
	}

	void Basic2DLayer::DrawHTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		BasicSurface *surface=Surfaces.Add();

		surface->setTexture(Image);
		surface->CreateTextureCoords();

		surface->TextureCoords[0].s=0;
		surface->TextureCoords[0].t=0;
		surface->VertexCoords[0].x=X;
		surface->VertexCoords[0].y=Y;

		surface->TextureCoords[1].s=(float)W/Image->W;
		surface->TextureCoords[1].t=0;
		surface->VertexCoords[1].x=X+W;
		surface->VertexCoords[1].y=Y;

		surface->TextureCoords[2].s=(float)W/Image->W;
		surface->TextureCoords[2].t=Image->ImageCoord[2].t;
		surface->VertexCoords[2].x=X+W;
		surface->VertexCoords[2].y=Y+H;

		surface->TextureCoords[3].s=0;
		surface->TextureCoords[3].t=Image->ImageCoord[2].t;
		surface->VertexCoords[3].x=X;
		surface->VertexCoords[3].y=Y+H;
		surface->Mode=DrawMode;		
	}

	void Basic2DLayer::DrawVTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		BasicSurface *surface=Surfaces.Add();

		surface->setTexture(Image);
		surface->CreateTextureCoords();

		surface->TextureCoords[0].s=0;
		surface->TextureCoords[0].t=0;
		surface->VertexCoords[0].x=X;
		surface->VertexCoords[0].y=Y;

		surface->TextureCoords[1].s=Image->ImageCoord[1].s;
		surface->TextureCoords[1].t=0;
		surface->VertexCoords[1].x=X+W;
		surface->VertexCoords[1].y=Y;

		surface->TextureCoords[2].s=Image->ImageCoord[2].s;
		surface->TextureCoords[2].t=(float)H/Image->H;
		surface->VertexCoords[2].x=X+W;
		surface->VertexCoords[2].y=Y+H;

		surface->TextureCoords[3].s=0;
		surface->TextureCoords[3].t=(float)H/Image->H;
		surface->VertexCoords[3].x=X;
		surface->VertexCoords[3].y=Y+H;
		surface->Mode=DrawMode;		
	}
	
	void Basic2DLayer::Render() {
		Rectangle psc;
		BasicSurface::DrawMode currentdrawmode=BasicSurface::Normal;

		if(!IsVisible) return;
		glPushAttrib(GL_SCISSOR_BIT);

		/*glMatrixMode(GL_MODELVIEW);
		glPushMatrix();*/
		glutil::PushStack _(ModelViewMatrixStack);
		//glTranslatef(BoundingBox.Left, BoundingBox.Top, 0);
		ModelViewMatrixStack.Translate(BoundingBox.Left, BoundingBox.Top, 0.0f);		
		translate+=BoundingBox.TopLeft();

		if(ClippingEnabled) {
			psc=scissors;
			glEnable(GL_SCISSOR_TEST);
			if(translate.x>scissors.Left)
				scissors.Left=translate.x;
			if(translate.y>scissors.Top)
				scissors.Top=translate.y;
			if(translate.y+BoundingBox.Height()<scissors.Bottom)
				scissors.Bottom=(translate.y+BoundingBox.Height());
			if(translate.x+BoundingBox.Width()<scissors.Right)
				scissors.Right=(translate.x+BoundingBox.Width());

			if(scissors.Right<=scissors.Left || scissors.Bottom<=scissors.Top) {
				goto end;
			}

			glScissor(scissors.Left, (ScreenSize.Height-scissors.Top)-scissors.Height(), scissors.Width(), scissors.Height());
		}

		int i;

		for(i=0;i<Surfaces.GetCount();i++) {
			BasicSurface *surface=Surfaces[i];
			if(surface->Mode!=currentdrawmode) {
				currentdrawmode=surface->Mode;

				if(currentdrawmode==BasicSurface::Normal) {
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
					glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
					if(system::OffscreenRendering) {
						system::SetRenderTarget(0);
						system::OffscreenRendering=false;
						DumpOffscreen();
					}
				}
				else if(currentdrawmode==BasicSurface::Offscreen) {
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
					glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
					if(!system::OffscreenRendering) {
						system::SetRenderTarget(FrameBuffer);
						glClearColor(0,0,0,0);
						glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
						glClear(GL_COLOR_BUFFER_BIT);
						glClearColor(0,0,0,1);

						system::OffscreenRendering=true;
					}
				}
				else if(currentdrawmode==BasicSurface::OffscreenAlphaOnly) {
					if(!system::OffscreenRendering) {
						system::SetRenderTarget(FrameBuffer);
						glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
						glClearColor(0,0,0,0);
						glClear(GL_COLOR_BUFFER_BIT);
						glClearColor(0,0,0,1);

						system::OffscreenRendering=true;
					}
					glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
					glBlendFunc(GL_DST_COLOR, GL_ZERO);
				}

			}
			RenderSurface<gge::shadercode::SimpleShader>(*surface);
			/*glBindTexture(GL_TEXTURE_2D, surface->getTexture()->ID);
			glBegin(GL_QUADS);
			glTexCoord2fv(surface->TextureCoords[0].vect);
			glVertex3fv(surface->VertexCoords[0].vect);
			glTexCoord2fv(surface->TextureCoords[1].vect);
			glVertex3fv(surface->VertexCoords[1].vect);
			glTexCoord2fv(surface->TextureCoords[2].vect);
			glVertex3fv(surface->VertexCoords[2].vect);
			glTexCoord2fv(surface->TextureCoords[3].vect);
			glVertex3fv(surface->VertexCoords[3].vect);
			glEnd();*/
		}
		
		if(system::OffscreenRendering) {
			system::OffscreenRendering=false;
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			system::SetRenderTarget(0);
			DumpOffscreen();
		}

		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.Last(); i.IsValid(); i.Previous()) {
			i->Render();
		}

end:
		//glPopMatrix();
		translate-=BoundingBox.TopLeft();

		if(ClippingEnabled) {
			scissors=psc;
			glScissor(scissors.Left, (ScreenSize.Height-scissors.Top)-scissors.Height(), scissors.Width(), scissors.Height());
		}

		//make sure everything is back in its place
		glPopAttrib();
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	}

	Basic2DLayer::~Basic2DLayer() {

	}


	void Colorizable2DLayer::Draw(const GLTexture *Image, float X1, float Y1, float X2, float Y2, float X3, float Y3, float X4, float Y4) {
		ColorizableSurface *surface=Surfaces.Add();

		surface->setTexture(Image);
		surface->VertexCoords[0].x=X1;
		surface->VertexCoords[0].y=Y1;

		surface->VertexCoords[1].x=X2;
		surface->VertexCoords[1].y=Y2;

		surface->VertexCoords[2].x=X3;
		surface->VertexCoords[2].y=Y3;

		surface->VertexCoords[3].x=X4;
		surface->VertexCoords[3].y=Y4;

		surface->Color=ToRGBfloat(CurrentColor);
		surface->Mode=DrawMode;
	}

	void Colorizable2DLayer::Draw(const GLTexture *Image, float X1, float Y1, float X2, float Y2, float X3, float Y3, float X4, float Y4,  float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) {
		ColorizableSurface *surface=Surfaces.Add();

		surface->setTexture(Image);
		surface->VertexCoords[0].x=X1;
		surface->VertexCoords[0].y=Y1;

		surface->VertexCoords[1].x=X2;
		surface->VertexCoords[1].y=Y2;

		surface->VertexCoords[2].x=X3;
		surface->VertexCoords[2].y=Y3;

		surface->VertexCoords[3].x=X4;
		surface->VertexCoords[3].y=Y4;

		surface->CreateTextureCoords();

		float w=Image->W, h=Image->H;
		surface->TextureCoords[0].s=S1/w;
		surface->TextureCoords[0].t=U1/h;

		surface->TextureCoords[1].s=S2/w;
		surface->TextureCoords[1].t=U2/h;

		surface->TextureCoords[2].s=S3/w;
		surface->TextureCoords[2].t=U3/h;

		surface->TextureCoords[3].s=S4/w;
		surface->TextureCoords[3].t=U4/h;

		surface->Color=ToRGBfloat(CurrentColor);
		surface->Mode=DrawMode;
	}

	void Colorizable2DLayer::DrawTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		ColorizableSurface *surface=Surfaces.Add();
		surface->Color=ToRGBfloat(CurrentColor);

		surface->setTexture(Image);
		surface->CreateTextureCoords();

		surface->TextureCoords[0].s=0;
		surface->TextureCoords[0].t=0;
		surface->VertexCoords[0].x=X;
		surface->VertexCoords[0].y=Y;

		surface->TextureCoords[1].s=(float)W/Image->W;
		surface->TextureCoords[1].t=0;
		surface->VertexCoords[1].x=X+W;
		surface->VertexCoords[1].y=Y;

		surface->TextureCoords[2].s=(float)W/Image->W;
		surface->TextureCoords[2].t=(float)H/Image->H;
		surface->VertexCoords[2].x=X+W;
		surface->VertexCoords[2].y=Y+H;

		surface->TextureCoords[3].s=0;
		surface->TextureCoords[3].t=(float)H/Image->H;
		surface->VertexCoords[3].x=X;
		surface->VertexCoords[3].y=Y+H;
		surface->Mode=DrawMode;
	}

	void Colorizable2DLayer::DrawHTiled(const GLTexture *Image,int X,int Y,int W,int H) {
			ColorizableSurface *surface=Surfaces.Add();
		surface->Color=ToRGBfloat(CurrentColor);

		surface->setTexture(Image);
		surface->CreateTextureCoords();

		surface->TextureCoords[0].s=0;
		surface->TextureCoords[0].t=0;
		surface->VertexCoords[0].x=X;
		surface->VertexCoords[0].y=Y;

		surface->TextureCoords[1].s=(float)W/Image->W;
		surface->TextureCoords[1].t=0;
		surface->VertexCoords[1].x=X+W;
		surface->VertexCoords[1].y=Y;

		surface->TextureCoords[2].s=(float)W/Image->W;
		surface->TextureCoords[2].t=1;
		surface->VertexCoords[2].x=X+W;
		surface->VertexCoords[2].y=Y+H;

		surface->TextureCoords[3].s=0;
		surface->TextureCoords[3].t=1;
		surface->VertexCoords[3].x=X;
		surface->VertexCoords[3].y=Y+H;
		surface->Mode=DrawMode;
	}

	void Colorizable2DLayer::DrawVTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		
		ColorizableSurface *surface=Surfaces.Add();
		surface->Color=ToRGBfloat(CurrentColor);

		surface->setTexture(Image);
		surface->CreateTextureCoords();

		surface->TextureCoords[0].s=0;
		surface->TextureCoords[0].t=0;
		surface->VertexCoords[0].x=X;
		surface->VertexCoords[0].y=Y;

		surface->TextureCoords[1].s=1;
		surface->TextureCoords[1].t=0;
		surface->VertexCoords[1].x=X+W;
		surface->VertexCoords[1].y=Y;

		surface->TextureCoords[2].s=1;
		surface->TextureCoords[2].t=(float)H/Image->H;
		surface->VertexCoords[2].x=X+W;
		surface->VertexCoords[2].y=Y+H;

		surface->TextureCoords[3].s=0;
		surface->TextureCoords[3].t=(float)H/Image->H;
		surface->VertexCoords[3].x=X;
		surface->VertexCoords[3].y=Y+H;
		surface->Mode=DrawMode;
	}

	void Colorizable2DLayer::Render() {
		Rectangle psc;
		BasicSurface::DrawMode currentdrawmode=BasicSurface::Normal;

		if(!IsVisible) return;
		if(Ambient.a==0) return;
		glPushAttrib(GL_SCISSOR_BIT);

		/*glMatrixMode(GL_MODELVIEW);
		glPushMatrix();*/
		glutil::PushStack _(ModelViewMatrixStack);
		//glTranslatef(BoundingBox.Left, BoundingBox.Top, 0);
		ModelViewMatrixStack.Translate(BoundingBox.Left, BoundingBox.Top, 0.0f);		
		translate+=BoundingBox.TopLeft();

		RGBfloat prevcolor=CurrentLayerColor;
		CurrentLayerColor.a*=(float)Ambient.a/255;
		CurrentLayerColor.r*=(float)Ambient.r/255;
		CurrentLayerColor.g*=(float)Ambient.g/255;
		CurrentLayerColor.b*=(float)Ambient.b/255;

		gge::shadercode::SimpleTintShader::Use();

		if(ClippingEnabled) {
			psc=scissors;

			glEnable(GL_SCISSOR_TEST);
			if(translate.x>scissors.Left)
				scissors.Left=translate.x;
			if(translate.y>scissors.Top)
				scissors.Top=translate.y;
			if(translate.y+BoundingBox.Height()<scissors.Bottom)
				scissors.Bottom=(translate.y+BoundingBox.Height());
			if(translate.x+BoundingBox.Width()<scissors.Right)
				scissors.Right=(translate.x+BoundingBox.Width());

			if(scissors.Right<=scissors.Left || scissors.Bottom<=scissors.Top) {
				goto end;
			}

			glScissor(scissors.Left, (ScreenSize.Height-scissors.Top)-scissors.Height(), scissors.Width(), scissors.Height());
		}

		if(CurrentLayerColor.a==0)
			goto end;

		int i;

		for(i=0;i<Surfaces.GetCount();i++) {
			ColorizableSurface *surface=Surfaces[i];

			if(surface->Mode!=currentdrawmode) {
				currentdrawmode=surface->Mode;

				if(currentdrawmode==BasicSurface::Normal && system::OffscreenRendering) {
					system::OffscreenRendering=false;
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
					glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
					system::SetRenderTarget(0);
					DumpOffscreen();
				}
				else if(currentdrawmode==BasicSurface::Offscreen) {
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
					glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
					if(!system::OffscreenRendering) {
						system::SetRenderTarget(FrameBuffer);
						glClearColor(0,0,0,0);
						glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
						glClear(GL_COLOR_BUFFER_BIT);
						glClearColor(0,0,0,1);

						system::OffscreenRendering=true;
					}
				}
				else if(currentdrawmode==BasicSurface::OffscreenAlphaOnly) {
					if(!system::OffscreenRendering) {
						system::SetRenderTarget(FrameBuffer);
						glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
						glClearColor(0,0,0,0);
						glClear(GL_COLOR_BUFFER_BIT);
						glClearColor(0,0,0,1);

						system::OffscreenRendering=true;
					}
					glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
					glBlendFunc(GL_DST_COLOR, GL_ZERO);
				}

			}

			glm::vec4 tint;
			tint.r = surface->Color.r * CurrentLayerColor.r;
			tint.g = surface->Color.g * CurrentLayerColor.g;
			tint.b = surface->Color.b * CurrentLayerColor.b;
			tint.a = surface->Color.a * CurrentLayerColor.a;
			gge::shadercode::SimpleTintShader::Get().UpdateUniform("tint", tint);
			//glColor4f(surface->Color.r * CurrentLayerColor.r,surface->Color.g * CurrentLayerColor.g,surface->Color.b * CurrentLayerColor.b,surface->Color.a * CurrentLayerColor.a);
			// What's the purpose of that? Does the color get added or multiplied by the texture color?

			RenderSurface<gge::shadercode::SimpleTintShader>(*surface);
			/*
			glBindTexture(GL_TEXTURE_2D, surface->getTexture()->ID);
			glBegin(GL_QUADS);
			glTexCoord2fv(surface->TextureCoords[0].vect);
			glVertex3fv(surface->VertexCoords[0].vect);
			glTexCoord2fv(surface->TextureCoords[1].vect);
			glVertex3fv(surface->VertexCoords[1].vect);
			glTexCoord2fv(surface->TextureCoords[2].vect);
			glVertex3fv(surface->VertexCoords[2].vect);
			glTexCoord2fv(surface->TextureCoords[3].vect);
			glVertex3fv(surface->VertexCoords[3].vect);
			glEnd();*/
		}

		if(system::OffscreenRendering) {
			system::OffscreenRendering=false;
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			system::SetRenderTarget(0);
			DumpOffscreen();
		}

		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);		
		gge::shadercode::SimpleTintShader::Use();
		gge::shadercode::SimpleTintShader::Get().UpdateUniform("tint", glm::vec4(CurrentLayerColor.r, CurrentLayerColor.g, CurrentLayerColor.b, CurrentLayerColor.a));
		//glColor4fv(CurrentLayerColor.vect);
		// What are we doing here with this color4fv?
		for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.Last(); i.IsValid(); i.Previous()) {
			i->Render();
		}

end:
		CurrentLayerColor=prevcolor;
		//glColor4fv(prevcolor.vect);
		// Same question.
		gge::shadercode::SimpleTintShader::Use();
		gge::shadercode::SimpleTintShader::Get().UpdateUniform("tint", glm::vec4(prevcolor.r, prevcolor.g, prevcolor.b, prevcolor.a));
		//glPopMatrix();
		translate-=BoundingBox.TopLeft();

		if(ClippingEnabled) {
			scissors=psc;
			glScissor(scissors.Left, (ScreenSize.Height-scissors.Top)-scissors.Height(), scissors.Width(), scissors.Height());
		}

		glPopAttrib();
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	}

	
} }
