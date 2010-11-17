#include "MessageDialog.h"
#include "WidgetRegistry.h"
#include "VirtualFrame.h"
#include "../Engine/GGEMain.h"

namespace gorgonwidgets {

	extern 	LinkedList<VirtualFrame> toplevels;

	Collection<MessageDialog> MessageDialogs;

	MessageDialog::MessageDialog(std::string Message, int Width, int Height, int X, int Y) :
		Message(Message), Title(), Width(Width), Height(Height), X(X), Y(Y),
		frame(WidgetRegistry.DialogFrameBP()), 
		title(WidgetRegistry.createTitleLabel(frame, "")), message(WidgetRegistry.createDialogLabel(frame, Message)),
		btnOK(WidgetRegistry.createDialogButton(frame, "OK"))
	{	
		frame.Hide();
		toplevels.getFirstItem()->AddDialog(frame);
		frame.AllowDrag=true;
		frame.GotFocusEvent.Register(this, &MessageDialog::frame_focus);
		title.setAutosize(false);
		title.Align=TEXTALIGN_CENTER;

		btnOK.SetDefault();
		btnOK.SetCancel();

		btnOK.ClickEvent.Register(this,&MessageDialog::btnOK_click);
	}

	MessageDialog::MessageDialog(IWidgetContainer &container, std::string Message, int Width, int Height, int X, int Y) :
		Message(Message), Title(), Width(Width), Height(Height), X(X), Y(Y), 
		frame(WidgetRegistry.DialogFrameBP()), 
		title(WidgetRegistry.createTitleLabel(frame, "")), message(WidgetRegistry.createDialogLabel(frame, Message)),
		btnOK(WidgetRegistry.createDialogButton(frame, "OK"))
	{
		frame.Hide();
		container.AddDialog(frame);
		frame.AllowDrag=true;
		title.setAutosize(false);
		title.Align=TEXTALIGN_CENTER;
		frame.GotFocusEvent.Register(this, &MessageDialog::frame_focus);
		

		btnOK.SetDefault();
		btnOK.SetCancel();

		btnOK.ClickEvent.Register(this,&MessageDialog::btnOK_click);
	}

	void MessageDialog::Show() {
		int x,y,w,h;

		if(Width==-1)
			w=220;
		else
			w=Width;

		frame.Resize(w+frame.Overhead().x,frame.Height());

		h=0;
		if(Title!="") {
			title.SetText(Title);
			title.Resize(w, 0);
			h+=title.Height()+10;
			
			title.Show();
		} else
			title.Hide();
			

		if(Height==-1) {
			message.Move(0, h);
			message.Resize(w, 0);
			message.SetText(Message);
			h+=message.Height()+10;
		}

		if((h+btnOK.Height()+frame.Overhead().y)>frame.getContainer().Height())
			h=frame.getContainer().Height()-(btnOK.Height()+frame.Overhead().y);
	
		btnOK.Move((w-btnOK.Width())/2, h);
		h+=btnOK.Height();

		frame.Resize(w+frame.Overhead().x,h+frame.Overhead().y);
		
		if(X==-1)
			x=(frame.getContainer().Width()-frame.Width())/2;
		else
			x=X;
		
		if(Y==-1)
			y=(frame.getContainer().Height()-frame.Height())/2;
		else
			y=Y;


		frame.Move(x,y);

		frame.Show();
	}

	void MessageDialog::Show(IWidgetContainer &container) {
		SetParent(container);
		Show();
	}
	void MessageDialog::Hide() {
		frame.Hide();
	}
	void MessageDialog::Move(int X, int Y) { 
		this->X=X;
		this->Y=Y;

		if(X!=-1 && Y!=-1)
			frame.Move(X,Y);
	}
	void MessageDialog::Resize(int W, int H) {
		Width=W;
		Height=H;
	}
	void MessageDialog::SetParent(IWidgetContainer &container) {
		frame.SetContainer(container);
	}

	void MessageDialog::setMessage(string Message) { 
		this->Message=Message;
	}
	void MessageDialog::setMessage(int Message) { 
		stringstream ss;
		ss<<Message;
		this->Message=ss.str();
	}
	void MessageDialog::setMessage(float Message) {
		stringstream ss;
		ss<<Message;
		this->Message=ss.str();
	}
	void MessageDialog::setMessage(double Message) {
		stringstream ss;
		ss<<Message;
		this->Message=ss.str();
	}
	
	void MessageDialog::setTitle(string Title) {
		this->Title=Title;
	}

	MessageDialog::~MessageDialog() {
		WidgetRegistry.Delete(message);
		WidgetRegistry.Delete(title);
		WidgetRegistry.Delete(btnOK);
	}

	void MessageDialog::btnOK_click	(empty_event_params p, Button &button, Any data, string event) {
		Hide();
	}

	void MessageDialog::frame_focus	(empty_event_params p, Frame &frame, Any data, string event) {
		frame.ZOrder();
	}
	MessageDialog &ShowMessage(string Message, string Title, int Width,int Height, int X,int Y) {
		foreach(MessageDialog, dialog, MessageDialogs) {
			if(!dialog->isVisible()) {
				dialog->setMessage(Message);
				dialog->setTitle(Title);
				dialog->Move(X,Y);
				dialog->Resize(Width,Height);
				dialog->setAlignment(TEXTALIGN_LEFT);
				dialog->Show();

				return *dialog;
			}
		}

		MessageDialog *dialog=new MessageDialog(Message, X,Y, Width,Height);
		dialog->setTitle(Title);
		MessageDialogs.Add(dialog);
		dialog->Show();

		return *dialog;
	}

}