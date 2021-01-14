#include "Image.h"

namespace Importer{
    
    Importer::Image::Image(): 
    
    imgWind("Image Preview",{300,300}),
    wind("Preview Import", {300, 300}),
    scale(1),
    pathFrom(""),
    pathTo(""),
    resourceName(""),
    currentImage("")
    
    
    {
        std::cout << "Image class is created." << std::endl;
        
        
        bmp.Import(Gorgon::String::Concat(Gorgon::Filesystem::ExeDirectory(),"/GRM-Logo-72x72.png"));
        
        bmp.Prepare();
        
        l.GetLayer().Add(imgLayer);
        bmp.Draw(imgLayer,10,10);
        imgWind.Add(l);
        imgWind.SetVisible(false);
        
        
        
        process.SetWidthInUnits(10);
        process.SetHeight(240);
        process.SetOddEven(false);
        
        process.ChangedEvent.Register([&](long index, bool status){
            
            currentImage = "";
            std::cout << "Starting off CurrentImage is " << currentImage << std::endl << "s = ";

            bmp.Clear();
            
            currentImage = process[index];
            
            std::cout << currentImage;
            
            bmp.Import(Gorgon::String::Concat(pathFrom , "/", currentImage ));
            bmp.Prepare();
            imgWind.SetVisible(true);
            imgWind.SetTitle(currentImage);
            
            std::cout << std::endl << "CurrentImage is " << currentImage << std::endl;
        });
        
        imgWind.ClosingEvent.Register([&](){
            imgWind.SetVisible(false);
        });
        
        wind.Add(process);
        
        wind.SetVisible(false);
        
        wind.AddButton("Confirm", [&]{
            ActualImport();
        });
    }
    
    void Image::Set(int s, std::string pF, std::string pT, std::string rN){
        scale = s;
        pathFrom = pF;
        pathTo = pT;
        resourceName = rN;
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
    
    
    int Image::CountItemsInFolder()
    {
        int count = 0;
        for(Gorgon::Filesystem::Iterator it(pathFrom); it.IsValid(); it.Next()){ 
            auto file_name = *it;
            if(Gorgon::String::ToLower(Gorgon::Filesystem::GetExtension(file_name)) != "png" || Gorgon::String::ToLower(Gorgon::Filesystem::GetExtension(file_name)) != "jpeg")
                count++;
            
        }
        return count;
        
    }
    
    void Image::PreviewUI(){
        
        
        
        std::string fileType1 = "png";
        std::string fileType2 = "jpeg";
        
        auto fold_path = pathFrom;
        
        
        for(Gorgon::Filesystem::Iterator it(fold_path); it.IsValid(); it.Next()) {
            auto file_name = *it;
            
            std::cout << "In for loop for Preview\n";

            if(Gorgon::String::ToLower(Gorgon::Filesystem::GetExtension(file_name)) != fileType1)
                continue;
            
            std::cout << "If is done!\n";
            process.Add(Gorgon::String::Concat("",file_name));
                
            
            std::cout << "Added " << file_name << " to preview list\n";
            
        }
    }
    
    void Image::ActualImport()
    {
        std::cout << "Clicked Import\n";
        
        std::string fileType = "png";
        Gorgon::Resource::File file;
        
        auto fold_path = pathFrom;
        
        auto &fold = file.Root();
         std::cout << "About to for loop\n";
        
        for(Gorgon::Filesystem::Iterator it(fold_path); it.IsValid(); it.Next()) {
            auto file_name = *it;
            auto file_path = Gorgon::Filesystem::Join(fold_path, file_name);
            
            std::cout << "In for loop\n";

            if(Gorgon::String::ToLower(Gorgon::Filesystem::GetExtension(file_name)) != fileType)
                continue;
                
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
                
                
                

                std::cout << Gorgon::String::Concat("Imported ", file_name, ".\n");
            }
            else {
                std::cout << Gorgon::String::Concat("Cannot import file: ", file_name, "!\n");
            }
        
        }
        std::cout << "About to save\n";

        file.Save(Gorgon::String::Concat(pathTo , "/" ,resourceName,"_", scale, "x.gor"));
        
        std::cout << "Saving done\n";
        wind.SetVisible(false);
    }



    
    void Image::DoImport (){
        
        int numItems = CountItemsInFolder();
        std::cout << "There are "<< numItems << " items in the folder!\n";
        wind.SetVisible(true);
        PreviewUI();
    }
    
    
}
