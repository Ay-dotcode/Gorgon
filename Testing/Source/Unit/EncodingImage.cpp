#define CATCH_CONFIG_MAIN


#include <catch.h>

#include <Gorgon/Containers/Image.h>
#include <Gorgon/Encoding/PNG.h>
#include "Gorgon/Filesystem.h"

using namespace Gorgon;

bool operator ==(const Containers::Image &im1, const Containers::Image &im2) {
	if(im2.GetSize() != im1.GetSize() || im2.GetMode() != im1.GetMode())
		return false;

	for(int y=0; y<im1.GetSize().Height; y++) {
		for(int x=0; x<im1.GetSize().Width; x++) {
			for(unsigned c=0; c<im1.GetBytesPerPixel(); c++)
				if(im1({x,y}, c) != im2({x,y}, c))
					return false;
		}
	}

	return true;
}

TEST_CASE("PNG compression/decompression") {
	Containers::Image im1({36, 36}, Graphics::ColorMode::RGBA);

	int diff = 51;

	for(int r=0; r<6; r++) {
		for(int g=0; g<6; g++) {
			for(int b=0; b<6; b++) {
				for(int a=0; a<6; a++) {
					im1({r + g*6, b + a*6}, 0) = r*diff;
					im1({r + g*6, b + a*6}, 1) = g*diff;
					im1({r + g*6, b + a*6}, 2) = b*diff;
					im1({r + g*6, b + a*6}, 3) = a*diff;
				}
			}
		}
	}

	Encoding::Png.Encode(im1, "out.png");

	Containers::Image im2;

	REQUIRE(Filesystem::IsFile("out.png"));
	REQUIRE(Filesystem::Size("out.png") > 8);

	Encoding::Png.Decode("out.png", im2);

	REQUIRE(im2.GetSize() == im1.GetSize());
	REQUIRE(im2.GetMode() == im1.GetMode());

	REQUIRE(im1 == im2);

	im1.Resize({36, 6}, Graphics::ColorMode::RGB);

	for(int r=0; r<6; r++) {
		for(int g=0; g<6; g++) {
			for(int b=0; b<6; b++) {
				im1({r + g*6, b}, 0) = r*diff;
				im1({r + g*6, b}, 1) = g*diff;
				im1({r + g*6, b}, 2) = b*diff;
			}
		}
	}

	std::ofstream outfile("out.png", std::ios::binary);
	Encoding::Png.Encode(im1, outfile);
	outfile.close();

	std::ifstream infile("out.png", std::ios::binary);
	Encoding::Png.Decode(infile, im2);

	REQUIRE(im2.GetSize() == im1.GetSize());
	REQUIRE(im2.GetMode() == im1.GetMode());

	REQUIRE(im1 == im2);


	im1.Resize({6, 6}, Graphics::ColorMode::Grayscale_Alpha);

	for(int g=0; g<6; g++) {
		for(int a=0; a<6; a++) {
			im1({g, a}, 0) = g*diff;
			im1({g, a}, 1) = a*diff;
		}
	}

	Encoding::Png.Encode(im1, "outfile.png");

	Encoding::Png.Decode("outfile.png", im2);

	REQUIRE(im2.GetSize() == im1.GetSize());
	REQUIRE(im2.GetMode() == im1.GetMode());

	REQUIRE(im1 == im2);


	im1.Resize({6, 1}, Graphics::ColorMode::Grayscale);

	for(int g=0; g<6; g++) {
		im1({g, 0}, 0) = g*diff;
	}

	std::vector<Byte> vec;
	Encoding::Png.Encode(im1, vec);

	Encoding::Png.Decode(vec, im2);

	REQUIRE(im2.GetSize() == im1.GetSize());
	REQUIRE(im2.GetMode() == im1.GetMode());

	REQUIRE(im1 == im2);
}