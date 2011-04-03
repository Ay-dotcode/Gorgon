#include "WidgetMain.h"
#include "Line.h"
#include "Rectangle.h"
#include "../Engine/GGEMain.h"
#include "../Resource/ResourceFile.h"
#include "ButtonBP.h"
#include "ButtonElement.h"
#include "ButtonStyleGroup.h"
#include "TextboxBP.h"
#include "TextboxElement.h"
#include "TextboxStyleGroup.h"
#include "CheckboxBP.h"
#include "CheckboxElement.h"
#include "CheckboxStyleGroup.h"
#include "SliderBP.h"
#include "SliderElement.h"
#include "SliderStyleGroup.h"
#include "FrameBP.h"
#include "ResizableObjectResource.h"

#define DIALOG_LAYER_ZORDER		-10

namespace gge { namespace widgets {
	WidgetLayer *DialogLayer=NULL;

	void InitializeWidgets(GGEMain *Main) {
		DialogLayer=new WidgetLayer(0, 0, Main->getWidth(), Main->getHeight());
		Main->Add(DialogLayer,DIALOG_LAYER_ZORDER);
	}
	void RegisterWidgetLoaders(ResourceFile *File) {
		File->AddExtendedLoaders();
		File->AddGameLoaders();
		File->Loaders.Add(new ResourceLoader(GID_LINE, LoadLineResource)); 
		File->Loaders.Add(new ResourceLoader(GID_RECT, LoadRectangleResource)); 

		File->Loaders.Add(new ResourceLoader(GID_BUTTON, LoadButton)); 
		File->Loaders.Add(new ResourceLoader(GID_BUTTON_GRP, LoadButtonStyleGroup)); 
		File->Loaders.Add(new ResourceLoader(GID_BUTTON_ELEMENT, LoadButtonElement)); 

		File->Loaders.Add(new ResourceLoader(GID_TEXTBOX, LoadTextbox)); 
		File->Loaders.Add(new ResourceLoader(GID_TEXTBOX_GRP, LoadTextboxStyleGroup)); 
		File->Loaders.Add(new ResourceLoader(GID_TEXTBOX_ELEMENT, LoadTextboxElement)); 

		File->Loaders.Add(new ResourceLoader(GID_CHECKBOX, LoadCheckbox)); 
		File->Loaders.Add(new ResourceLoader(GID_CHECKBOX_GRP, LoadCheckboxStyleGroup)); 
		File->Loaders.Add(new ResourceLoader(GID_CHECKBOX_ELEMENT, LoadCheckboxElement)); 

		File->Loaders.Add(new ResourceLoader(GID_SLIDER, LoadSlider)); 
		File->Loaders.Add(new ResourceLoader(GID_SLIDER_GRP, LoadSliderStyleGroup)); 
		File->Loaders.Add(new ResourceLoader(GID_SLIDER_ELEMENT, LoadSliderElement)); 

		File->Loaders.Add(new ResourceLoader(GID_FRAME, LoadFrame)); 

		File->Loaders.Add(new ResourceLoader(GID_RESIZABLEOBJECT, LoadResizableObject)); 
	}
} }
