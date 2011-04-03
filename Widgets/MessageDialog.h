#pragma once

#include "IDialog.h"
#include "Frame.h"
#include "Label.h"
#include "Button.h"

namespace gge { namespace widgets {
	////Used to display messages. Dialog boxes should be 
	/// pooled to increase performance. See WidgetRegistry for
	/// pooled message dialogs.
	class MessageDialog : public IDialog {
	public:
		////Creates a new message dialog. Specifying position parameters -1 makes them
		/// automatic, this constructor registers this dialog to first toplevel container
		MessageDialog(string Message="", int Width=-1, int Height=-1, int X=-1, int Y=-1);

		////Creates a new message dialog. Specifying position parameters -1 makes them
		/// automatic
		MessageDialog(IWidgetContainer &container, string Message="", int Width=-1, int Height=-1, int X=-1, int Y=-1);

		virtual void Show();
		virtual void Show(IWidgetContainer &container);
		virtual void Hide();
		virtual void Move(int X, int Y);
		virtual void Resize(int W, int H);
		virtual void SetParent(IWidgetContainer &container);
		virtual bool isVisible() { return frame.isVisible(); }

		virtual void setMessage(string Message);
		virtual void setMessage(int Message);
		virtual void setMessage(float Message);
		virtual void setMessage(double Message);
		
		virtual void setTitle(string Title);

		virtual string getMessage() { return Message; }
		virtual string getTitle() { return Title; }

		void setAlignment(TextAlignment align){ message.Align=align; }
		TextAlignment getAlignment(){ return message.Align; }

		virtual ~MessageDialog();

	protected:
		int X,Y, Width,Height;
		string Message;
		string Title;

		Frame frame;
		Label &title, &message;
		Button &btnOK;

		void btnOK_click	();
		void frame_focus	(Frame &frame);
		
	};

	
	MessageDialog &ShowMessage(string Message, string Title, int Width=-1, int Height=-1, int X=-1, int Y=-1);

	inline MessageDialog &ShowMessage(string Message, int Width=-1, int Height=-1, int X=-1, int Y=-1) {
		return ShowMessage(Message, "", Width,Height, X,Y);
	}

	inline MessageDialog &ShowMessage(double Message, int Width=-1, int Height=-1, int X=-1, int Y=-1) {
		stringstream msg;
		msg<<Message;
		return ShowMessage(msg.str(), "", Width,Height, X,Y);
	}

	inline MessageDialog &ShowMessage(int Message, int Width=-1, int Height=-1, int X=-1, int Y=-1) {
		stringstream msg;
		msg<<Message;
		return ShowMessage(msg.str(), "", Width,Height, X,Y);
	}

} }
