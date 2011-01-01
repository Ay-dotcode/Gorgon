#include "GGEMain.h"
#include <atlbase.h>

namespace gge {
	GGEMain Main;

	GGEMain::GGEMain() :
		SystemName(""),
		Instance(NULL),
		Width(800)  ,
		Height(600),
		BitDepth(32),
		FullScreen(false),
		BeforeRenderEvent("BeforeRender", this),
		AfterRenderEvent("AfterRender", this),
		Window(NULL)
	{

		X=0;
		Y=0;
		isVisible=true;

		CurrentTime=GetTime();

		CoInitialize(NULL);
		FPS=50;
	}

	void GGEMain::Setup(string SystemName, InstanceHandle Instance,int Width, int Height, int BitDepth, bool FullScreen) {
#ifdef _DEBUG
		if(Window!=NULL)
			throw std::runtime_error("System already initialized.");
#elif
		if(Window!=NULL)
			return;
#endif
		this->SystemName=SystemName;
		this->Instance=Instance;
		this->Width=W=Width;
		this->Height=H=Height;
		this->BitDepth=BitDepth;
		this->FullScreen=FullScreen;
	}

	GGEMain::~GGEMain() {
		CoUninitialize();
	}

	void GGEMain::BeforeGameLoop() {
		static unsigned int lastFPSUpdate=CurrentTime;
		static int fpscounter=0;
		///*Setting game time
		CurrentTime=GetTime();
		if(CurrentTime-lastFPSUpdate>=1000) {
			FPS=fpscounter;
			fpscounter=1;
			lastFPSUpdate=CurrentTime;
		} else
			fpscounter++;

		gge::ProcessMessage();
		ProcessMousePosition(Window);
	}

	void GGEMain::BeforeRender() {
		BeforeRenderEvent();

		///*Processing interval objects
		IntervalObjects.ResetIteration();
		IntervalObject *interval;

		while(interval=IntervalObjects.next()) {
			if(interval->Enabled)
				if(CurrentTime-interval->LastSignal>interval->Timeout) {
					interval->Signal(interval, interval->Data);
					interval->LastSignal=CurrentTime;
				}
		}

	}

	void IntervalObject::Reset() {
		Enabled=true;
		LastSignal=Main.CurrentTime;
	}

	void GGEMain::AfterRender() {
		AfterRenderEvent();

		Sleep(1);
	}

	void GGEMain::Render() {
		gge::PreRender();
		LayerBase::Render();
		gge::PostRender(Device);
	}

	IntervalObject *GGEMain::RegisterInterval(unsigned int Timeout, void* Data, IntervalSignalEvent Signal) {
		IntervalObject *interval=new IntervalObject;

		interval->Data=Data;
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

	void GGEMain::InitializeAll(string Title, gge::IconHandle Icon, int X, int Y) {
		InitializeOS();
		CreateWin(Title, Icon, X, Y);
		InitializeGraphics();
		InitializeSound();
		InitializeInput();
		gge::AddPointerTarget(this,0); 

		InitializeAnimation();
		InitializePointer();
		InitializeWidgets();
	}

}