#include "GraphicLayers.h"
#include "OpenGL.h"
#include "InternalShaders.h"
#include "GGEMain.h"

#pragma warning(disable:4244)
#pragma GCC diagnostic ignored "-Wuninitialized"

using namespace gge::utils;
using namespace gge::input;
using namespace gge::graphics::system;

namespace gge { namespace graphics {
	RGBfloat CurrentLayerColor;
	Point translate;
	Bounds scissors(0,0, 100000,100000);

	// ShaderType has to inherit from ShaderBase
	template<class ShaderType>
	void RenderSurface(const BasicSurface& surface)
	{
		glm::mat4 vertex_coords;
		for (int i = 0; i < 4; ++i) vertex_coords[i] = glm::vec4(surface.VertexCoords[i].x, surface.VertexCoords[i].y, surface.VertexCoords[i].z, 1.0f);
		vertex_coords = ProjectionMatrixStack.Top() * ModelViewMatrixStack.Top() * vertex_coords;
		for (int i = 0; i < 4; ++i) vertex_coords[i] /= vertex_coords[i].w;
		glm::mat4x3 vertex_coords_3d = glm::mat4x3(glm::vec3(vertex_coords[0]), glm::vec3(vertex_coords[1]), glm::vec3(vertex_coords[2]), glm::vec3(vertex_coords[3]));

		glm::mat4x2 tex_coords;
		for (int i = 0; i < 4; ++i) tex_coords[i] = glm::vec2(surface.TextureCoords[i].s, surface.TextureCoords[i].t);

		ShaderType::Use()
			.SetVertexCoords(vertex_coords_3d)
			.SetTextureCoords(tex_coords)
			.SetDiffuse(surface.GetTexture()->ID);
			
		UnitQuad::Draw();
	}

    template<class ShaderType>
	void RenderSurface(const BasicSurface& surface, TexturePosition *maskTexCoords, GLuint maskTextureID)
	{

		glm::mat4 vertex_coords;
		for (int i = 0; i < 4; ++i) vertex_coords[i] = glm::vec4(surface.VertexCoords[i].x, surface.VertexCoords[i].y, surface.VertexCoords[i].z, 1.0f);
		vertex_coords = ProjectionMatrixStack.Top() * ModelViewMatrixStack.Top() * vertex_coords;
		for (int i = 0; i < 4; ++i) vertex_coords[i] /= vertex_coords[i].w;
		glm::mat4x3 vertex_coords_3d = glm::mat4x3(glm::vec3(vertex_coords[0]), glm::vec3(vertex_coords[1]), glm::vec3(vertex_coords[2]), glm::vec3(vertex_coords[3]));

		glm::mat4x4 tex_coords;
		for (int i = 0; i < 4; ++i) tex_coords[i] = glm::vec4(surface.TextureCoords[i].s, surface.TextureCoords[i].t, maskTexCoords[i].s, maskTexCoords[i].t);

		ShaderType::Use()
			.SetVertexCoords(vertex_coords_3d)
			.SetTextureCoords(tex_coords)
			.SetDiffuse(surface.GetTexture()->ID)
			.SetMask(maskTextureID);
			
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, maskTextureID);
		UnitQuad::Draw();
	}

	void Basic2DLayer::Draw(const GLTexture *Image, float X1, float Y1, float X2, float Y2, float X3, float Y3, float X4, float Y4) {
		if(DrawMode==BasicSurface::SetAlpha && Surfaces.GetCount()) {
			BasicSurface *surface=Surfaces[Surfaces.GetCount()-1];
			
			surface->Alpha=new TextureAttachment;
			surface->Alpha->SetTexture(Image);
			if(surface->GetTexture()->W!=Image->W || surface->GetTexture()->H!=Image->H) {
				surface->Alpha->CreateTextureCoords();
				
				surface->Alpha->TextureCoords[0].s=0;
				surface->Alpha->TextureCoords[0].t=0;
				surface->Alpha->TextureCoords[1].s=(float)surface->GetTexture()->W/Image->W;
				surface->Alpha->TextureCoords[1].t=0;
				surface->Alpha->TextureCoords[2].s=0;
				surface->Alpha->TextureCoords[2].t=(float)surface->GetTexture()->H/Image->H;
				surface->Alpha->TextureCoords[3].s=(float)surface->GetTexture()->W/Image->W;
				surface->Alpha->TextureCoords[3].t=(float)surface->GetTexture()->H/Image->H;
			}
		}
		else {
			BasicSurface *surface=Surfaces.Add();

			surface->SetTexture(Image);
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
	}

	void Basic2DLayer::Draw(const GLTexture *Image, float X1, float Y1, float X2, float Y2, float X3, float Y3, float X4, float Y4,  float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) {
		if(DrawMode==BasicSurface::SetAlpha && Surfaces.GetCount()) {
			BasicSurface *surface=Surfaces[Surfaces.GetCount()-1];
			
			surface->Alpha=new TextureAttachment;
			surface->Alpha->SetTexture(Image);
			
			surface->Alpha->CreateTextureCoords();

			float w=Image->W, h=Image->H;
			surface->Alpha->TextureCoords[0].s=S1/w;
			surface->Alpha->TextureCoords[0].t=U1/h;

			surface->Alpha->TextureCoords[1].s=S2/w;
			surface->Alpha->TextureCoords[1].t=U2/h;

			surface->Alpha->TextureCoords[2].s=S3/w;
			surface->Alpha->TextureCoords[2].t=U3/h;

			surface->Alpha->TextureCoords[3].s=S4/w;
			surface->Alpha->TextureCoords[3].t=U4/h;
			}
		else {
			BasicSurface *surface=Surfaces.Add();

			surface->SetTexture(Image);
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
	}

	void Basic2DLayer::DrawTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		Draw(Image, X,Y, X+W,Y, X+W,Y+H, X,Y+H, 0,0, W,0, W,H, 0,H);
	}

	void Basic2DLayer::DrawHTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		Draw(Image, X,Y, X+W,Y, X+W,Y+H, X,Y+H, 0,0, W,0, W,Image->H, 0,Image->H);
	}

	void Basic2DLayer::DrawVTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		Draw(Image, X,Y, X+W,Y, X+W,Y+H, X,Y+H, 0,0, Image->W,0, Image->W,H, 0,H);
	}

	void Basic2DLayer::Render() {
		Rectangle psc=scissors;
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
			if(surface->Alpha) {
				RenderSurface<gge::shaders::MaskedShader>(*surface, surface->Alpha->TextureCoords, surface->Alpha->GetTexture()->ID);
			}
			else {
				RenderSurface<gge::shaders::SimpleShader>(*surface);
			}
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
		if(DrawMode==BasicSurface::SetAlpha && Surfaces.GetCount()) {
			ColorizableSurface *surface=Surfaces[Surfaces.GetCount()-1];
			
			surface->Alpha=new TextureAttachment;
			surface->Alpha->SetTexture(Image);
			if(surface->GetTexture()->W!=Image->W || surface->GetTexture()->H!=Image->H) {
				surface->Alpha->CreateTextureCoords();
				
				surface->Alpha->TextureCoords[0].s=0;
				surface->Alpha->TextureCoords[0].t=0;
				surface->Alpha->TextureCoords[1].s=(float)surface->GetTexture()->W/Image->W;
				surface->Alpha->TextureCoords[1].t=0;
				surface->Alpha->TextureCoords[2].s=0;
				surface->Alpha->TextureCoords[2].t=(float)surface->GetTexture()->H/Image->H;
				surface->Alpha->TextureCoords[3].s=(float)surface->GetTexture()->W/Image->W;
				surface->Alpha->TextureCoords[3].t=(float)surface->GetTexture()->H/Image->H;
			}
		}
		else {
			ColorizableSurface *surface=Surfaces.Add();

			surface->SetTexture(Image);
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
	}

	void Colorizable2DLayer::Draw(const GLTexture *Image, float X1, float Y1, float X2, float Y2, float X3, float Y3, float X4, float Y4,  float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) {
		if(DrawMode==BasicSurface::SetAlpha && Surfaces.GetCount()) {
			ColorizableSurface *surface=Surfaces[Surfaces.GetCount()-1];
			
			surface->Alpha=new TextureAttachment;
			surface->Alpha->SetTexture(Image);
			
			surface->Alpha->CreateTextureCoords();

			float w=Image->W, h=Image->H;
			surface->Alpha->TextureCoords[0].s=S1/w;
			surface->Alpha->TextureCoords[0].t=U1/h;

			surface->Alpha->TextureCoords[1].s=S2/w;
			surface->Alpha->TextureCoords[1].t=U2/h;

			surface->Alpha->TextureCoords[2].s=S3/w;
			surface->Alpha->TextureCoords[2].t=U3/h;

			surface->Alpha->TextureCoords[3].s=S4/w;
			surface->Alpha->TextureCoords[3].t=U4/h;
			}
		else {
			ColorizableSurface *surface=Surfaces.Add();

			surface->SetTexture(Image);
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
	}

	void Colorizable2DLayer::DrawTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		Draw(Image, X,Y, X+W,Y, X+W,Y+H, X,Y+H, 0,0, W,0, W,H, 0,H);
	}

	void Colorizable2DLayer::DrawHTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		Draw(Image, X,Y, X+W,Y, X+W,Y+H, X,Y+H, 0,0, W,0, W,Image->H, 0,Image->H);
	}

	void Colorizable2DLayer::DrawVTiled(const GLTexture *Image,int X,int Y,int W,int H) {
		Draw(Image, X,Y, X+W,Y, X+W,Y+H, X,Y+H, 0,0, Image->W,0, Image->W,H, 0,H);
	}

	void Colorizable2DLayer::Render() {
		Rectangle psc=scissors;
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

		gge::shaders::SimpleTintShader::Use();

		if(ClippingEnabled) {
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

			RGBfloat tint;
			tint.r = surface->Color.r * CurrentLayerColor.r;
			tint.g = surface->Color.g * CurrentLayerColor.g;
			tint.b = surface->Color.b * CurrentLayerColor.b;
			tint.a = surface->Color.a * CurrentLayerColor.a;
			
			if(surface->Alpha) {
				gge::shaders::TintedMaskedShader::Use().SetTint(tint);
				RenderSurface<gge::shaders::TintedMaskedShader>(*surface, surface->Alpha->TextureCoords, surface->Alpha->GetTexture()->ID);
			}
			else {
				gge::shaders::SimpleTintShader::Use().SetTint(tint);
				RenderSurface<gge::shaders::SimpleTintShader>(*surface);
			}
		}

		if(system::OffscreenRendering) {
			system::OffscreenRendering=false;
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			system::SetRenderTarget(0);
			DumpOffscreen();
		}

		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		shaders::SimpleTintShader::Use().SetTint(CurrentLayerColor);
		//glColor4fv(CurrentLayerColor.vect);
		// What are we doing here with this color4fv?
		for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.Last(); i.IsValid(); i.Previous()) {
			i->Render();
		}

end:
		CurrentLayerColor=prevcolor;
		//glColor4fv(prevcolor.vect);
		// Same question.
		gge::shaders::SimpleTintShader::Use().SetTint(prevcolor);
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
