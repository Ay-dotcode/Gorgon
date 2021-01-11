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

#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/Widgets/Button.h>
#include <Gorgon/Widgets/Checkbox.h>
#include <Gorgon/Widgets/RadioButtons.h>
#include <Gorgon/Widgets/Label.h>
#include <Gorgon/Widgets/Panel.h>
#include <Gorgon/Widgets/Textbox.h>
#include <Gorgon/Widgets/Numberbox.h>
#include <Gorgon/Widgets/GeometryBoxes.h>
#include <Gorgon/Widgets/Progressbar.h>
#include <Gorgon/Widgets/Scrollbar.h>
#include <Gorgon/Widgets/Composer.h>
#include <Gorgon/Widgets/Listbox.h>
#include <Gorgon/Widgets/ListItem.h>
#include <Gorgon/Widgets/Dropdown.h>
#include <Gorgon/Widgets/Window.h>
#include <Gorgon/Widgets/DialogWindow.h>

#include <functional>
#include <fstream>


namespace Importer{
    class Image{
    public:
        
        Image();
        
        void DoImport(int scale, std::function<void(std::string)> report, std::string pathFrom, std::string pathTo, std::string resourceName);
        
    private:
        
         
        Gorgon::Graphics::Bitmap Scale(const Gorgon::Graphics::Bitmap &bmp, int scale);
        
        Gorgon::Widgets::DialogWindow wind;
        
        void Preview(std::string fileName);
        
        
    };
}


