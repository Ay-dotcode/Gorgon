#include "Image.h"

namespace Importer{
    
    Importer::Image::Image (std::string pathFrom, std::string pathTo){
        std::cout << "Image class is created\n";
    }
    
    void Image::DoImport (int scale, std::function<void(std::string)> report){
        std::cout << "Image class is does Import\n";
    }
    
    Gorgon::Graphics::Bitmap Scale(const Gorgon::Graphics::Bitmap &bmp, int scale){
        Gorgon::Graphics::Bitmap n(bmp.GetSize() * scale, bmp.GetMode());

        bmp.ForAllPixels([&bmp, &n, scale](int x, int y, int c) {
            for(int yy=0; yy<scale; yy++)
                for(int xx=0; xx<scale; xx++)
                    n(x*scale + xx, y*scale + yy, c) = bmp(x, y, c);
        });

        return n;
    }
}
