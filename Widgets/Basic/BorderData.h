#pragma once


#include "..\..\Resource\Base.h"
#include "..\Definitions.h"
#include "..\..\Resource\ResizableObject.h"
#include "..\..\Resource\File.h"
#include "..\..\Resource\NullImage.h"
#include "..\..\Engine\Animation.h"
#include "..\..\Utils\Margins.h"
#include "ResizableObjectResource.h"
#include "Rectangle.h"
#include "Line.h"


namespace gge { namespace widgets {

	class BorderDataResource;

	BorderDataResource *LoadBorderDataResource(resource::File& File, std::istream &Data, int Size);


	//You should use draw and calculate functions without size controller for this object to control its size automatically
	class BorderData : virtual public resource::ResizableObject {
	public:

		BorderData(resource::ResizableObject &object, bool autowidth=true) : object(&object), Margins(0), Padding(0) {
			if(autowidth) {
				if(typeid(object)==typeid(widgets::ResizableObject)) {
					widgets::ResizableObject& ro=dynamic_cast<widgets::ResizableObject&>(object);
					if(dynamic_cast<Rectangle*>(&ro.GetObject())) {
						BorderWidth=dynamic_cast<Rectangle&>(ro.GetObject()).BorderWidth();
					}
					else if(dynamic_cast<Line*>(&ro.GetObject())) {
						BorderWidth=dynamic_cast<Line&>(ro.GetObject()).BorderWidth();
					}
					else
						BorderWidth=utils::Margins(0);	
				}
				else if(dynamic_cast<Rectangle*>(&object)) {
					BorderWidth=dynamic_cast<Rectangle&>(object).BorderWidth();
				}
				else if(dynamic_cast<Line*>(&object)) {
					BorderWidth=dynamic_cast<Line&>(object).BorderWidth();
				}
				else
					BorderWidth=utils::Margins(0);
			}
		}

		resource::ResizableObject &GetObject() { return *object; }
		void SetObject(resource::ResizableObject &object) { this->object	=&object; }

		utils::Bounds ContentBounds(utils::Bounds outer) { return ((outer-BorderWidth)-Padding); }
		utils::Bounds DrawingBound(utils::Bounds outer) { return outer-Margins; }

		void DrawAround(graphics::ImageTarget2D& Target, utils::Bounds bounds) { DrawIn(Target, utils::Rectangle(bounds+BorderWidth+Padding+Margins)); }

		utils::Margins Margins;
		utils::Margins Padding;
		utils::Margins BorderWidth;
		
		virtual ~BorderData() {
			if(object)
				object->DeleteAnimation();
		}

		virtual void DeleteAnimation() {
			object->DeleteAnimation();
			object=NULL;
			delete this;
		}

		virtual void SetController( animation::Timer &controller, bool owner=false ) {
			Base::SetController(controller, owner);
			object->SetController(controller);
		}

	protected:
		resource::ResizableObject *object;
		virtual animation::ProgressResult::Type Progress() 
		{ return animation::ProgressResult::None; }

		virtual void drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const
		{ object->DrawIn(Target, X, Y, W, H); }

		virtual void drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) const
		{ object->DrawIn(Target, controller, X, Y, W, H); }

		virtual int calculatewidth (int w=-1) const
		{ return object->CalculateWidth(w); }

		virtual int calculateheight(int h=-1) const
		{ return object->CalculateHeight(h); }

		virtual int calculatewidth (const graphics::SizeController2D &controller, int w=-1) const
		{ return object->CalculateWidth(controller, w); }

		virtual int calculateheight(const graphics::SizeController2D &controller, int h=-1) const
		{ return object->CalculateHeight(controller, h); }
	};

	class BorderDataResource : public resource::Base, virtual public resource::ResizableObjectProvider {
		friend BorderDataResource *LoadBorderDataResource(resource::File& File, std::istream &Data, int Size);
	public:

		BorderDataResource(ResizableObjectResource &object) : object(&object), Margins(0), Padding(0), BorderWidth(0), AutoBorderWidth(true) 
		{ }

		virtual BorderData &CreateResizableObject(animation::Timer &controller, bool owner=false) { 
			BorderData *bd=new BorderData(object->CreateResizableObject(controller, owner), AutoBorderWidth);
			if(!AutoBorderWidth)
				bd->BorderWidth=BorderWidth;
			bd->Margins=Margins;
			bd->Padding=Padding;

			return *bd; 
		}

		virtual BorderData &CreateResizableObject(bool create=false) { 
			BorderData *bd=new BorderData(object->CreateResizableObject(create), AutoBorderWidth);
			if(!AutoBorderWidth)
				bd->BorderWidth=BorderWidth;
			bd->Margins=Margins;
			bd->Padding=Padding;

			return *bd; 
		}


		virtual BorderData &CreateAnimation(animation::Timer &controller, bool owner=false) 
		{ return CreateResizableObject(controller, owner); }

		virtual BorderData &CreateAnimation(bool create=false)
		{ return CreateResizableObject(create); }


		void SetProvider(ResizableObjectResource &object) 
		{ this->object=&object; }

		ResizableObjectProvider &GetProvider() 
		{ return *this->object; }


		GID::Type GetGID() const { return GID::BorderData; }


		utils::Margins Margins;
		utils::Margins Padding;
		utils::Margins BorderWidth;
		bool AutoBorderWidth;

		virtual void Prepare(GGEMain &main, resource::File &file);

		virtual ~BorderDataResource() {
		}

	protected:
		BorderDataResource() : object(NULL) { }
		ResizableObjectResource *object;

		utils::SGuid target;
	};

}}
