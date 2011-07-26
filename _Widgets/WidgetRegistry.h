#pragma once

#include "WidgetMain.h"
#include "Button.h"
#include "Textbox.h"
#include "Checkbox.h"
#include "RadioGroup.h"
#include "Label.h"
#include "Frame.h"
#include "Slider.h"
#include "NamedSlider.h"
#include "Scrollbar.h"
#include "Slider.h"
#include "Selectbar.h"
#include "Progressbar.h"
#include "Spinner.h"
#include "../Resource/ResourceFile.h"
#include "../Resource/FontTheme.h"
#include "../Engine/Font.h"
#undef small

namespace gge { namespace widgets {

	class LinearOrganizer;

	class WidgetRegistry {
	public:
		ButtonBP *button,
				 *largeButton,
				 *smallButton,
				 *actionButton,
				 *dialogButton,
				 *browseButton,
				 *symbolButton,
				 *toolButton,
				 *navigationButton,
				 *decorativeButton,
				 *menuButton,
				 *pictureChooser,
				 *colorChooser
		;

		TextboxBP	*textbox,
					*numberbox,
					*shortTextbox,
					*signitureTextbox
		;

		CheckboxBP	*checkbox,
					*radioButton,
					*extenderButton,
					*listCheckbox,
					*listRadioButton,
					*menuCheckbox,
					*menuRadioButton,
					*tabButton
		;

		resource::BitmapFontResource *small,
						   *normal,
						   *bold,
						   *italic,
						   *h1,
						   *h2,
						   *h3
		;

		resource::FontTheme *fonttheme;

		LineResource *hSeparator,
					 *vSeparator,
					 *detailsSeperator,
					 *creditsSeperator,
					 *sectionSeperator
		;

		RectangleResource *pictureFrame,
						  *photoFrame,
						  *thickFrame,
						  *thinFrame,
						  *selectionFrame,
						  *dialogFrame,
						  *modalFrame,
						  *controlContainer,
						  *controlPanel

		;

		SliderBP *slider,
				 *dialogScroller,
				 *controlScroller,
				 *pictureScroller,
				 *scroller,
				 *progressor,
				 *fileProgressor,
				 *drawingProgressor,
				 *loadingProgressor,
				 *downloadProgressor,
				 *selectbar,
				 *indicatorbar,
				 *radiobar,
				 *percentbar,
				 *namedslider,
				 *heatbar,
				 *spinner
		;

		FrameBP		*DialogBP;

		resource::ImageResource* background;

		Button &createButton(IWidgetContainer &container, string text="", int x=0, int y=0, int w=-1, int h=-1);
		Button &createButton(string text="", int w=-1, int h=-1, int x=0, int y=0);
		Button &createDialogButton(IWidgetContainer &container, string text="", int x=0, int y=0, int w=-1, int h=-1);
		Button &createDialogButton(string text="", int w=-1, int h=-1, int x=0, int y=0);

		Button &createPictureChooserButton(IWidgetContainer &container, int x=0, int y=0, int w=-1, int h=-1);

		Checkbox &createCheckbox(IWidgetContainer &container, string text="", int x=0, int y=0, int w=0, int h=0);
		Checkbox &createCheckbox(string text="", int w=0, int h=0, int x=0, int y=0);

		RadioButton &createRadioButton(IWidgetContainer &container, string text="", int x=0, int y=0, int w=0, int h=0);
		RadioButton &createRadioButton(string text="", int w=0, int h=0, int x=0, int y=0);

		RadioButton &createTabButton(IWidgetContainer &container, string text="", int x=0, int y=0, int w=0, int h=0);

		Textbox &createTextbox(IWidgetContainer &container, string text="", int x=0, int y=0, int w=-1, int h=-1);
		Textbox &createTextbox(string text="", int w=-1, int h=-1, int x=0, int y=0);

		Textbox &createNumberbox(IWidgetContainer &container, float initial=0, int x=0, int y=0, int w=-1, int h=-1);
		Textbox &createNumberbox(float initial=0, int w=-1, int h=-1, int x=0, int y=0);
		Scrollbar &createVScrollbar(IWidgetContainer &container, int w=-1, int h=-1, int max=100);

		Label &createFieldLabel(IWidgetContainer &container, string text="", int x=0, int y=0, int w=150);
		Label &createListLabel(IWidgetContainer &container, string text="", int x=0, int y=0, int w=150);
		Label &createHintLabel(IWidgetContainer &container, string text="", int x=0, int y=0, int w=150);
		Label &createDialogLabel(IWidgetContainer &container, string text="", int x=0, int y=0, int w=150);
		Label &createTitleLabel(IWidgetContainer &container, string text="", int x=0, int y=0, int w=150);
		Label &createHeadingLabel(IWidgetContainer &container, string text="", int x=0, int y=0, int w=150);
		Label &createSubHeadingLabel(IWidgetContainer &container, string text="", int x=0, int y=0, int w=150);
		Label &createBoldLabel(IWidgetContainer &container, string text="", int x=0, int y=0, int w=150);

		Label &createFieldLabel(string text="", int w=150, int x=0, int y=0);
		Label &createListLabel(string text="", int w=150, int x=0, int y=0);
		Label &createHintLabel(string text="", int w=150, int x=0, int y=0);
		Label &createDialogLabel(string text="", int w=150, int x=0, int y=0);
		Label &createTitleLabel(string text="", int w=150, int x=0, int y=0);
		Label &createHeadingLabel(string text="", int w=150, int x=0, int y=0);
		Label &createSubHeadingLabel(string text="", int w=150, int x=0, int y=0);
		Label &createBoldLabel(string text="", int w=150, int x=0, int y=0);

		Spinner &createNumberSpinner(IWidgetContainer &container, int x=0, int y=0, float min=0, float max=100, float steps=1, float value=0, int w=-1, int h=-1);
		Spinner &createNumberSpinner(float min=0, float max=100, float steps=1, float value=0, int w=-1, int h=-1,int x=0, int y=0);
		Spinner &createSpinner(IWidgetContainer &container, Textbox &txt, float min=0, float max=100, float steps=1, float value=0);
		Spinner &createSpinner(IWidgetContainer &container, float min=0, float max=100, float steps=1, float value=0);
		Spinner &createSpinner(Textbox &txt, float min=0, float max=100, float steps=1, float value=0);
		Spinner &createSpinner(float min=0, float max=100, float steps=1, float value=0);
		Selectbar &createSelectbar(IWidgetContainer &container, SelectbarOrientations orientation=SBO_Bottom, int x=0, int y=0, int w=-1, int h=-1);
		Slider &createSlider(IWidgetContainer &container, SliderStyles orientation=SS_Bottom, float min=0, float max=100, float steps=1, float value=0, int x=0, int y=0, int w=-1, int h=-1);
		NamedSlider &createNamedSlider(IWidgetContainer &container, NamedSliderDirections orientation=NSD_Bottom, float min=0, float max=100, float value=0, int x=0, int y=0, int w=-1, int h=-1);
		Progressbar &createProgressbar(IWidgetContainer &container, float min=0, float max=100, int w=-1, int h=-1);
		Progressbar &createProgressbar(float min=0, float max=100, int w=-1, int h=-1);

		ResizableRect &createPictureFrame();
		ResizableRect &createPhotoFrame();
		ResizableRect &createThinFrame();
		ResizableRect &createThickFrame();
		ResizableRect &createSelectionFrame();
		ResizableRect &createDialogFrame();
		ResizableRect &createModalFrame();
		ResizableRect &createControlContainer();

		Line &createVSeparator();
		Line &createHSeparator();

		void Delete(IWidgetObject &object) {
			Widgets.Delete(object);
		}

		void DiscoverWidgets(resource::File *file);
		void DiscoverWidgets(resource::File &file) { DiscoverWidgets(&file); }

		FrameBP &DialogFrameBP(bool hasScroll=false);
		FrameBP &PictureFrameBP(bool hasScroll=false);

		Frame &createDialogFrame(int w, int h, int x=0, int y=0, ScrollbarDisplayStates scroll=SDS_WhenNeeded);
		Frame &createDialogFrame(int w, int h, ScrollbarDisplayStates scroll=SDS_WhenNeeded) { return createDialogFrame(w, h, 0, 0, scroll); }

		LinearOrganizer &createDialogLinearOrganizer(int w, int h, int x=0, int y=0, ScrollbarDisplayStates scroll=SDS_WhenNeeded);
		LinearOrganizer &createDialogLinearOrganizer(int w, int h, ScrollbarDisplayStates scroll=SDS_WhenNeeded) { return createDialogLinearOrganizer(w, h, 0, 0, scroll); }

		resource::FontTheme &FontTheme() { return *fonttheme; }

		graphics::RGBint titleColor;
		graphics::RGBint textColor;
		graphics::RGBint hintColor;

		utils::Collection<IWidgetObject> Widgets;
		utils::Collection<ResizableRect> Rectangles;


	};

	extern WidgetRegistry WR;
} }
