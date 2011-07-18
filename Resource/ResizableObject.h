#pragma once

#include "../Utils/Margins.h"
#include "../Utils/BasicMath.h"
#include "../Engine/Graphics.h"
#include "../Resource/ResourceBase.h"

namespace gge { namespace resource {

	class File;

	class ResizableObject {
	public:

		enum TileOption {
			Single=0,
			Tile,
			Integral,
			Stretch,
		};

		enum IntegralSizeOption {
			Continous=0,
			Automatic=1,
			Manual=2,
		};

		enum IntegralSizeCalculationMethod {
			SmallerThanGivenSize,
			ClosestToGivenSize,
			FillTheGivenSize
		};

		class Tiling {
		public:
			TileOption Type;
			IntegralSizeCalculationMethod CalculationMethod;

			Tiling(TileOption Type=Single, IntegralSizeCalculationMethod CalculationMethod=SmallerThanGivenSize) :
			Type(Type), CalculationMethod(CalculationMethod)
			{ }


			int Calculate(int Original, int Requested) {
				if(Type==Single)
					return Original;

				if(Type==Integral) {
					switch(CalculationMethod) {
						case SmallerThanGivenSize:
							return (int)utils::Max(floor((float)Requested/Original), 1)*Original;
						case ClosestToGivenSize:
							return (int)utils::Max(utils::Round((float)Requested/Original), 1)*Original;
						case FillTheGivenSize:
							return (int)utils::Max(ceil((float)Requested/Original), 1)*Original;
					}
				}

				return Requested;
			}
		};

		class IntegralSize {
		public:
			int Overhead, Increment;
			IntegralSizeOption Type;
			IntegralSizeCalculationMethod CalculationMethod;



			IntegralSize(int Overhead, int Increment, IntegralSizeOption Type=Manual) :
			Overhead(Overhead), Increment(Increment), Type(Type), CalculationMethod(SmallerThanGivenSize)
			{ }

			IntegralSize(IntegralSizeOption Type) :
			Overhead(0), Increment(0), Type(Type), CalculationMethod(SmallerThanGivenSize)
			{ }

			IntegralSize() :
			Overhead(0), Increment(0), Type(Continous), CalculationMethod(SmallerThanGivenSize)
			{ }

			int Calculate(int w, int AutoOverhead=0, int AutoIncrement=1) {
				if(Type==Continous)
					return w;

				if(w<Overhead)
					return w;
			
				if(Type==Automatic) {
					Overhead=AutoOverhead;
					Increment=AutoIncrement;
				}

				if(CalculationMethod==SmallerThanGivenSize)
					return (int)utils::Max(floor( ((float)(w-Overhead)) / Increment ), 0)  * Increment + Overhead;
				else if(CalculationMethod==ClosestToGivenSize)
					return (int)utils::Max(utils::Round( ((float)(w-Overhead)) / Increment ), 0)  * Increment + Overhead;
				else
					return (int)utils::Max(ceil( ((float)(w-Overhead)) / Increment ), 0)  * Increment + Overhead;
			}
		};

		static IntegralSize IntegralSizeBestFit(int Overhead, int Increment, IntegralSizeOption Type=Manual) {
			IntegralSize is(Overhead, Increment, Type);
			is.CalculationMethod=ClosestToGivenSize;

			return is;
		}

		static IntegralSize IntegralSizeBestFit(IntegralSizeOption Type=Automatic) {
			IntegralSize is(Type);
			is.CalculationMethod=ClosestToGivenSize;

			return is;
		}

		static IntegralSize IntegralSizeSmaller(int Overhead, int Increment, IntegralSizeOption Type=Manual) {
			IntegralSize is(Overhead, Increment, Type);
			is.CalculationMethod=SmallerThanGivenSize;

			return is;
		}

		static IntegralSize IntegralSizeSmaller(IntegralSizeOption Type=Automatic) {
			IntegralSize is(Type);
			is.CalculationMethod=SmallerThanGivenSize;

			return is;
		}

		static IntegralSize IntegralSizeFill(int Overhead, int Increment, IntegralSizeOption Type=Manual) {
			IntegralSize is(Overhead, Increment, Type);
			is.CalculationMethod=FillTheGivenSize;

			return is;
		}

		static IntegralSize IntegralSizeFill(IntegralSizeOption Type=Automatic) {
			IntegralSize is(Type);
			is.CalculationMethod=FillTheGivenSize;

			return is;
		}

		virtual void DrawResized(graphics::I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment::Type Align=Alignment::Center) =0;
		virtual void DrawResized(graphics::I2DGraphicsTarget &Target, int X, int Y, int W, int H, Alignment::Type Align=Alignment::Center) { DrawResized(&Target, X, Y, W, H, Align); }
		virtual void DrawResized(graphics::I2DGraphicsTarget *Target, int X=0, int Y=0, Alignment::Type Align=Alignment::Center) { DrawResized(Target, X, Y, Target->Width(), Target->Height(), Align); }
		virtual void DrawResized(graphics::I2DGraphicsTarget &Target, int X=0, int Y=0, Alignment::Type Align=Alignment::Center) { DrawResized(&Target, X, Y, Target.Width(), Target.Height(), Align); }
		virtual int  Width(int W=-1)=0;
		virtual int  Height(int H=-1)=0;
		virtual void Reset(bool Reverse=false) {}
		virtual void Reverse() {}
		virtual void Play() {}
		virtual void Pause() {}
		virtual void setLoop(bool Loop) {}
		virtual int  getDuration() { return 0; }
		virtual utils::Margins getBorderWidth() { return utils::Margins(0); }

		virtual ~ResizableObject() {}
	};

} }
