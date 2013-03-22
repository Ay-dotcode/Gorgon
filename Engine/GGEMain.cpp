#include "GGEMain.h"
#include "Sound.h"
#include "Pointer.h"
#include "Graphics.h"
#include "Input.h"
#include "Animation.h"
#include "../Utils/Rectangle2D.h"
#include "../Resource/Main.h"
//#include "VideoClip.h"
#include <algorithm>
#include <functional>
#include "../Multimedia/Multimedia.h"

using namespace gge::utils;
using namespace std;

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
		GameLoopEvent("GameLoop", this),
		Window((os::WindowHandle)NULL)
	{
		BoundingBox=utils::Bounds(0,0, Width,Height);
		IsVisible=true;

		CurrentTime=os::GetTime();

		FPS=50;

		srand((unsigned)(time(NULL))+CurrentTime);
	}

	void GGEMain::Setup(string SystemName, int Width, int Height, int BitDepth, bool FullScreen) {
#ifdef _DEBUG
		if(Window!=(os::WindowHandle)NULL)
			throw std::runtime_error("System already initialized.");
#else
		if(Window!=(os::WindowHandle)NULL)
			return;
#endif
		adjustlayers(utils::Size(Width, Height));

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

		while(oncelist.size()) {
			oncelist.back()();
			oncelist.pop_back();
		}

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
		graphics::system::PostRender(Device, Window);
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
	
	void GGEMain::initializerest(string Title, os::IconHandle Icon, int X, int Y) {
		CreateWindow(Title, Icon, X, Y);
		InitializeGraphics();
		InitializeSound();
		InitializeInput();

		InitializeAnimation();
		InitializeResource();
		InitializePointer();
		InitializeMultimedia();
	}

	void GGEMain::InitializeAll(string Title, os::IconHandle Icon, int X, int Y) {
		InitializeOS();
		initializerest(Title, Icon, X, Y);
	}

	void GGEMain::InitializeAll(string Title, os::IconHandle Icon) {
		InitializeOS();
		Rectangle r=os::window::UsableScreenMetrics();
		initializerest(Title, Icon, r.Left+(r.Width-Width)/2, r.Top+(r.Height-Height)/4);
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

	void GGEMain::Cleanup() {
		BeforeTerminateEvent();

		for(auto it=SubLayers.First();it.IsValid();it.Next())
			it->parent=NULL;

		isrunning=false;
	}

	void GGEMain::Exit(int code) {
		Cleanup();

		os::Quit(code);
	}

	void GGEMain::InitializeMultimedia() {
		multimedia::Initialize(*this);
	}

	void GGEMain::Run() {
		gge::os::window::Destroyed.RegisterLambda([&]{ 
			Cleanup();
		});
		
		isrunning=true;

		while(true) {
			BeforeGameLoop();

			if(!isrunning)
				break;

			GameLoopEvent();
                        
                        BeforeRender();
			Render();
			AfterRender();
		}
	}

	void GGEMain::ResizeWindow(utils::Size size) {
		os::window::ResizeWindow(Window, size);

		Width=size.Width;
		Height=size.Height;

		graphics::system::ResizeGL(size.Width, size.Height);

		adjustlayers(size);
		ResizeEvent();
	}

	void GGEMain::adjustlayers(utils::Size size) {
		adjustlayers_recurse(*this, this->BoundingBox.GetSize(), size);
	}

	void GGEMain::adjustlayers_recurse(LayerBase &layer, utils::Size from, utils::Size to) {
		std::for_each(layer.SubLayers.begin(), layer.SubLayers.end(), 
			std::bind(&GGEMain::adjustlayers_recurse, std::placeholders::_1, from, to)
		);
		if(layer.BoundingBox.GetSize()==from)
			layer.BoundingBox.SetSize(to);
	}

	void GGEMain::CenterWindow() {
		Rectangle r=os::window::UsableScreenMetrics();
		MoveWindow(r.Left+(r.Width-Width)/2, r.Top+(r.Height-Height)/4);
	}

	void GGEMain::RegisterOnce(std::function<void()> fn) {
		oncelist.push_back(fn);
	}

	//void GGEMain::InitializeWidgets() {
	//	gge::widgets::InitializeWidgets(this);
	//}

}