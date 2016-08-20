#include "../Graphics.h"
#include "../GL/OpenGL.h"
#include "../WindowManager.h"
#include "../Geometry/Transform3D.h"

namespace Gorgon { namespace Graphics {

	const Geometry::Pointf TextureSource::fullcoordinates[4]={{0, 0}, {1, 0}, {1, 1}, {0, 1}};

	namespace internal {
		Geometry::Transform3D Transform;

		GL::Texture LastTexture=0;

		int quadvertexindex[] ={
			0, 3, 1,
			1, 3, 2
		};

		GLuint quadvbo;
		std::map<decltype(WindowManager::CurrentContext()), GLuint> vaos;

		void ActivateQuadVertices() {
#ifndef NDEBUG
			if(vaos.count(WindowManager::CurrentContext())==0) {
				throw std::logic_error("Context not initialized???");
			}
#endif
			glBindVertexArray(vaos[WindowManager::CurrentContext()]);
		}

		void DrawQuadVertices() {
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	void Initialize() {
		using namespace internal;

		static bool initialized=false;
		if(!initialized) {
			initialized=true;
			GL::LoadFunctions();

			glGenBuffers(1, &quadvbo);

			glBindBuffer(GL_ARRAY_BUFFER, quadvbo);
			glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(int), quadvertexindex, GL_STATIC_DRAW);
		}

		if(vaos.count(WindowManager::CurrentContext())==0) {
			auto ctx=WindowManager::CurrentContext();
			glGenVertexArrays(1, &vaos[ctx]);
			glBindVertexArray(vaos[ctx]);

			glEnableVertexAttribArray(0);
			glVertexAttribIPointer(0, 1, GL_INT, 0, (GLvoid*)0);
		}
	}

} }



#ifdef nonnnne

#include "Graphics.h"
#include "OpenGL.h"
#include "GGEMain.h"

#ifdef WIN32
#	undef APIENTRY
#	undef WINGDIAPI
#endif

#ifdef WIN32
#	include <windows.h>
#elif defined(LINUX)
#	include <GL/glx.h>
#	include <unistd.h>
#endif

#include "InternalShaders.h"

using namespace gge::utils;

namespace gge { namespace graphics {

	gge::utils::Logger						log;

	glutil::MatrixStack						ModelViewMatrixStack;
	glutil::MatrixStack						ProjectionMatrixStack;

	static int								offset = 0;
	static const int						buffer_size = 24 * 4 * 8192;

	// 2 triangles (3 vertices each)
	// Each vertex:
	// 1 int for index
	int UnitQuadVertices::unit_quad[6] =
	{
		0, 3, 1, 1, 3, 2
	};

	UnitQuadVertices::UnitQuadVertices()
	{
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(int), unit_quad, GL_STATIC_DRAW); // GL_STATIC_DRAW
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribIPointer(0, 1, GL_INT, 0, (GLvoid*) 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	UnitQuadVertices::~UnitQuadVertices()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
	void UnitQuadVertices::GLDraw()
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
	void QuadVertices::GLDraw()
	{
		assert(false);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
	void QuadVertices::UpdateInstanceVertexData(const std::array<float,24>& data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Move offset and orphan once in a while // Don't forget to comment out and uncomment glBufferData code in the UnitQuadVertices constructor

		offset += sizeof(data);
		if (offset >= buffer_size) {
			offset = 0;
			glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);
		}
		float* map = (float*)glMapBufferRange(GL_ARRAY_BUFFER, offset, sizeof(data), GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		std::memcpy(map, data.data(), sizeof(data));
		glUnmapBuffer(GL_ARRAY_BUFFER);

		// BufferData NULL, then MapBuffer
		/*glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_DYNAMIC_DRAW);
		float* map = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(data), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		std::memcpy(map, data.data(), sizeof(data));
		glUnmapBuffer(GL_ARRAY_BUFFER);*/

		// BufferData NULL, then BufferSubData
		/*glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data.data());*/

		// Invalidate map
		/*float* map = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(data), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		std::memcpy(map, data.data(), sizeof(data));
		glUnmapBuffer(GL_ARRAY_BUFFER);*/

		// Blocking BufferSubdata
		/*glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data.data());*/
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Size ScreenSize;
	extern RGBfloat CurrentLayerColor;
	extern utils::Bounds scissors;
	extern Point translate;

	namespace system {
	}


	namespace system {

		void A8ToA8L8(int cx,int cy,Byte *data,Byte *dest)
		{
			int sz=cx*cy;

			for(int i=0;i<sz;i++) {
				dest[i*2]=0xff;
				dest[i*2+1]=data[i];
			}
		}

		void ResizeGL(int Width, int Height) {
			ScreenSize.Width=Width;
			ScreenSize.Height=Height;


			///*Adjusting Matrices
			glViewport(0, 0, Width, Height);					// Reset The Current Viewport


			//These can be overridden by layers
			ProjectionMatrixStack.SetIdentity();
			ProjectionMatrixStack.Orthographic(0.0f, float(Width), float(Height), 0.0f, -100.0f, 100.0f);
			glFrontFace(GL_CCW);

			//position
			ModelViewMatrixStack.SetIdentity();
		}

		void PreRender() {
			///*Resetting OpenGL parameters
			glDisable(GL_SCISSOR_TEST);
			ModelViewMatrixStack.SetIdentity();

			///*Clearing buffers
			glClearColor(0.0,0.0,0.0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			CurrentLayerColor.a=1;
			CurrentLayerColor.r=1;
			CurrentLayerColor.g=1;
			CurrentLayerColor.b=1;

			translate=Point(0,0);
			scissors=utils::Rectangle(0, 0, ScreenSize);
		}

		void PostRender(os::DeviceHandle hDC, os::WindowHandle win) {
			glFlush();
			glFinish();
			///*Swapping back and front buffers
#ifdef WIN32
			SwapBuffers( (HDC)hDC );
#elif defined(LINUX)
			glXSwapBuffers((Display*)hDC, win);
#endif
		}
	}


	const SizeController2D SizeController2D::TileFit(Tile_Continous, Tile_Continous);
	const SizeController2D SizeController2D::StretchFit(Stretch, Stretch);
	const SizeController2D SizeController2D::SingleTopLeft(Single, Single, Alignment::Top_Left);
	const SizeController2D SizeController2D::SingleBottomRight(Single, Single, Alignment::Bottom_Right);
	const SizeController2D SizeController2D::SingleMiddleCenter(Single, Single, Alignment::Middle_Center);

	std::array<float,24> GetVertexData(const BasicSurface& surface)
	{
		std::array<float,24> data;

		// First triangle vertices
		data[0] = surface.VertexCoords[0].x;
		data[1] = surface.VertexCoords[0].y;
		data[2] = surface.TextureCoords[0].s;
		data[3] = surface.TextureCoords[0].t;

		data[4] = surface.VertexCoords[1].x;
		data[5] = surface.VertexCoords[1].y;
		data[6] = surface.TextureCoords[1].s;
		data[7] = surface.TextureCoords[1].t;

		data[8] = surface.VertexCoords[3].x;
		data[9] = surface.VertexCoords[3].y;
		data[10] = surface.TextureCoords[3].s;
		data[11] = surface.TextureCoords[3].t;

		// Second triangle vertices
		data[12] = data[8];
		data[13] = data[9];
		data[14] = data[10];
		data[15] = data[11];

		data[16] = data[4];
		data[17] = data[5];
		data[18] = data[6];
		data[19] = data[7];

		data[20] = surface.VertexCoords[2].x;
		data[21] = surface.VertexCoords[2].y;
		data[22] = surface.TextureCoords[2].s;
		data[23] = surface.TextureCoords[2].t;

		return data;
	}
} }

#endif