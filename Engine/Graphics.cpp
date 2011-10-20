#include "Graphics.h"

#ifdef WIN32
#include <windows.h>


using namespace gge::utils;

#endif

namespace gge { namespace graphics {
	Size ScreenSize;
	extern RGBfloat CurrentLayerColor;
	extern utils::Bounds scissors;
	extern Point translate;

	namespace system { 	
		GLuint FBTexture;
		GLuint FrameBuffer; 

		bool OffscreenRendering=false;
	}

	const GLenum GL_FRAMEBUFFER=0x8D40;
	const GLenum GL_COLOR_ATTACHMENT0=0x8CE0;
	const GLenum GL_GENERATE_MIPMAP=0x8191;

	typedef void(__stdcall *glGenFramebuffers_t)(int, GLuint*);
	typedef void(__stdcall *glBindFramebuffer_t)(GLenum, GLuint);
	typedef void(__stdcall *glFramebufferTexture2D_t)(GLenum, GLenum, GLenum, GLuint, GLint);
	typedef GLenum(__stdcall *glCheckFramebufferStatus_t)(GLenum);
	typedef void(__stdcall *glGenerateMipmap_t)(GLenum);

	glGenFramebuffers_t glGenFramebuffers;
	//glDeleteFramebuffers_t glDeleteFramebuffers;
	glBindFramebuffer_t glBindFramebuffer;
	glFramebufferTexture2D_t glFramebufferTexture2D;
	glCheckFramebufferStatus_t glCheckFramebufferStatus;
	glGenerateMipmap_t glGenerateMipmap;

	os::DeviceHandle Initialize(os::WindowHandle hWnd, int BitDepth, int Width, int Height) {
		using namespace gge::graphics::system;
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
#endif

		const char *gl_extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
		const char *gl_vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		const char *gl_renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		const char *gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));

		bool support_framebuffer_object=false, support_framebuffer_via_ext=false;

		// If OpenGL version >= 3, framebuffer objects are core - enable regardless of extension
		// (the flags are initialised to false)
		if (atof(gl_version) >= 3.0)
		{
			support_framebuffer_object = true;
			support_framebuffer_via_ext = false;
		}
		else
		{
			// Detect framebuffer object support via ARB (for OpenGL version < 3) - also uses non-EXT names
			if (strstr(gl_extensions, "ARB_framebuffer_object") != 0)
			{
				support_framebuffer_object = true;
				support_framebuffer_via_ext = false;
			}
			// Detect framebuffer object support via EXT (for OpenGL version < 3) - uses the EXT names
			else if (strstr(gl_extensions, "EXT_framebuffer_object") != 0)
			{
				support_framebuffer_object = true;
				support_framebuffer_via_ext = true;
			}
		}



		ScreenSize.Width=Width;
		ScreenSize.Height=Height;

		///*Setting OpenGL parameters
		glShadeModel(GL_SMOOTH);							// Enables Smooth Shading

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
		glColor4f(1.0f,1.0f,1.0f,1.0f);						// Full Brightness, 50% Alpha ( NEW )

		glClearDepth(1.0f);									// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);

		//Alpha
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);	// Blending Function For Translucency Based On Source Alpha Value ( NEW )

		//textures
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);


		///*Adjusting Matrices
		glViewport(0, 0, Width, Height);					// Reset The Current Viewport


		//These can be overridden by layers
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix
		glOrtho(0,Width,Height,0,-100,100);					// Calculate The Aspect Ratio Of The Window
		glFrontFace(GL_CW);

		//position
		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glLoadIdentity();									// Reset The Modelview Matrix


		if (support_framebuffer_object)
		{
			// If support is via EXT (OpenGL version < 3), add the EXT suffix; otherwise functions are core (OpenGL version >= 3)
			// or ARB without the EXT suffix, so just get the functions on their own.
			std::string suffix = (support_framebuffer_via_ext ? "EXT" : "");

			glGenFramebuffers = (glGenFramebuffers_t)wglGetProcAddress((std::string("glGenFramebuffers") + suffix).c_str());
			//glDeleteFramebuffers = (glDeleteFramebuffers_t)wglGetProcAddress((std::string("glDeleteFramebuffers") + suffix).c_str());
			glBindFramebuffer = (glBindFramebuffer_t)wglGetProcAddress((std::string("glBindFramebuffer") + suffix).c_str());
			glFramebufferTexture2D = (glFramebufferTexture2D_t)wglGetProcAddress((std::string("glFramebufferTexture2D") + suffix).c_str());
			glCheckFramebufferStatus = (glCheckFramebufferStatus_t)wglGetProcAddress((std::string("glCheckFramebufferStatus") + suffix).c_str());
			glGenerateMipmap = (glGenerateMipmap_t)wglGetProcAddress((std::string("glGenerateMipmap") + suffix).c_str());

			glGenTextures(1, &FBTexture);
			glBindTexture(GL_TEXTURE_2D, FBTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ScreenSize.Width, ScreenSize.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glBindTexture(GL_TEXTURE_2D, 0);

			// Create a FBO in anticipation of render-to-texture
			glGenFramebuffers(1, &system::FrameBuffer);
			system::SetRenderTarget(system::FrameBuffer);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBTexture, 0);

			GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);

			system::SetRenderTarget(0);
		} else {
			system::FrameBuffer=0;
			os::DisplayMessage("Device compatibility", "No frame buffer is supported, there might be problems with rendering.");
		}


		return (os::DeviceHandle)hDC;
	}

	namespace system {
		void A8ToA8L8(int cx,int cy,Byte *data,Byte *dest)
		{
			int icx=cx,icy=cy;

			__asm {
				; A8 (actually saved as a) data to 
					; A8L8

					; push everything so we can use them at will
					push edi		; destination array
					push esi		; source array
					push ecx		; x
					push edx		; y
					push ebx		; temp

					mov  edi,dest	; destination data
					mov  esi,data	; source data


					mov edx,0		; y=0
	loopystart:
				cmp edx,[icy]	; if y=cy
					je loopyend		; break y loop

					mov ecx,0		; x=0
	loopxstart:
				cmp ecx,[icx]	; if x=cx
					je loopxend		; break x loop

					mov byte ptr[edi],0xff
					inc edi

					movsb

					inc ecx			; x++
					jmp loopxstart	; next
	loopxend:
				inc edx			; y++
					jmp loopystart	; next
	loopyend:

				; restore everything
					pop ebx
					pop ecx
					pop edx
					pop esi
					pop edi
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

			int bytes=getBPP(mode);

			if(mode==ColorMode::Alpha) {
				///*If alpha only, converted to grayscale alpha
				glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
				glPixelStorei(GL_PACK_ALIGNMENT, 2);

				target=new Byte[cx*cy*2];	
				A8ToA8L8(cx,cy,data,target);
				delete data;
				data=target;

				mode=ColorMode::Grayscale_Alpha;
				colormode=GL_LUMINANCE_ALPHA;
			} else {
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
				glPixelStorei(GL_PACK_ALIGNMENT, 4);
			}

			glTexImage2D(GL_TEXTURE_2D,0,getBPP(mode),cx,cy,0,colormode,GL_UNSIGNED_BYTE,NULL);
			glTexSubImage2D(GL_TEXTURE_2D,0,0,0,cx,cy,colormode,GL_UNSIGNED_BYTE,data);
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

		void DestroyTexture(GLTexture *texture) {
			glDeleteTextures(1, &texture->ID);
			texture->ID=0;
		}

		void PreRender() {
			///*Resetting OpenGL parameters
			glDisable(GL_SCISSOR_TEST);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			///*Clearing buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			CurrentLayerColor.a=1;
			CurrentLayerColor.r=1;
			CurrentLayerColor.g=1;
			CurrentLayerColor.b=1;

			translate=Point(0,0);
			scissors=utils::Rectangle(0, 0, ScreenSize);
		}

		void PostRender(os::DeviceHandle hDC) {
			glFlush();
			///*Swapping back and front buffers
			SwapBuffers( (HDC)hDC );
		}

		void SetRenderTarget(GLuint Target) {
			glBindFramebuffer(GL_FRAMEBUFFER, Target);
		}

		void DumpOffscreen() {
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glBindTexture(GL_TEXTURE_2D, system::FBTexture);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);
				glVertex3f(0,0,0);
				glTexCoord2f(1,1);
				glVertex3f((float)ScreenSize.Width,0,0);
				glTexCoord2f(1,0);
				glVertex3f((float)ScreenSize.Width, (float)ScreenSize.Height,0);
				glTexCoord2f(0,0);
				glVertex3f(0, (float)ScreenSize.Height,0);
			glEnd();
			glPopMatrix();
		}
	}

	const SizeController2D SizeController2D::TileFit(Tile_Continous, Tile_Continous);
	const SizeController2D SizeController2D::StretchFit(Stretch, Stretch);
	const SizeController2D SizeController2D::SingleTopLeft(Single, Single, Alignment::Top_Left);
	const SizeController2D SizeController2D::SingleBottomRight(Single, Single, Alignment::Bottom_Right);
	const SizeController2D SizeController2D::SingleMiddleCenter(Single, Single, Alignment::Middle_Center);


} }
