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

#include "../ShaderCode/ShaderCode.h"

using namespace gge::utils;

namespace gge { namespace graphics {

	gge::utils::Logger						log;

	glutil::MatrixStack						ModelViewMatrixStack;
	glutil::MatrixStack						ProjectionMatrixStack;

	static int								offset = 0;
	static const int						buffer_size = 24 * 4 * 8192;

	// 2 triangles (3 vertices each)
	// Each vertex:
	// 2 floats for XY positions, 2 floats for UV texture coordinates
	int UnitQuad::unit_quad[6] =
	{/*
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f*/
		0, 3, 1, 1, 3, 2
	};

	UnitQuad::UnitQuad()
	{
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(int), unit_quad, GL_STATIC_DRAW); // GL_STATIC_DRAW
		/*glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, k_unit_quad_floats * sizeof(float), unit_quad);*/
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(0);
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(0, 1, GL_INT, GL_FALSE, 0, (GLvoid*) 0);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*) 8);
		glVertexAttribIPointer(0, 1, GL_INT, 0, (GLvoid*) 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	UnitQuad::~UnitQuad()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
	void UnitQuad::GLDraw()
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
	void Quad::GLDraw()
	{
		assert(false);
		glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArrays(GL_TRIANGLES, offset / 16, 6);
		glBindVertexArray(0);
	}
	void Quad::UpdateInstanceVertexData(const std::array<float,24>& data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Move offset and orphan once in a while // Don't forget to comment out and uncomment glBufferData code in the UnitQuad constructor

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
		GLuint FBTexture=0;
		GLuint FrameBuffer=0;

		bool OffscreenRendering=false;

		void SetupFrameBuffer();


	}

	os::DeviceHandle Initialize(os::WindowHandle hWnd, int BitDepth, int Width, int Height) {
		using namespace gge::graphics::system;

		os::DeviceHandle handle=0;

		///!Platform specific
#ifdef WIN32
		///*Preparing device context, platform specific
		HDC hDC = GetDC((HWND)hWnd);

		static	PIXELFORMATDESCRIPTOR pfd=	// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
			1,								// Version Number
			PFD_DRAW_TO_WINDOW |			// Format Must Support Window
			PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,				// Must Support Double Buffering
			PFD_TYPE_RGBA,					// Request An RGBA Format
			BitDepth,						// Select Our Color Depth
			0, 0, 0, 0, 0, 0,				// Color Bits Ignored
			0,								// No Alpha Buffer
			0,								// Shift Bit Ignored
			0,								// No Accumulation Buffer
			0, 0, 0, 0,						// Accumulation Bits Ignored
			16,								// 16Bit Z-Buffer (Depth Buffer)
			0,								// No Stencil Buffer
			0,								// No Auxiliary Buffer
			PFD_MAIN_PLANE,					// Main Drawing Layer
			0,								// Reserved
			0, 0, 0							// Layer Masks Ignored
		};

		int PixelFormat=ChoosePixelFormat(hDC,&pfd);
		SetPixelFormat(hDC,PixelFormat,&pfd);


		HGLRC hRC;
		hRC = wglCreateContext( hDC );
		wglMakeCurrent(hDC,hRC);

		handle=(os::DeviceHandle)hDC;

		if(hRC==NULL) {
			os::DisplayMessage("OpenGL", "Context creation failed");
			exit(0);
		}

#elif defined(LINUX)
		static int attributeListDbl[] = {
			GLX_RGBA,
			GLX_DOUBLEBUFFER,
			GLX_RED_SIZE,   1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE,  1,
			None
		};
		Display *display = XOpenDisplay(NULL);

		XVisualInfo *vi = glXChooseVisual(display, DefaultScreen(display), attributeListDbl);
		GLXContext cx = glXCreateContext(display, vi, 0, GL_TRUE);
		glXMakeCurrent(display, hWnd, cx);

		handle=(os::DeviceHandle)display;

		if(cx==NULL) {
			os::DisplayMessage("OpenGL", "Context creation failed");
			exit(0);
		}
#endif

		std::string gl_version(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
		if(utils::StrToNumber<float>(gl_version)<3.0) {
			os::DisplayMessage("OpenGL", ("OpenGL version 3.0 and above is required. Yours is "+gl_version).c_str());
			exit(0);
		}

		ScreenSize.Width=Width;
		ScreenSize.Height=Height;

		///*Setting OpenGL parameters
		glShadeModel(GL_SMOOTH);							// Enables Smooth Shading

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
		glColor4f(1.0f,1.0f,1.0f,1.0f);						// Full Brightness, 50% Alpha ( NEW )

		glClearDepth(1.0f);									// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		//glDepthFunc(GL_LEQUAL);
		glDepthFunc(GL_ALWAYS);

		//Alpha
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);	// Blending Function For Translucency Based On Source Alpha Value ( NEW )

		//textures
		glEnable(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		///*Adjusting Matrices
		glViewport(0, 0, Width, Height);					// Reset The Current Viewport


		//These can be overridden by layers
		/*glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix
		glOrtho(0,Width,Height,0,-100,100);					// Calculate The Aspect Ratio Of The Window
		*/
		glCullFace(GL_BACK);
		//glFrontFace(GL_CW);
		glFrontFace(GL_CCW);

		ProjectionMatrixStack.SetIdentity();
		ProjectionMatrixStack.Orthographic(0.0f, float(Width), float(Height), 0.0f, -100.0f, 100.0f);
		//position
		/*
		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glLoadIdentity();									// Reset The Modelview Matrix
		*/
		ModelViewMatrixStack.SetIdentity();

		gge::system::LoadGLFunctions();

		SetupFrameBuffer();

		return handle;
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
		GLenum getGLColorMode(ColorMode::Type color_mode) {
			switch(color_mode) {
			case ColorMode::Alpha:
				return GL_ALPHA;
			case ColorMode::Grayscale_Alpha:
				return GL_LUMINANCE_ALPHA;
			case ColorMode::BGR:
				return GL_BGR;
			case ColorMode::RGB:
				return GL_RGB;
			case ColorMode::ABGR:
				return GL_RGBA;
			case ColorMode::ARGB:
				return GL_BGRA;
			default:
				return GL_BGRA;
			}
		}
		void SetTexture(Byte *data, int cx, int cy, ColorMode::Type mode) {
			GLenum colormode=getGLColorMode(mode);

			Byte *target=NULL;

			///*Setting Texture Parameters to
			/// Magnify filter: Linear,
			/// Minify filter:  Linear,
			/// Border color:   Transparent
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR, 0x0);
			glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

			if(mode==ColorMode::Alpha) {
				///*If alpha only, converted to grayscale alpha
				glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
				glPixelStorei(GL_PACK_ALIGNMENT, 2);

				target=new Byte[cx*cy*2];
				A8ToA8L8(cx,cy,data,target);
				//delete data;
				data=target;

				mode=ColorMode::Grayscale_Alpha;
				colormode=GL_LUMINANCE_ALPHA;
			}
			else if(mode==ColorMode::Grayscale_Alpha) {
				colormode=GL_LUMINANCE_ALPHA;
			} else {
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
				glPixelStorei(GL_PACK_ALIGNMENT, 4);
			}

			glTexImage2D(GL_TEXTURE_2D,0,getBPP(mode),cx,cy,0,colormode,GL_UNSIGNED_BYTE,data);
			//glTexSubImage2D(GL_TEXTURE_2D,0,0,0,cx,cy,colormode,GL_UNSIGNED_BYTE,data);
		}
		GLTexture GenerateTexture(Byte *data,int cx,int cy,ColorMode::Type mode) {

			GLTexture ret;
			ret.CalcuateCoordinates(cx,cy);

			///*Create the texture object
			glGenTextures(1,&ret.ID);
			glBindTexture(GL_TEXTURE_2D,ret.ID);

			SetTexture(data,cx,cy,mode);

			return ret;
		}
		void UpdateTexture(GLTexture texture, Byte *data,ColorMode::Type mode) {
			glBindTexture(GL_TEXTURE_2D,texture.ID);

			SetTexture(data,texture.W,texture.H,mode);
		}

		void ResizeGL(int Width, int Height) {
			ScreenSize.Width=Width;
			ScreenSize.Height=Height;


			///*Adjusting Matrices
			glViewport(0, 0, Width, Height);					// Reset The Current Viewport


			//These can be overridden by layers
			/*glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
			glLoadIdentity();									// Reset The Projection Matrix
			glOrtho(0,Width,Height,0,-100,100);					// Calculate The Aspect Ratio Of The Window
			*/
			ProjectionMatrixStack.SetIdentity();
			ProjectionMatrixStack.Orthographic(0.0f, float(Width), float(Height), 0.0f, -100.0f, 100.0f);
			glFrontFace(GL_CCW);
			//glFrontFace(GL_CW);

			//position
			/*
			glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
			glLoadIdentity();									// Reset The Modelview Matrix
			*/
			ModelViewMatrixStack.SetIdentity();

			SetupFrameBuffer();

		}

		void DestroyTexture(GLTexture &texture) {
			glDeleteTextures(1, &texture.ID);
			texture.ID=0;
		}

		void PreRender() {
			///*Resetting OpenGL parameters
			glDisable(GL_SCISSOR_TEST);
			/*glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();*/
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

		void SetRenderTarget(GLuint Target) {
			glBindFramebuffer(GL_FRAMEBUFFER, Target);
		}

		void DumpOffscreen() {

			static const glm::mat4x3 vertex_coords = glm::mat4x3(  -1.0f, 1.0f, 0.0f,
																	1.0f, 1.0f, 0.0f,
																	1.0f,-1.0f, 0.0f,
																   -1.0f,-1.0f, 0.0f	);

			static const glm::mat4x2 tex_coords = glm::mat4x2(		0.0f, 1.0f,
																	1.0f, 1.0f,
																	1.0f, 0.0f,
																	0.0f, 0.0f		);
			gge::shadercode::SimpleShader::Use();
			gge::shadercode::SimpleShader::Get().UpdateUniform("vertex_coords", vertex_coords);
			gge::shadercode::SimpleShader::Get().UpdateUniform("tex_coords", tex_coords);
			glBindTexture(GL_TEXTURE_2D, system::FBTexture);
			gge::graphics::UnitQuad::Draw();
		}

		void SetupFrameBuffer() {
			if(FBTexture)
				glDeleteTextures(1, &FBTexture);

			glGenTextures(1, &FBTexture);
			glBindTexture(GL_TEXTURE_2D, FBTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ScreenSize.Width, ScreenSize.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glBindTexture(GL_TEXTURE_2D, 0);

			// Create a FBO in anticipation of render-to-texture
			if(FrameBuffer)
				glDeleteFramebuffers(1, &system::FrameBuffer);
			glGenFramebuffers(1, &system::FrameBuffer);
			system::SetRenderTarget(system::FrameBuffer);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBTexture, 0);

			//GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);

			system::SetRenderTarget(0);
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
