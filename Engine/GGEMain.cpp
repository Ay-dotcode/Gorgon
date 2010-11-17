#include "GGEMain.h"
#include <atlbase.h>

namespace gge {
	GGEMain *main;

	GGEMain::GGEMain(char *SystemName, InstanceHandle Instance, int Width, int Height, int BitDepth, bool FullScreen) :
		SystemName(SystemName),
		Instance(Instance),
		Width(Width)  ,
		Height(Height),
		BitDepth(BitDepth),
		FullScreen(FullScreen),
		BeforeRenderEvent("BeforeRender", this),
		AfterRenderEvent("AfterRender", this)
	{

		W=Width;
		H=Height;
		X=0;
		Y=0;

		gge::AddPointerTarget(this,0);

		Window_Activate=NULL;
		Window_Deactivate=NULL;
		Window_Destroy=NULL;

		CurrentTime=GetTime();

		CoInitialize(NULL);
		main=this;

		FPS=50;
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
		LastSignal=main->CurrentTime;
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
		IntervalObjects.Remove(Interval);

		delete Interval;
	}

	void GGEMain::InitializeAll(const char *Title, gge::IconHandle Icon, int X, int Y) {
		main->InitializeOS();
		main->CreateWin(Title, Icon, X, Y);
		main->InitializeGraphics();
		main->InitializeSound();
		main->InitializeInput();
		main->InitializeAnimation();
		main->InitializePointer();
		main->InitializeWidgets();
	}

}