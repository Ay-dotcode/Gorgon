#pragma once

#include "../Utils/Margins.h"
#include "../Utils/GGE.h"
#include "../Utils/Utils.h"
#include "../Engine/Graphics.h"
#include "../Resource/ResourceBase.h"

#define GID_RESIZABLEOBJECT			0x05130000
#define GID_RESIZABLEOBJECT_PROPS	0x05130101

namespace gre {

	class ResourceFile;

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
							return gge::Max(floor((float)Requested/Original), 1)*Original;
						case ClosestToGivenSize:
							return gge::Max(gge::Round((float)Requested/Original), 1)*Original;
						case FillTheGivenSize:
							return gge::Max(ceil((float)Requested/Original), 1)*Original;
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

			int Calculate(int w, int AutoOverhead=0, int AutoIncrement=0) {
				if(Type==Continous)
					return w;

				if(w<Overhead)
					return w;
			
				if(Type==Automatic) {
					Overhead=AutoOverhead;
					Increment=AutoIncrement;
				}

				if(CalculationMethod==SmallerThanGivenSize)
					return gge::Max(floor( ((float)(w-Overhead)) / Increment ), 0)  * Increment + Overhead;
				else if(CalculationMethod==ClosestToGivenSize)
					return gge::Max(gge::Round( ((float)(w-Overhead)) / Increment ), 0)  * Increment + Overhead;
				else
					return gge::Max(ceil( ((float)(w-Overhead)) / Increment ), 0)  * Increment + Overhead;
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

		virtual void DrawResized(gge::I2DGraphicsTarget *Target, int X, int Y, int W, int H, gge::Alignment Align)=0;
		virtual void DrawResized(gge::I2DGraphicsTarget &Target, int X, int Y, int W, int H, gge::Alignment Align) { DrawResized(&Target, X, Y, W, H, Align); }
		virtual int  Width(int W=-1)=0;
		virtual int  Height(int H=-1)=0;
		virtual void Reset(bool Reverse=false)=0;
		virtual void Reverse()=0;
		virtual void Play()=0;
		virtual void Pause()=0;
		virtual void setLoop(bool Loop)=0;
		virtual int  getDuration()=0;
		virtual gge::Margins getBorderWidth() { return gge::Margins(0); }
	};

	class ResizableObjectInitiator : public ResourceBase {
	public:
		Guid *target;
		ResizableObject::TileOption		HTile, VTile;
		ResizableObject::IntegralSize	HIntegral, VIntegral;

		virtual int getGID() { return GID_RESIZABLEOBJECT; }

		////Currently does nothing
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		operator ResizableObject *();

	};

	ResourceBase *LoadResizableObject(ResourceFile* File, FILE* Data, int Size);
}