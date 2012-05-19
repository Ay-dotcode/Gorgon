#pragma once


#include "../../Resource/Base.h"
#include "../Definitions.h"
#include "../../Resource/ResizableObject.h"
#include "../../Resource/File.h"
#include "../../Resource/NullImage.h"
#include "../../Engine/Animation.h"



namespace gge { namespace widgets {

	class ResizableObjectResource;

	ResizableObjectResource *LoadResizableObjectResource(resource::File& File, std::istream &Data, int Size);

	class ResizableObject : virtual public resource::ResizableObject {
	public:

		ResizableObject(resource::ResizableObject &object, graphics::SizeController2D SizeController) : object(&object), SizeController(SizeController) {
		}

		resource::ResizableObject &GetObject() { return *object; }
		void SetObject(resource::ResizableObject &object) { this->object	=&object; }

		graphics::SizeController2D SizeController;

		virtual	~ResizableObject() {
			if(object)
				object->DeleteAnimation();
		}

		virtual void SetController( animation::Timer &controller, bool owner=false ) {
			Base::SetController(controller, owner);
			object->SetController(controller);
		}

		virtual void DeleteAnimation() {
			object->DeleteAnimation();
			object=NULL;
			delete this;
		}

	protected:
		resource::ResizableObject *object;
		virtual animation::ProgressResult::Type Progress() 
		{ return animation::ProgressResult::None; }

		virtual void drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const 
		{ object->DrawIn(Target, SizeController, X, Y, W, H); }

		virtual void drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) const
		{ object->DrawIn(Target, controller, X, Y, W, H); }

		virtual int calculatewidth (int w=-1) const
		{ return object->CalculateWidth(SizeController, w); }

		virtual int calculateheight(int h=-1) const
		{ return object->CalculateHeight(SizeController, h); }

		virtual int calculatewidth (const graphics::SizeController2D &controller, int w=-1) const
		{ return object->CalculateWidth(controller, w); }

		virtual int calculateheight(const graphics::SizeController2D &controller, int h=-1) const
		{ return object->CalculateHeight(controller, h); }

	};

	class ResizableObjectResource : public resource::Base, virtual public resource::ResizableObjectProvider {
		friend ResizableObjectResource *LoadResizableObjectResource(resource::File& File, std::istream &Data, int Size);
	public:

		virtual GID::Type GetGID() const { return GID::ResizableObj; }

		ResizableObjectResource(resource::ResizableObjectProvider &object, graphics::SizeController2D controller) : object(&object), SizeController(controller) {
		}


		virtual ResizableObject &CreateResizableObject(animation::Timer &controller, bool owner=false)
		{ return *new ResizableObject(object->CreateResizableObject(controller, owner),SizeController); }

		virtual ResizableObject &CreateResizableObject(bool create=false)
		{ return *new ResizableObject(object->CreateResizableObject(create),SizeController); }


		virtual ResizableObject &CreateAnimation(animation::Timer &controller, bool owner=false) 
		{ return CreateResizableObject(controller, owner); }

		virtual ResizableObject &CreateAnimation(bool create=false)
		{ return CreateResizableObject(create); }


		void SetProvider(resource::ResizableObjectProvider &object) 
		{ this->object=&object; }

		ResizableObjectProvider &GetProvider() 
		{ return *this->object; }

		enum ControllerFillModes {
			Smaller = B8(00001000),
			Fill	= B8(00010000),
			Best	= B8(00100000)
		};

		graphics::SizeController2D ControllerWithFillMode(ControllerFillModes h, ControllerFillModes v) {
			graphics::SizeController2D controller=SizeController;

			controller.HorizontalTiling=(graphics::SizeController2D::TilingType)((controller.HorizontalTiling & B8(11000111))|h);
			controller.VerticalTiling  =(graphics::SizeController2D::TilingType)((controller.VerticalTiling   & B8(11000111))|v);
			
			return controller;
		}



		graphics::SizeController2D SizeController;

		virtual void Prepare(GGEMain &main, resource::File &file);

		virtual ~ResizableObjectResource() {
		}

	protected:
		ResizableObjectResource() : object(NULL) { }

		resource::ResizableObjectProvider *object;
		utils::SGuid target;

	};


}}
