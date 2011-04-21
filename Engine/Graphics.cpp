#include "Graphics.h"
#ifdef WIN32
#include <windows.h>

#endif
namespace gge { namespace graphics {
	Point ScreenSize;
	extern RGBfloat CurrentLayerColor;
	extern int scX,scY,scW,scH;
	extern int trX,trY;

	os::DeviceHandle Initialize(os::WindowHandle hWnd, int BitDepth, int Width, int Height) {
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

		ScreenSize.x=Width;
		ScreenSize.y=Height;

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

		return (os::DeviceHandle)hDC;
	}
	RECT makerect(int x, int y, int w, int h) {	
		RECT ret;
		ret.left=x;
		ret.right=x+w;
		ret.top=y;
		ret.bottom=y+h;

		return ret;
	}
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
	void SetTexture(Byte *data, int cx, int cy, ColorMode mode) {
		GLenum colormode=getGLColorMode(mode);

		Byte *target=NULL;

		///*Setting Texture Parameters to
		/// Magnify filter: Linear,
		/// Minify filter:  Linear,
		/// Border color:   Transparent
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR, 0x0);

		if(mode==ALPHAONLY_8BPP) {
			///*If alpha only, converted to grayscale alpha
			glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
			glPixelStorei(GL_PACK_ALIGNMENT, 2);

			target=new Byte[cx*cy*2];	
			A8ToA8L8(cx,cy,data,target);
			delete data;
			data=target;

			mode=AGRAYSCALE_16BPP;
			colormode=GL_LUMINANCE_ALPHA;
		} else {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glPixelStorei(GL_PACK_ALIGNMENT, 4);
		}

		glTexImage2D(GL_TEXTURE_2D,0,getBPP(mode),sl2(cx),sl2(cy),0,colormode,GL_UNSIGNED_BYTE,NULL);
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,cx,cy,colormode,GL_UNSIGNED_BYTE,data);
	}
	GLTexture GenerateTexture(Byte *data,int cx,int cy,ColorMode mode) {

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

	GLenum getGLColorMode(ColorMode color_mode) {
		switch(color_mode) {
		case ALPHAONLY_8BPP:
			return GL_ALPHA;
		case AGRAYSCALE_16BPP:
			return GL_LUMINANCE_ALPHA;
		case RGB:
			return GL_BGR;
		case BGR:
			return GL_RGB;
		case ABGR_32BPP:
			return GL_RGBA;
		case ARGB_32BPP:
			return GL_BGRA;
		default:
			return GL_BGRA;
		}
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

		trX=0;
		trY=0;
		scX=scY=0;
		scW=ScreenSize.x;
		scH=ScreenSize.y;
	}

	void PostRender(os::DeviceHandle hDC) {
		///*Swapping back and front buffers
		SwapBuffers( (HDC)hDC );
	}
} }
