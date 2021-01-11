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
#include <Gorgon/Resource/Sound.h>
#include <Gorgon/Encoding/FLAC.h>

#include <functional>
#include <fstream>


namespace Importer{
    class Audio{
    public:
        
        Audio();
        
        void DoImport(std::string pathFrom, std::string pathTo, std::string resourceType);
        
    private:
        
        
       
    };
}
