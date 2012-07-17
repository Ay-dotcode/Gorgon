#pragma once

#include "../../Resource/Base.h"
#include "../Definitions.h"
#include "../../Utils/Margins.h"
#include "../../Utils/Size2D.h"


namespace gge { namespace widgets {

	class PlaceholderResource;

	PlaceholderResource *LoadPlaceholderResource(resource::File& File, std::istream &Data, int Size);

	struct Placeholder {
		enum PlaceholderSizingMode {
			//Obtainer is free to use any size
			Free=0,
			//Obtainer should try to use the minimum size
			Specified,
			//Obtainer should allocate maximum size available for this placeholder
			MaximumAvailable,
			//Size of the placeholder should be determined using contents
			Contents
		};

		Alignment::Type Align;		
		utils::Margins Margins;
		PlaceholderSizingMode SizingMode;
		utils::Size Minimum;

		virtual utils::Size GetSize(utils::Size ideal, utils::Size maximum) const {
			utils::Size size=ideal;

			switch (SizingMode) {
			case Contents:
			case Free:
				if(Minimum.Width>ideal.Width)
					size.Width=Minimum.Width;
				if(Minimum.Height>ideal.Height)
					size.Height=Minimum.Height;

				return size;

			case MaximumAvailable:
				return maximum;

			case Specified:
			default:
				return Minimum;
			}
		}

		Placeholder() : Align(Alignment::Middle_Center), Margins(0), SizingMode(Free), Minimum(0,0)
		{ }

	};

	class PlaceholderResource : public Placeholder, public resource::Base {
	public:

		virtual GID::Type GetGID() const { return GID::Placeholder; }
	};

}}