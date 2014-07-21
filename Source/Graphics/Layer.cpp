#include "Layer.h"

namespace Gorgon { namespace Graphics {

	void Layer::Render() {
		internal::Transform.Push();

		for(auto &surface : surfaces) {

		}

		Layer::Render();

		internal::Transform.Pop();
	}

} }


#ifdef aaaa


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

	namespace system { GLuint lasttexture=0; }
	template<class ShaderType>
	// ShaderType has to inherit from ShaderBase, uses lasttexture, if there is another function to modify
	// texture parameters apart from rendersurface, lasttexture should be set 0 prior to calling this function
	void RenderSurface(const BasicSurface& surface)
	{
		glm::mat4 vertex_coords;
		for (int i = 0; i < 4; ++i) vertex_coords[i] = glm::vec4(surface.VertexCoords[i].x, surface.VertexCoords[i].y, surface.VertexCoords[i].z, 1.0f);
		vertex_coords = ProjectionMatrixStack.Top() * ModelViewMatrixStack.Top() * vertex_coords;
		for (int i = 0; i < 4; ++i) vertex_coords[i] /= vertex_coords[i].w;
		glm::mat4x3 vertex_coords_3d = glm::mat4x3(glm::vec3(vertex_coords[0]), glm::vec3(vertex_coords[1]), glm::vec3(vertex_coords[2]), glm::vec3(vertex_coords[3]));

		glm::mat4x2 tex_coords;
		for (int i = 0; i < 4; ++i) tex_coords[i] = glm::vec2(surface.TextureCoords[i].s, surface.TextureCoords[i].t);

		if(system::lasttexture==surface.GetTexture()->ID) {
			ShaderType::Use()
				.SetVertexCoords(vertex_coords_3d)
				.SetTextureCoords(tex_coords);
		}
		else {
			ShaderType::Use()
				.SetVertexCoords(vertex_coords_3d)
				.SetTextureCoords(tex_coords)
				.SetDiffuse(surface.GetTexture()->ID);
			system::lasttexture=surface.GetTexture()->ID;
		}

			
		UnitQuad::Draw();
	}

	// ShaderType has to inherit from ShaderBase, uses lasttexture, if there is another function to modify
	// texture parameters apart from rendersurface, lasttexture should be set 0 prior to calling this function
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

		if(system::lasttexture==surface.GetTexture()->ID) {
			ShaderType::Use()
				.SetVertexCoords(vertex_coords_3d)
				.SetTextureCoords(tex_coords)
				.SetMask(maskTextureID);
		}
		else {
			ShaderType::Use()
				.SetVertexCoords(vertex_coords_3d)
				.SetTextureCoords(tex_coords)
				.SetDiffuse(surface.GetTexture()->ID)
				.SetMask(maskTextureID);
			system::lasttexture=surface.GetTexture()->ID;
		}
			
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

		system::lasttexture=0;
		for(i=0;i<Surfaces.GetCount();i++) {
			BasicSurface *surface=Surfaces[i];

			if(surface->VertexCoords[0].y+translate.y<scissors.Top && surface->VertexCoords[1].y+translate.y<scissors.Top && 
				surface->VertexCoords[2].y+translate.y<scissors.Top && surface->VertexCoords[3].y+translate.y<scissors.Top)
				continue;

			if(surface->VertexCoords[0].y+translate.y>scissors.Bottom && surface->VertexCoords[1].y+translate.y>scissors.Bottom &&
				surface->VertexCoords[2].y+translate.y>scissors.Bottom && surface->VertexCoords[3].y+translate.y>scissors.Bottom)
				continue;

			if(surface->VertexCoords[0].x+translate.x<scissors.Left && surface->VertexCoords[1].x+translate.x<scissors.Left && 
				surface->VertexCoords[2].x+translate.x<scissors.Left && surface->VertexCoords[3].x+translate.x<scissors.Left)
				continue;

			if(surface->VertexCoords[0].x+translate.x>scissors.Right && surface->VertexCoords[1].x+translate.x>scissors.Right &&
				surface->VertexCoords[2].x+translate.x>scissors.Right && surface->VertexCoords[3].x+translate.x>scissors.Right)
				continue;

			if(surface->Alpha) {
				RenderSurface<gge::shaders::MaskedShader>(*surface, surface->Alpha->TextureCoords, surface->Alpha->GetTexture()->ID);
			}
			else {
				RenderSurface<gge::shaders::SimpleShader>(*surface);
			}
		}


		for(auto i=SubLayers.Last(); i.IsValid(); i.Previous()) {
			i->Render();
		}

end:
		translate-=BoundingBox.TopLeft();

		if(ClippingEnabled) {
			scissors=psc;
			glScissor(scissors.Left, (ScreenSize.Height-scissors.Top)-scissors.Height(), scissors.Width(), scissors.Height());
		}

		//make sure everything is back in its place
		glPopAttrib();
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
		CurrentLayerColor=CurrentLayerColor*RGBfloat(Ambient);
		
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

		system::lasttexture=0;
		for(i=0;i<Surfaces.GetCount();i++) {
			ColorizableSurface *surface=Surfaces[i];

			RGBfloat tint;
			tint = surface->Color * CurrentLayerColor;

			if(surface->VertexCoords[0].y+translate.y<scissors.Top && surface->VertexCoords[1].y+translate.y<scissors.Top && 
				surface->VertexCoords[2].y+translate.y<scissors.Top && surface->VertexCoords[3].y+translate.y<scissors.Top)
				continue;

			if(surface->VertexCoords[0].y+translate.y>scissors.Bottom && surface->VertexCoords[1].y+translate.y>scissors.Bottom &&
				surface->VertexCoords[2].y+translate.y>scissors.Bottom && surface->VertexCoords[3].y+translate.y>scissors.Bottom)
				continue;

			if(surface->VertexCoords[0].x+translate.x<scissors.Left && surface->VertexCoords[1].x+translate.x<scissors.Left && 
				surface->VertexCoords[2].x+translate.x<scissors.Left && surface->VertexCoords[3].x+translate.x<scissors.Left)
				continue;

			if(surface->VertexCoords[0].x+translate.x>scissors.Right && surface->VertexCoords[1].x+translate.x>scissors.Right &&
				surface->VertexCoords[2].x+translate.x>scissors.Right && surface->VertexCoords[3].x+translate.x>scissors.Right)
				continue;

			if(surface->Alpha) {
				gge::shaders::TintedMaskedShader::Use().SetTint(tint);
				RenderSurface<gge::shaders::TintedMaskedShader>(*surface, surface->Alpha->TextureCoords, surface->Alpha->GetTexture()->ID);
			}
			else {
				gge::shaders::SimpleTintShader::Use().SetTint(tint);
				RenderSurface<gge::shaders::SimpleTintShader>(*surface);
			}
		}
		
		// What are we doing here with this color4fv?
		//! Sublayers are no longer force-colorized
		for(auto i=SubLayers.Last(); i.IsValid(); i.Previous()) {
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
	}


} }

#endif