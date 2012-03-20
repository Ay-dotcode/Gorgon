#include "GGEMain.h"
#include "Sound.h"
#include "Pointer.h"
#include "Graphics.h"
#include "Input.h"
#include "Animation.h"
//#include "../Widgets/WidgetMain.h"
#include "..\Utils\Rectangle2D.h"
#include "..\Resource\Main.h"
#include "VideoClip.h"

using namespace gge::utils;

namespace gge { namespace animation {
	void Animator_Signal();
} }

namespace gge {
	GGEMain Main;

	GGEMain::GGEMain() :
		SystemName(""),
		Width(800)  ,
		Height(600),
		BitDepth(32),
		FullScreen(false),
		BeforeRenderEvent("BeforeRender", this),
		AfterRenderEvent("AfterRender", this),
		Window(NULL)
	{
		BoundingBox=utils::Bounds(0,0, Width,Height);
		isVisible=true;

		CurrentTime=os::GetTime();

		FPS=50;

		srand((unsigned)time(NULL));
	}

	void GGEMain::Setup(string SystemName, int Width, int Height, int BitDepth, bool FullScreen) {
#ifdef _DEBUG
		if(Window!=NULL)
			throw std::runtime_error("System already initialized.");
#else
		if(Window!=NULL)
			return;
#endif
		this->SystemName=SystemName;
		this->Width=Width;
		this->Height=Height;
		this->BitDepth=BitDepth;
		this->FullScreen=FullScreen;

		BoundingBox.SetSize(Width, Height);
	}

	GGEMain::~GGEMain() {
	}

	void GGEMain::BeforeGameLoop() {
		static unsigned int lastFPSUpdate=CurrentTime;
		static int fpscounter=0;
		///*Setting game time
		CurrentTime=os::GetTime();
		if(CurrentTime-lastFPSUpdate>=1000) {
			FPS=fpscounter;
			fpscounter=1;
			lastFPSUpdate=CurrentTime;
		} else
			fpscounter++;

		os::system::ProcessMessage();
		input::system::ProcessMousePosition(Window);

		BeforeGameLoopEvent();
	}

	void GGEMain::BeforeRender() {
		BeforeRenderEvent();

		///*Processing interval objects
		for(utils::Collection<IntervalObject>::Iterator interval=IntervalObjects.First();interval.IsValid();interval.Next()) {
			if(interval->Enabled)
				if(CurrentTime-interval->LastSignal>interval->Timeout) {
					interval->Signal(*interval);
					interval->LastSignal=CurrentTime;
				}
		}

		gge::animation::Animator_Signal();
	}

	void IntervalObject::Reset() {
		Enabled=true;
		LastSignal=Main.CurrentTime;
	}

	void GGEMain::AfterRender() {
		AfterRenderEvent();

		if(os::GetTime()-CurrentTime<16)
			os::Sleep(16-(os::GetTime()-CurrentTime));
	}

	void GGEMain::Render() {
		graphics::system::PreRender();
		LayerBase::Render();
		graphics::system::PostRender(Device);
	}

	IntervalObject *GGEMain::RegisterInterval(unsigned int Timeout, IntervalSignalEvent Signal) {
		IntervalObject *interval=new IntervalObject;

		interval->Enabled=true;
		interval->LastSignal=CurrentTime;
		interval->Timeout=Timeout;
		interval->Signal=Signal;

		IntervalObjects.Add(interval);

		return interval;
	}

	void GGEMain::UnregisterInterval(IntervalObject *Interval) {
		IntervalObjects.Delete(Interval);
	}

	void GGEMain::InitializeAll(string Title, os::IconHandle Icon, int X, int Y) {
		InitializeOS();
		CreateWindow(Title, Icon, X, Y);
		InitializeGraphics();
		InitializeSound();
		InitializeInput();

		InitializeAnimation();
		InitializeResource();
		InitializePointer();
		InitializeMultimedia();
	}

	void GGEMain::InitializeAll(string Title, os::IconHandle Icon) {
		Rectangle r=os::window::UsableScreenMetrics();
		InitializeAll(Title, Icon, r.Left+(r.Width-Width)/2, r.Top+(r.Height-Height)/4);
	}

	os::DeviceHandle GGEMain::InitializeGraphics() {
		Device = gge::graphics::Initialize(Window, BitDepth, Width, Height); return Device;
	}

	void GGEMain::InitializeSound() {
		gge::sound::system::InitializeSound(Window);
	}

	void GGEMain::InitializeInput() {
		gge::input::Initialize();
	}

	void GGEMain::InitializeOS() {
		gge::os::Initialize();
	}

	void GGEMain::InitializeAnimation() {
		gge::animation::Initialize(*this);
	}

	void GGEMain::InitializePointer() {
		Pointers.Initialize(*this);
	}

	os::WindowHandle GGEMain::CreateWindow( string Title, os::IconHandle Icon, int X/*=0*/, int Y/*=0*/ ) {
		Window=os::window::CreateWindow(SystemName,Title,Icon,X,Y,Width,Height,BitDepth,FullScreen);
		return Window;
	}

	void GGEMain::InitializeResource() {
		gge::resource::Init(*this);
	}

	void GGEMain::Exit(int code) {
		BeforeTerminate();

		for(auto it=SubLayers.First();it.IsValid();it.Next())
			it->parent=NULL;

		os::Quit(code);
	}

	void GGEMain::InitializeMultimedia() {
		multimedia::Initialize();
	}

	void GGEMain::Run() {
		gge::os::window::Destroyed.RegisterLambda([&]{ Exit(0); });

		while(true) {
			BeforeGameLoop();
			BeforeRender();
			Render();
			AfterRender();
		}
	}

	//void GGEMain::InitializeWidgets() {
	//	gge::widgets::InitializeWidgets(this);
	//}

}