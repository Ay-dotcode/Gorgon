#include "../Graphics.h"
#include "../GL/OpenGL.h"
#include "../WindowManager.h"
#include "../Geometry/Transform3D.h"

namespace Gorgon { namespace Graphics {

	const Geometry::Pointf TextureSource::fullcoordinates[4]={{0, 0}, {1, 0}, {1, 1}, {0, 1}};

	namespace internal {

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

} }

#endif
