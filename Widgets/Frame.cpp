#include "Frame.h"
#include "Scrollbar.h"
#include "WidgetRegistry.h"

#ifndef FRAME_SCROLL
#define FRAME_SCROLL	3
#endif

namespace gorgonwidgets {
	bool frame_scroll(int amount, MouseEventType event, int x, int y, void *data) {
		Frame* frame=(Frame*)data;
		if(frame->verticlescrollbar==NULL)
			return false;

		frame->verticlescrollbar->ScrollUp(-amount*FRAME_SCROLL);

		return true;
	}

	
	Frame::Frame(FrameBP &bp, int X, int Y, int W, int H, bool Dialog) :
		IWidgetContainer(X, Y, W, H, 0), IWidgetObject(),
		KeyboardEvent("Keyboard", this),
		KeypreviewEvent("Keypreview", this),
		GotFocusEvent("GotFocus", this),
		LostFocusEvent("LostFocus", this),
		verticlescrollbar(NULL),
		verticlescrollbarstate(SDS_WhenNeeded),
		noactivate(false),
		outerborderwidth(bp.OuterBorderWidth),
		scrollbarmargin(bp.ScrollbarMargin),
		innerbordermargin(bp.InnerBorderMargin),
		innerborderwidth(bp.InnerBorderWidth),
		scrollingbordermargin(bp.ScrollingBorderMargin),
		scrollingborderwidth(bp.ScrollingBorderWidth),
		contentmargin(bp.ContentMargin),
		maxvscroll(0),
		BoxLayer(
			bp.ContentMargin.Left,
			bp.ContentMargin.Top, 
			W-bp.ContentMargin.TotalX(),
			H-bp.ContentMargin.TotalY()
		),
		ScrollingLayer(0,0,W,H),
		AllowDrag(false), verticlescrollbardisplayed(false)
	{
		
		IWidgetObject::Move(X,Y);
		IWidgetObject::Resize(W,H);

#define p_checkandassign(obj, source)	if(bp.source) { obj=new ResizableRect(bp.source); } else { obj=NULL; }
		p_checkandassign(normal, Normal);
		p_checkandassign(active, Active);
		p_checkandassign(innernormal, InnerNormal);
		p_checkandassign(inneractive, InnerActive);
		p_checkandassign(scrollingnormal, ScrollingNormal);
		p_checkandassign(scrollingactive, ScrollingActive);

		if(bp.AutoBorderWidth) {
			if(bp.Normal)
				outerborderwidth=bp.Normal->getBorderWidth(); 
			else
				outerborderwidth=Margins(0);

			if(bp.InnerNormal)
				innerborderwidth=bp.InnerNormal->getBorderWidth(); 
			else
				innerborderwidth=Margins(0);

			if(bp.ScrollingNormal)
				scrollingborderwidth=bp.ScrollingNormal->getBorderWidth(); 
			else
				scrollingborderwidth=Margins(0);
		}
#undef p_checkandassign


		if(bp.Scroller) {
			this->SetVerticleScrollbar(new Scrollbar(bp.Scroller, ScrollbarAlignments::SA_Verticle));
		}

		init();
	}

	void Frame::init()
	{
		layer.Add(BoxLayer);

		BoxLayer.EnableClipping=true;
		BoxLayer.Add(ScrollingLayer);
		ScrollingLayer.X=0;
		ScrollingLayer.Y=0;

		ScrollingLayer.Add(BaseLayer);
		BaseLayer.X=0;
		BaseLayer.Y=0;
		BaseLayer.EnableClipping=true;

		MouseEventObject *mitem=mouseevent->Item;
		mouseevent->Item->vscroll=frame_scroll;

		IWidgetObject::GotFocusEvent=GotFocusEvent;
		IWidgetObject::LostFocusEvent=LostFocusEvent;

		adjustlocations();
	}

	void Frame::Draw() {
		layer.Clear();

		Margins border(0);

		if(!IWidgetContainer::isVisible()) return;

#define p_drawit(what) what->DrawResized(layer, border.Left, border.Top, layer.W-border.TotalX(), layer.H-border.TotalY(), ALIGN_MIDDLE_CENTER)

		static ResizableRect r(WidgetRegistry.createSelectionFrame());
		

		if(isFocussed() && active) {
			p_drawit(active);
		} else if(normal) {
			p_drawit(normal);
		}


		if(normal)
			border+=outerborderwidth;
		if(verticlescrollbardisplayed) {
			border.Right+=scrollbarmargin.TotalX()+verticlescrollbar->GetWidgetObject()->Width();
		}
		
		if(innernormal) {
			if(verticlescrollbardisplayed)
				border.Right-=innerbordermargin.Right;
			
			border+=innerbordermargin;
		}

		if(isFocussed() && inneractive) {
			p_drawit(inneractive);
		} else if(innernormal) {
			p_drawit(innernormal);
		}

		if(isFocussed() && scrollingactive) {
			scrollingactive->DrawResized(BoxLayer, ScrollingLayer.X, ScrollingLayer.Y, ScrollingLayer.W, ScrollingLayer.H, ALIGN_MIDDLE_CENTER);
		} else if(scrollingnormal) {
			scrollingnormal->DrawResized(BoxLayer, ScrollingLayer.X, ScrollingLayer.Y, ScrollingLayer.W, ScrollingLayer.H, ALIGN_MIDDLE_CENTER);
		}

#undef p_drawit

		//r.DrawResized(layer, verticlescrollbar->GetWidgetObject()->X(),verticlescrollbar->GetWidgetObject()->Y(), 16,verticlescrollbar->GetWidgetObject()->Height(),Alignment::ALIGN_CENTER);
		//r.DrawResized(layer, verticlescrollbar->GetWidgetObject()->X(),verticlescrollbar->GetWidgetObject()->Y(), 16,verticlescrollbar->GetWidgetObject()->Height(),Alignment::ALIGN_CENTER);

		LinkedListOrderedIterator<IWidgetObject> it=Subobjects;
		IWidgetObject *item;

		while(item=it) {
			if(item->isVisible())
				item->Draw();
		}

		if(verticlescrollbar) verticlescrollbar->GetWidgetObject()->Draw();
	}

	void Frame::SetVerticleScrollbar(gorgonwidgets::IScroller &scrollbar) {
		if(verticlescrollbar) delete verticlescrollbar;
		verticlescrollbar=&scrollbar;

		layer.Add(scrollbar.GetWidgetObject()->getLayer(), 0);
		
		verticlescrollbar->ChangeEvent.Register(this, &Frame::frame_verticlescroll);

		adjustlocations();
	}

	void Frame::ScrollIntoView(IWidgetObject *Object) {
		if(Object->Y()+layer.Y<0) {
			if(verticlescrollbar)
				verticlescrollbar->setValue(Object->Y());
			else
				BaseLayer.Y=-Object->Y();
		} else if(Object->Y()+Object->Height()+BaseLayer.Y>BoxLayer.H) {
			if(verticlescrollbar)
				verticlescrollbar->setValue((Object->Y()+Object->Height())-BoxLayer.H);
			else
				BaseLayer.Y=-((Object->Y()+Object->Height())-BoxLayer.H);
		}
	}

	void Frame::adjustlocations() {
		BoxLayer.X=0;
		BoxLayer.Y=0;
		BoxLayer.W=layer.W;
		BoxLayer.H=layer.H;

		if(normal) {
			BoxLayer.X+=outerborderwidth.Left;
			BoxLayer.Y+=outerborderwidth.Top;
			BoxLayer.W-=outerborderwidth.TotalX();
			BoxLayer.H-=outerborderwidth.TotalY();
		}

		if(innernormal) {
			BoxLayer.X+=innerborderwidth.Left+innerbordermargin.Left;
			BoxLayer.Y+=innerborderwidth.Top+innerbordermargin.Top;
			BoxLayer.W-=innerborderwidth.TotalX()+innerbordermargin.TotalX();
			BoxLayer.H-=innerborderwidth.TotalY()+innerbordermargin.TotalY();
		}

		if(scrollingnormal) {
			BoxLayer.X	+= scrollingbordermargin.Left;
			BoxLayer.Y	+= scrollingbordermargin.Top;
			BoxLayer.W	-= scrollingbordermargin.TotalX();
			BoxLayer.H	-= scrollingbordermargin.TotalY();
		}

		ScrollingLayer.W	= BoxLayer.W;
		BaseLayer.W			= ScrollingLayer.W;

		//WidgetRegistry.createSelectionFrame().DrawResized(BoxLayer, 0,0,500,500,Alignment::ALIGN_CENTER);
		//WidgetRegistry.createSelectionFrame().DrawResized(BoxLayer, 0,0,500,500,Alignment::ALIGN_CENTER);
		//WidgetRegistry.createDialogFrame().DrawResized(ScrollingLayer, 0,0,ScrollingLayer.W,ScrollingLayer.H,Alignment::ALIGN_CENTER);


		int maxy=0;//BoxLayer.H-((scrollingnormal ? scrollingborderwidth.TotalY() : 0)+contentmargin.TotalY());

		foreach(IWidgetObject, object, Subobjects) {
			if(object->Y()+object->Height()>maxy)
				maxy=object->Y()+object->Height();
		}

		BaseLayer.H			= maxy;
		ScrollingLayer.H	= BaseLayer.H;

		if(scrollingnormal) {
			BaseLayer.Y		 =scrollingborderwidth.Top+contentmargin.Top;
			ScrollingLayer.H+=scrollingborderwidth.TotalY()+contentmargin.TotalY();
		} else {
			BaseLayer.Y		 =contentmargin.Top;
			ScrollingLayer.H+=contentmargin.TotalY();
		}

		if(verticlescrollbar) {
			bool display=false;
			switch(verticlescrollbarstate) {
				case SDS_Always:
					display=true;
					break;
				case SDS_WhenNeeded:
					if(ScrollingLayer.H>BoxLayer.H)
						display=true;
					break;
			}

			verticlescrollbardisplayed=display;
			if(display) {
				IWidgetObject *obj=verticlescrollbar->GetWidgetObject();
				obj->Show();


				ScrollingLayer.W=BoxLayer.W=BoxLayer.W-(obj->Width()+scrollbarmargin.TotalX())+(innernormal ? innerbordermargin.Right : 0);

				int ox=BoxLayer.X+BoxLayer.W+scrollbarmargin.Left;
				int oy=BoxLayer.Y;
				int oh=BoxLayer.H;

				if(innernormal) {
					oh+=innerborderwidth.TotalY()+contentmargin.TotalY();
					oy-=innerborderwidth.Top+contentmargin.Top;
				}
				if(scrollingnormal) {
					oh+=scrollingbordermargin.TotalY();
					oy-=scrollingbordermargin.Top;
					ox+=scrollingbordermargin.Right;
				}

		
				obj->Move(ox, oy);
				obj->Resize(obj->Width(), oh);

				maxvscroll=ScrollingLayer.H-BoxLayer.H;
				verticlescrollbar->setMaximum(maxvscroll);
				verticlescrollbar->setLargeChange((BoxLayer.H*2)/3);
			} else {
				verticlescrollbar->GetWidgetObject()->Hide();
				verticlescrollbar->setMaximum(0);
			}
		}

		if(scrollingnormal) {
			BaseLayer.X		 =scrollingborderwidth.Left+contentmargin.Left;
			BaseLayer.W		 =ScrollingLayer.W - (scrollingborderwidth.TotalX()+contentmargin.TotalX());
		} else {
			BaseLayer.X		 =contentmargin.Left;
			BaseLayer.W		 =ScrollingLayer.W-contentmargin.TotalX();
		}
		
		//code must reach here
		IWidgetContainer::Resize(BaseLayer.W, BaseLayer.H);
	}


	bool Frame::mouse_event(MouseEventType event,int x,int y) {
		if(event&MOUSE_EVENT_DOWN)
			IWidgetObject::SetFocus();

		static int prevx=0,prevy=0;
		static bool mdown=false;

		if(AllowDrag) {
			if(event==MOUSE_EVENT_LDOWN) {
				prevx=x;
				prevy=y;

				mdown=true;
			}

			if(event==MOUSE_EVENT_LUP) {
				mdown=false;
			}

			if(event==MOUSE_EVENT_MOVE && mdown) {
				Move(this->x+x-prevx, this->y+y-prevy);
				/*prevx=x;
				prevy=y;*/
			}
		}

		return true;
	}

	ResourceBase *LoadFrame(ResourceFile* file,FILE* gfile,int sz) {
		FrameBP *frame =new FrameBP();
		ResourceBase *obj;

		int gid,size,cnt=0;

		int tmpint;

		frame->file=file;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				frame->guid=new Guid(gfile);
				break;
			case GID_FRAME_PROPS:
				fread(&frame->PointerType,1,4,gfile);
				fread(&tmpint,1,4,gfile);
				frame->AutoBorderWidth=(bool)tmpint;
				fread(&frame->OuterBorderWidth,4,4,gfile);
				fread(&frame->ScrollbarMargin,4,4,gfile);
				fread(&frame->InnerBorderMargin,4,4,gfile);
				fread(&frame->InnerBorderWidth,4,4,gfile);
				fread(&frame->ScrollingBorderMargin,4,4,gfile);
				fread(&frame->ScrollingBorderWidth,4,4,gfile);
				fread(&frame->ContentMargin,4,4,gfile);

				frame->guid_normal=new Guid(gfile);
				frame->guid_active=new Guid(gfile);
				frame->guid_innernormal=new Guid(gfile);
				frame->guid_inneractive=new Guid(gfile);
				frame->guid_scrollingnormal=new Guid(gfile);
				frame->guid_scrollingactive=new Guid(gfile);
				frame->guid_scroller=new Guid(gfile);

				EatChunk(gfile,size-(4*2+16*7 + 16*7));
				break;
			default:
				obj=file->LoadObject(gfile,gid,size);
				if(obj)
					frame->Subitems.Add(obj);
				break;
			}
		}

		return frame;
	}

	void FrameBP::Prepare(GGEMain *main) {
		ResourceBase::Prepare(main);

		Normal			=dynamic_cast<RectangleResource*>(file->FindObject(guid_normal));
		Active			=dynamic_cast<RectangleResource*>(file->FindObject(guid_active));
		InnerNormal		=dynamic_cast<RectangleResource*>(file->FindObject(guid_innernormal));
		InnerActive		=dynamic_cast<RectangleResource*>(file->FindObject(guid_inneractive));
		ScrollingNormal	=dynamic_cast<RectangleResource*>(file->FindObject(guid_scrollingnormal));
		ScrollingActive	=dynamic_cast<RectangleResource*>(file->FindObject(guid_scrollingactive));
		Scroller		=dynamic_cast<SliderBP*		    >(file->FindObject(guid_scroller));
	}

}
