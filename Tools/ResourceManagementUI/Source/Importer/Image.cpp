#include "Image.h"

namespace Importer{
    
    Importer::Image::Image (): wind("Preview Import", {300, 300}){
        std::cout << "Image class is created." << std::endl;
        wind.SetVisible(true);
    }
    
    Gorgon::Graphics::Bitmap Importer::Image::Scale(const Gorgon::Graphics::Bitmap &bmp, int scale){
        Gorgon::Graphics::Bitmap n(bmp.GetSize() * scale, bmp.GetMode());

        bmp.ForAllPixels([&bmp, &n, scale](int x, int y, int c) {
            for(int yy=0; yy<scale; yy++)
                for(int xx=0; xx<scale; xx++)
                    n(x*scale + xx, y*scale + yy, c) = bmp(x, y, c);
        });

        return n;
    }
    
    void Image::Preview(std::string fileName){
        
    }

    
    void Image::DoImport (int scale, std::function<void(std::string)> report, std::string pathFrom, std::string pathTo, std::string resourceName){
        std::cout << "Image class is does Import\n";
        std::string fileType = "png";
        Gorgon::Resource::File file;
        
        auto fold_path = pathFrom;
        
        auto &fold = file.Root();
        
        for(Gorgon::Filesystem::Iterator it(fold_path); it.IsValid(); it.Next()) {
            auto file_name = *it;
            auto file_path = Gorgon::Filesystem::Join(fold_path, file_name);
            
            std::cout << "In for loop\n";

            if(Gorgon::String::ToLower(Gorgon::Filesystem::GetExtension(file_name)) != fileType)
                continue;
            
            Preview(resourceName);

            Gorgon::Graphics::Bitmap im;
            
            std::cout << "About to Import\n";
            
            if(im.Import(file_path)) {
                im = Scale(im, scale);

                auto &imres = *new Gorgon::Resource::Image(std::move(im));
                fold.Add(imres);
                //auto &data  = *new Gorgon::Resource::Data;
                //fold.Add(data);

                /*auto name = Gorgon::Filesystem::GetBasename(file_name);
                std::string descr = "";
                auto off  = Gorgon::Geometry::Point(0, 0);

                std::ifstream dataf(Gorgon::Filesystem::Join(fold_path, name + ".txt"));

                if(dataf.is_open()) {
                    std::string line;

                    std::getline(dataf, line);
                    if(!line.empty())
                        name = Gorgon::String::Trim(line);

                    std::getline(dataf, line);
                    if(!line.empty())
                        descr = Gorgon::String::Trim(line);
    
                    std::getline(dataf, line);
                    if(!line.empty())
                        off  = Gorgon::String::To<Gorgon::Geometry::Point>(line);
                    if(!IsInside(Gorgon::Geometry::Bounds(0, 0, 1024, 1024), off))
                        off ={0, 0};
                }*/

                //data.Append("name", name);
                //data.Append("descr", descr);
                //data.Append("offset", off);

                report(Gorgon::String::Concat("Imported ", file_name, "."));
            }
            else {
                report(Gorgon::String::Concat("Cannot import file: ", file_name, "!"));
            }
        }
        
        std::cout << "About to save\n";

        file.Save(Gorgon::String::Concat(resourceName,"_", scale, "x.gor"));
        
        std::cout << "Saving done\n";
    }
    
    
}
