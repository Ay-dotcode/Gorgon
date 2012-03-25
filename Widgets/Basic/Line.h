#pragma once

//Line is a primitive resizable object. It can either be
// horizontal or vertical. It is composed of 3 parts
// and center part can either be tiled or scaled.
// 

#include "..\..\Resource\Base.h"
#include "..\Definitions.h"
#include "..\..\Resource\ResizableObject.h"
#include "..\..\Resource\File.h"
#include "..\..\Resource\NullImage.h"
#include "..\..\Engine\Animation.h"
#include "..\..\Utils\Margins.h"



namespace gge { namespace widgets {

	class LineResource;

	LineResource *LoadLineResource(resource::File& File, std::istream &Data, int Size);

	class Line : public resource::ResizableObject {
	public:
		Line(LineResource &parent, animation::Timer &controller, bool owner=false);
		Line(LineResource &parent, bool create=false);

		LineResource &parent;

		utils::Margins BorderWidth();

		virtual ~Line() {
			start->DeleteAnimation();
			loop->DeleteAnimation();
			end->DeleteAnimation();
		}

		virtual void SetController( animation::Timer &controller, bool owner=false ) {
			Base::SetController(controller, owner);
			start->SetController(controller);
			loop->SetController(controller);
			end->SetController(controller);
		}

	protected:
		virtual animation::ProgressResult::Type Progress();

		virtual void drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const; 

		virtual void drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) const;

		virtual int calculatewidth (int w=-1) const;
		virtual int calculateheight(int h=-1) const;

		virtual int calculatewidth (const graphics::SizeController2D &controller, int w=-1) const;
		virtual int calculateheight(const graphics::SizeController2D &controller, int h=-1) const;

		animation::RectangularGraphic2DAnimation *start, *loop, *end;
	};


	class MaskedLine : public Line {
	public:
		MaskedLine(LineResource &parent, animation::Timer &controller, LineResource *mask, bool owner=false);
		MaskedLine(LineResource &parent, LineResource *mask, bool create=false);

		virtual ~MaskedLine() {
			delete Mask;
		}

	protected:
		virtual void drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const; 

		virtual void drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) const;

		Line *Mask;
	};


	class LineResource : public resource::Base, virtual public resource::ResizableObjectProvider, 
		virtual public animation::DiscreteProvider 
	{
		friend LineResource *LoadLineResource(resource::File& File, std::istream &Data, int Size);
	public:

		enum OrientationType {
			Horizontal=0,
			Vertical
		};

		LineResource(animation::RectangularGraphic2DSequenceProvider &start ,animation::RectangularGraphic2DSequenceProvider &loop, 
			animation::RectangularGraphic2DSequenceProvider &end, OrientationType Orientation=Horizontal, bool IsLoopTiled=true) : 
			Orientation(Orientation), IsLoopTiled(IsLoopTiled),
			start(&start), loop(&loop), end(&end), Mask(NULL)
		{ }

		virtual GID::Type GetGID() const { return GID::Line; }

		virtual Line &CreateAnimation(animation::Timer &controller, bool owner=false) { return CreateResizableObject(controller,owner); }
		virtual Line &CreateAnimation(bool create=false) { return CreateResizableObject(create); }

		virtual Line &CreateResizableObject(animation::Timer &controller, bool owner=false) {
			if(Mask==NULL)
				return *new Line(*this, controller,owner); 
			else
				return *new MaskedLine(*this, controller, Mask,owner); 
		}
		virtual Line &CreateResizableObject(bool create=false) { 
			if(Mask==NULL)
				return *new Line(*this, create); 
			else
				return *new MaskedLine(*this, Mask, create); 
		}

		OrientationType Orientation;

		bool IsVertical() { return Orientation==Vertical; }

		bool IsHorizontal() { return Orientation==Horizontal; }

		const animation::RectangularGraphic2DSequenceProvider &GetStart() const { return *start; }
		animation::RectangularGraphic2DSequenceProvider &GetStart() { return *start; }
		void SetStart(animation::RectangularGraphic2DSequenceProvider &val) { start = &val; }

		const animation::RectangularGraphic2DSequenceProvider &GetLoop() const { return *loop; }
		animation::RectangularGraphic2DSequenceProvider &GetLoop() { return *loop; }
		void SetLoop(animation::RectangularGraphic2DSequenceProvider &val) { loop = &val; }

		const animation::RectangularGraphic2DSequenceProvider &GetEnd() const { return *end; }
		animation::RectangularGraphic2DSequenceProvider &GetEnd() { return *end; }
		void SetEnd(animation::RectangularGraphic2DSequenceProvider &val) { end = &val; }

		void SetSources(animation::RectangularGraphic2DSequenceProvider &Start ,animation::RectangularGraphic2DSequenceProvider &Loop, animation::RectangularGraphic2DSequenceProvider &End) {
			start=&Start;
			loop=&Loop;
			end=&End;
		}

		bool IsLoopTiled;

		virtual int GetDuration() const	 { return loop->GetDuration(); }
		virtual int GetDuration(unsigned Frame) const { return loop->GetDuration(Frame); }
		virtual int GetNumberofFrames() const { return loop->GetNumberofFrames(); }

		virtual int		 FrameAt(unsigned Time) const { return loop->FrameAt(Time); }
		virtual int		 StartOf(unsigned Frame) const { return loop->StartOf(Frame); }
		virtual	int		 EndOf(unsigned Frame) const { return loop->EndOf(Frame); }

		virtual void Prepare(GGEMain &main, resource::File &file) {
			Base::Prepare(main, file);
			Mask=dynamic_cast<LineResource*>(file.Root().FindObject(mask));
		}


	protected:
		animation::RectangularGraphic2DSequenceProvider *start, *loop, *end;
		LineResource *Mask;
		utils::SGuid mask;
	};


	inline int Line::calculatewidth (int w) const {
		return (parent.IsHorizontal() ? w : loop->GetWidth()); 
	}
	inline int Line::calculateheight(int h) const { 
		return (parent.IsVertical() ? h : loop->GetHeight()); 
	}
	inline int Line::calculatewidth (const graphics::SizeController2D &controller, int w) const  { 
		if(parent.IsHorizontal()) {
			return controller.CalculateWidth(w, loop->GetWidth(), start->GetWidth()+end->GetWidth());
		}
		else {
			if(controller.HorizontalTiling==graphics::SizeController2D::Single)
				return loop->GetWidth();

			return controller.CalculateWidth(w, loop->GetWidth());
		}
	}
	inline int Line::calculateheight (const graphics::SizeController2D &controller, int h) const  { 
		if(parent.IsVertical()) {
			return controller.CalculateHeight(h, loop->GetHeight(), start->GetHeight()+end->GetHeight());
		}
		else {
			if(controller.VerticalTiling==graphics::SizeController2D::Single)
				return loop->GetHeight();

			return controller.CalculateHeight(h, loop->GetHeight());
		}
	}

	inline Line::Line(LineResource &parent, animation::Timer &controller, bool owner/*=false*/) : parent(parent), Base(controller, owner) {
		start=&parent.GetStart().CreateAnimation(controller);
		loop=&parent.GetLoop().CreateAnimation(controller);
		end=&parent.GetEnd().CreateAnimation(controller);
	}

	inline Line::Line(LineResource &parent, bool create/*=false*/) : parent(parent), Base(create) {
		if(Controller) {
			start=&parent.GetStart().CreateAnimation(*Controller);
			loop=&parent.GetLoop().CreateAnimation(*Controller);
			end=&parent.GetEnd().CreateAnimation(*Controller);
		}
		else {
			start=&parent.GetStart().CreateAnimation();
			loop=&parent.GetLoop().CreateAnimation();
			end=&parent.GetEnd().CreateAnimation();
		}
	}


	inline utils::Margins Line::BorderWidth() {
		if(parent.Orientation==LineResource::Horizontal)
			return utils::Margins(start->GetWidth(), 0, end->GetWidth(),0);
		else
			return utils::Margins(0, start->GetHeight(), 0, end->GetHeight());
	}
	

	inline MaskedLine::MaskedLine(LineResource &parent, animation::Timer &controller, LineResource *mask, bool owner/*=false*/) : 
	Line(parent, controller, owner) {
		Mask=&mask->CreateResizableObject(controller);
	}

	inline MaskedLine::MaskedLine(LineResource &parent, LineResource *mask, bool create/*=false*/) : 
	Line(parent, create) {
		if(Controller)
			Mask=&mask->CreateResizableObject(*Controller);
		else
			Mask=&mask->CreateResizableObject();
	}

}}
