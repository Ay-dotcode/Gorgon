#pragma once

#include <Gorgon/Main.h>
#include <Gorgon/Window.h>
#include <Gorgon/Geometry/Point.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Graphics/FreeType.h>
#include <Gorgon/String.h>
#include <Gorgon/Filesystem.h>
#include <Gorgon/Filesystem/Iterator.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Image.h>
#include <Gorgon/Resource/Data.h>

#include <functional>
#include <fstream>


namespace Importer{
    class Image{
    public:
        
        Image(std::string pathFrom, std::string pathTo);
        
        
        
    private:
        
        void DoImport(int scale, std::function<void(std::string)> report);
        Gorgon::Graphics::Bitmap Scale(const Gorgon::Graphics::Bitmap &bmp, int scale);
    };
}


