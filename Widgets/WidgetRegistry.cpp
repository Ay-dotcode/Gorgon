#include "WidgetRegistry.h"
#include "LinearOrganizer.h"
#include "../Resource/DataResource.h"

using namespace gge::resource;

namespace gge { namespace widgets {
	
	void WidgetRegistry::DiscoverWidgets(ResourceFile *file) {
		//these indices subject to change

		DataResource *colors=file->Root().asData(0);
		textColor=colors->getInt(0);
		titleColor=colors->getInt(1);
		hintColor=colors->getInt(2);

		Pointers.Fetch(file->Root().asFolder(1));
		
		FolderResource *fonts=file->Root().asFolder(2);
		fonttheme=dynamic_cast<resource::FontTheme *>(fonts->getItem(0));
		normal=fonts->asBitmapFont(1);
		if(fonts->getCount()>2)
			small=fonts->asBitmapFont(2);
		else
			small=normal;
		if(fonts->getCount()>3)
			bold=fonts->asBitmapFont(3);
		else
			bold=normal;
		if(fonts->getCount()>4)
			italic=fonts->asBitmapFont(4);
		else
			italic=normal;
		if(fonts->getCount()>5)
			h3=fonts->asBitmapFont(5);
		else
			h3=normal;
		if(fonts->getCount()>6)
			h2=fonts->asBitmapFont(6);
		else
			h2=h3;
		if(fonts->getCount()>7)
			h1=fonts->asBitmapFont(7);
		else
			h1=h2;

		FolderResource *frames=file->Root().asFolder(3);

		background=frames->asImage(0);
		background->SetResizingOptions(ResizableObject::Tile, ResizableObject::Tile);

		selectionFrame=dynamic_cast<RectangleResource*>(frames->getItem(1));

		if(frames->getCount()>1)
			dialogFrame=dynamic_cast<RectangleResource*>(frames->getItem(2));
		else
			dialogFrame=selectionFrame;

		if(frames->getCount()>2)
			thickFrame=dynamic_cast<RectangleResource*>(frames->getItem(3));
		else
			thickFrame=selectionFrame;

		if(frames->getCount()>3)
			thinFrame=dynamic_cast<RectangleResource*>(frames->getItem(4));
		else
			thinFrame=selectionFrame;

		if(frames->getCount()>4)
			modalFrame=dynamic_cast<RectangleResource*>(frames->getItem(5));
		else
			modalFrame=dialogFrame;

		if(frames->getCount()>5)
			pictureFrame=dynamic_cast<RectangleResource*>(frames->getItem(6));
		else
			pictureFrame=thinFrame;

		if(frames->getCount()>6)
			photoFrame=dynamic_cast<RectangleResource*>(frames->getItem(7));
		else
			photoFrame=thickFrame;


		FolderResource *separators=file->Root().asFolder(4);
		hSeparator=dynamic_cast<LineResource*>(separators->getItem(0));
		vSeparator=dynamic_cast<LineResource*>(separators->getItem(1));


		FolderResource *buttons=file->Root().asFolder(5);

		button=dynamic_cast<ButtonBP*>(buttons->getItem(0));
		if(buttons->getCount()>1)
			decorativeButton=dynamic_cast<ButtonBP*>(buttons->getItem(1));
		else
			decorativeButton=button;

		largeButton=button;
		smallButton=button;
		actionButton=button;
		dialogButton=button;
		browseButton=button;
		symbolButton=button;
		toolButton=button;
		navigationButton=button;
		menuButton=button;
		pictureChooser=button;
		colorChooser=button;


		FolderResource *textboxes=file->Root().asFolder(6);
		textbox=dynamic_cast<TextboxBP*>(textboxes->getItem(0));
		if(textboxes->getCount()>1)
			numberbox=dynamic_cast<TextboxBP*>(textboxes->getItem(1));
		else
			numberbox=textbox;
		shortTextbox=textbox;
		signitureTextbox=textbox;

		FolderResource *checkboxes=file->Root().asFolder(7);
		radioButton=checkbox=dynamic_cast<CheckboxBP*>(checkboxes->getItem(0));
		if(checkboxes->getCount()>1)
			tabButton=dynamic_cast<CheckboxBP*>(checkboxes->getItem(1));
		else
			tabButton=radioButton;


		FolderResource *sliders=file->Root().asFolder(8);

		slider=progressor=dynamic_cast<SliderBP*>(sliders->getItem(0));
		scroller=dynamic_cast<SliderBP*>(sliders->getItem(1));

		dialogScroller=scroller;
		controlScroller=scroller;
		pictureScroller=scroller;
		fileProgressor=progressor;
		drawingProgressor=progressor;
		loadingProgressor=progressor;
		downloadProgressor=progressor;
		selectbar=slider;
		indicatorbar=slider;
		radiobar=slider;
		percentbar=progressor;
		namedslider=slider;
		heatbar=progressor;
		spinner=slider;

		if(file->Root().Subitems.getCount()>9) {
			FolderResource *frames=file->Root().asFolder(9);

			if(frames->Subitems.getCount()>0)
				DialogBP=dynamic_cast<FrameBP*>(frames->getItem(0));
			else
				DialogBP=new FrameBP(dialogFrame, dialogScroller);
		}

	}

	Button &WidgetRegistry::createButton(IWidgetContainer &container, string text, int x, int y, int w, int h) {
		Button *btn=new Button(button, container);
		btn->SetText(text);
		btn->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		btn->Resize(w,h);

		return *btn;
	}

	Button &WidgetRegistry::createButton(string text, int w, int h, int x, int y) {
		Button *btn=new Button(button);
		btn->SetText(text);
		btn->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		btn->Resize(w,h);

		return *btn;
	}

	Button &WidgetRegistry::createDialogButton(IWidgetContainer &container, string text, int x, int y, int w, int h) {
		Button *btn=new Button(button, container);
		btn->SetText(text);
		btn->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		btn->Resize(w,h);

		return *btn;
	}

	Button &WidgetRegistry::createDialogButton(string text, int w, int h, int x, int y) {
		Button *btn=new Button(button);
		btn->SetText(text);
		btn->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		btn->Resize(w,h);

		return *btn;
	}

	Button &WidgetRegistry::createPictureChooserButton(IWidgetContainer &container, int x, int y, int w, int h) {
		ButtonBP *bp=button;
		if(pictureChooser)
			bp=pictureChooser;

		Button *btn=new Button(bp, container);
		btn->SetText("");
		btn->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		btn->Resize(w,h);

		return *btn;
	}

	Checkbox &WidgetRegistry::createCheckbox(IWidgetContainer &container, string text, int x, int y, int w, int h) {
		Checkbox *chk=new Checkbox(checkbox, container);
		chk->SetText(text);
		chk->Move(x, y);
		if(w==0)
			w=120;
		chk->Resize(w, h);

		return *chk;
	}

	Checkbox &WidgetRegistry::createCheckbox(string text, int w, int h, int x, int y) {
		Checkbox *chk=new Checkbox(checkbox);
		chk->SetText(text);
		chk->Move(x, y);
		if(w==0)
			w=120;
		chk->Resize(w, h);

		return *chk;
	}

	RadioButton &WidgetRegistry::createRadioButton(IWidgetContainer &container, string text, int x, int y, int w, int h) {
		RadioButton *chk=new RadioButton(radioButton, container);
		chk->SetText(text);
		chk->Move(x, y);
		if(w==0)
			w=120;
		chk->Resize(w, h);

		return *chk;
	}

	RadioButton &WidgetRegistry::createRadioButton(string text, int w, int h, int x, int y) {
		RadioButton *chk=new RadioButton(radioButton);
		chk->SetText(text);
		chk->Move(x, y);
		if(w==0)
			w=120;
		chk->Resize(w, h);

		return *chk;
	}

	RadioButton &WidgetRegistry::createTabButton(IWidgetContainer &container, string text, int x, int y, int w, int h) {
		CheckboxBP *bp=tabButton;
		if(tabButton)
			bp=tabButton;

		RadioButton *chk=new RadioButton(bp, container);
		chk->SetText(text);
		chk->Move(x, y);
		chk->Resize(w, h);

		return *chk;
	}

	Textbox &WidgetRegistry::createTextbox(IWidgetContainer &container, string text, int x, int y, int w, int h) {
		Textbox *txt=new Textbox(textbox, container);
		txt->SetText(text);
		txt->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		txt->Resize(w,h);


		return *txt;
	}

	Textbox &WidgetRegistry::createTextbox(string text, int w, int h, int x, int y) {
		Textbox *txt=new Textbox(textbox);
		txt->SetText(text);
		txt->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		txt->Resize(w,h);


		return *txt;
	}

	Scrollbar &WidgetRegistry::createVScrollbar(IWidgetContainer &container, int w, int h, int max) {
		Scrollbar *sc=new Scrollbar(scroller, container, SA_Verticle);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		sc->setMaximum(max);
		sc->Resize(w,h);


		return *sc;
	}
	
	Spinner &WidgetRegistry::createNumberSpinner(IWidgetContainer &container, int x, int y, float min, float max, float steps, float value, int w, int h) {
		Textbox &txt=createNumberbox(container, value, x,y, w,h);
		return createSpinner(container, txt, min, max, steps, value);
	}
	
	Spinner &WidgetRegistry::createNumberSpinner(float min, float max, float steps, float value, int w, int h, int x, int y) {
		Textbox &txt=createNumberbox(value, w,h);
		return createSpinner(txt, min, max, steps, value);
	}

	Progressbar &WidgetRegistry::createProgressbar(IWidgetContainer &container, float min, float max, int w, int h) {
		Progressbar *pb=new Progressbar(progressor, container);

		pb->SetRange(min, max);
		pb->Resize(w,h);

		return *pb;
	}

	Progressbar &WidgetRegistry::createProgressbar(float min, float max, int w, int h) {
		Progressbar *pb=new Progressbar(progressor);

		pb->SetRange(min, max);
		pb->Resize(w,h);

		return *pb;
	}

	Textbox &WidgetRegistry::createNumberbox(IWidgetContainer &container, float value, int x, int y, int w, int h) {
		TextboxBP *bp=textbox;
		if(numberbox)
			bp=numberbox;

		Textbox *txt=new Textbox(bp, container);
		char text[30];
		sprintf_s<30>(text, "%.2f", value);
		txt->SetText(text);
		txt->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		txt->Resize(w,h);


		return *txt;
	}

	Textbox &WidgetRegistry::createNumberbox(float value, int w, int h, int x, int y) {
		TextboxBP *bp=textbox;
		if(numberbox)
			bp=numberbox;

		Textbox *txt=new Textbox(bp);
		char text[30];
		sprintf_s<30>(text, "%.2f", value);
		txt->SetText(text);
		txt->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		txt->Resize(w,h);


		return *txt;
	}

	ResizableRect &WidgetRegistry::createThinFrame() {
		return  *(new ResizableRect(thinFrame));
	}

	ResizableRect &WidgetRegistry::createThickFrame() {
		return  *(new ResizableRect(thickFrame));
	}

	ResizableRect &WidgetRegistry::createPictureFrame() {
		return  *(new ResizableRect(pictureFrame));
	}

	ResizableRect &WidgetRegistry::createPhotoFrame() {
		return  *(new ResizableRect(photoFrame));
	}

	ResizableRect &WidgetRegistry::createSelectionFrame() {
		return  *(new ResizableRect(selectionFrame));
	}

	ResizableRect &WidgetRegistry::createDialogFrame() {
		return  *(new ResizableRect(dialogFrame));
	}
	ResizableRect &WidgetRegistry::createControlContainer() {
		return  *(new ResizableRect(controlContainer));
	}

	ResizableRect &WidgetRegistry::createModalFrame() {
		return  *(new ResizableRect(modalFrame));
	}

	Label &WidgetRegistry::createFieldLabel(IWidgetContainer &container, string text, int x, int y, int w) {
		return *(new Label(container, normal, text, x,y,true,w,TEXTALIGN_LEFT,textColor));
	}

	Label &WidgetRegistry::createListLabel(IWidgetContainer &container, string text, int x, int y, int w) {
		return *(new Label(container, normal, text, x,y,true,w,TEXTALIGN_LEFT,textColor));
	}

	Label &WidgetRegistry::createHintLabel(IWidgetContainer &container, string text, int x, int y, int w) {
		return *(new Label(container, normal, text, x,y,true,w,TEXTALIGN_LEFT,hintColor));
	}

	Label &WidgetRegistry::createDialogLabel(IWidgetContainer &container, string text, int x, int y, int w) {
		return *(new Label(container, normal, text, x,y,false,w,TEXTALIGN_LEFT,textColor));
	}

	Label &WidgetRegistry::createTitleLabel(IWidgetContainer &container, string text, int x, int y, int w) {
		return *(new Label(container, h3, text, x,y,true,w,TEXTALIGN_LEFT,titleColor));
	}

	Label &WidgetRegistry::createBoldLabel(IWidgetContainer &container, string text, int x, int y, int w) {
		return *(new Label(container, bold, text, x,y,true,w,TEXTALIGN_LEFT,titleColor));
	}

	Label &WidgetRegistry::createHeadingLabel(IWidgetContainer &container, string text, int x, int y, int w) {
		return *(new Label(container, h1, text, x,y,true,w,TEXTALIGN_LEFT,titleColor));
	}

	Label &WidgetRegistry::createSubHeadingLabel(IWidgetContainer &container, string text, int x, int y, int w) {
		return *(new Label(container, h2, text, x,y,true,w,TEXTALIGN_LEFT,titleColor));
	}

	Label &WidgetRegistry::createFieldLabel(string text, int w, int x, int y) {
		return *(new Label(normal, text, x,y,true,w,TEXTALIGN_LEFT,textColor));
	}

	Label &WidgetRegistry::createListLabel(string text, int w, int x, int y) {
		return *(new Label(normal, text, x,y,true,w,TEXTALIGN_LEFT,textColor));
	}

	Label &WidgetRegistry::createHintLabel(string text, int w, int x, int y) {
		return *(new Label(normal, text, x,y,true,w,TEXTALIGN_LEFT,hintColor));
	}

	Label &WidgetRegistry::createDialogLabel(string text, int w, int x, int y) {
		return *(new Label(normal, text, x,y,false,w,TEXTALIGN_LEFT,textColor));
	}

	Label &WidgetRegistry::createTitleLabel(string text, int w, int x, int y) {
		return *(new Label(h3, text, x,y,true,w,TEXTALIGN_LEFT,titleColor));
	}

	Label &WidgetRegistry::createBoldLabel(string text, int w, int x, int y) {
		return *(new Label(bold, text, x,y,true,w,TEXTALIGN_LEFT,titleColor));
	}

	Label &WidgetRegistry::createHeadingLabel(string text, int w, int x, int y) {
		return *(new Label(h1, text, x,y,true,w,TEXTALIGN_LEFT,titleColor));
	}

	Label &WidgetRegistry::createSubHeadingLabel(string text, int w, int x, int y) {
		return *(new Label(h2, text, x,y,true,w,TEXTALIGN_LEFT,titleColor));
	}

	Line &WidgetRegistry::createVSeparator() {
		return *(new Line(vSeparator));
	}
	Line &WidgetRegistry::createHSeparator() {
		return *(new Line(hSeparator));
	}
	
	Slider &WidgetRegistry::createSlider(IWidgetContainer &container, SliderStyles orientation, float min, float max, float steps, float value, int x, int y, int w, int h) {
		Slider *slider=new Slider(this->slider, container, orientation);
		slider->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		slider->Resize(w, h);
		slider->SetRange(min, max, steps);
		slider->setValue(value);
		return *slider;
	}
	
	NamedSlider &WidgetRegistry::createNamedSlider(IWidgetContainer &container, NamedSliderDirections orientation, float min, float max, float value, int x, int y, int w, int h) {
		NamedSlider *slider=new NamedSlider(this->slider, container, orientation);
		slider->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		slider->Resize(w, h);
		slider->SetRange(min, max);

		slider->setValue(value);
		slider->showIndicators();
		return *slider;
	}

	Selectbar &WidgetRegistry::createSelectbar(IWidgetContainer &container, SelectbarOrientations orientation, int x, int y, int w, int h) {
		Selectbar *sb=new Selectbar(selectbar, container, orientation);
		sb->Move(x, y);
		if(w==-1)
			w=0;
		if(h==-1)
			h=0;
		sb->Resize(w, h);
		return *sb;
	}

	Spinner &WidgetRegistry::createSpinner(IWidgetContainer &container, Textbox &txt, float min, float max, float steps, float value) {
		Spinner &spn=createSpinner(container, min,max, steps, value);
		spn.AttachTo(txt);

		return spn;
	}

	Spinner &WidgetRegistry::createSpinner(IWidgetContainer &container, float min, float max, float steps, float value) {
		Spinner *spn;
		spn=new Spinner(scroller, container);
		spn->SetRange(min, max, steps);
		if(steps<1 && steps>0) {
			int i;
			for(i=1;;i++) 
				if(steps>=pow(10.0f, -i))
					break;
			spn->setDecimalPoints(i);
		}

		spn->setValue(value);
		return *spn;
	}

	Spinner &WidgetRegistry::createSpinner(Textbox &txt, float min, float max, float steps, float value) {
		Spinner &spn=createSpinner(min,max, steps, value);
		spn.AttachTo(txt);

		return spn;
	}

	Spinner &WidgetRegistry::createSpinner(float min, float max, float steps, float value) {
		Spinner *spn;
		spn=new Spinner(scroller);
		spn->SetRange(min, max, steps);
		if(steps<1 && steps>0) {
			int i;
			for(i=1;;i++) 
				if(steps>=pow(10.0f, -i))
					break;
			spn->setDecimalPoints(i);
		}

		spn->setValue(value);
		return *spn;
	}

	FrameBP &WidgetRegistry::DialogFrameBP(bool hasScroll) {
		if(hasScroll)
			return *(new FrameBP(dialogFrame, scroller));
		else
			return *(new FrameBP(dialogFrame));
	}

	FrameBP &WidgetRegistry::PictureFrameBP(bool hasScroll) {
	if(hasScroll)
		return *(new FrameBP(pictureFrame, scroller));
	else
		return *(new FrameBP(pictureFrame));
}

	Frame &WidgetRegistry::createDialogFrame(int w, int h, int x, int y, ScrollbarDisplayStates scroll) {
		Frame *frame=new Frame(*DialogBP, x, y, w, h);
		frame->setVerticleScrollbarState(scroll);
		Widgets.Add(frame);

		return *frame;
	}
	LinearOrganizer &WidgetRegistry::createDialogLinearOrganizer(int w, int h, int x, int y, ScrollbarDisplayStates scroll) {
		LinearOrganizer *frame=new LinearOrganizer(*DialogBP, x, y, w, h);
		frame->setVerticleScrollbarState(scroll);
		Widgets.Add(frame);

		return *frame;
	}

	
	class WidgetRegistry WR;
} }
