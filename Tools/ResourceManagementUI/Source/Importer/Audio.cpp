#include "Audio.h"

namespace Importer{
    
    Importer::Audio::Audio(){
        std::cout <<  "Audio class is created." << std::endl;
    }

    void Importer::Audio::DoImport(std::string pathFrom, std::string pathTo, std::string resourceType){
        
        Gorgon::Resource::File file;
    
        auto fold_path = pathFrom;

        auto &fold = file.Root();

        for(Gorgon::Filesystem::Iterator it(fold_path); it.IsValid(); it.Next()) {
            auto file_name = *it;
            auto file_path = Gorgon::Filesystem::Join(fold_path, file_name);

            if(Gorgon::String::ToLower(Gorgon::Filesystem::GetExtension(file_name)) != "flac")
                continue;

            auto &sr = *new Gorgon::Resource::Sound;
            Gorgon::Containers::Wave s;
            Gorgon::Encoding::Flac.Decode(file_path, s);
            sr.SetName(Gorgon::Filesystem::GetBasename(file_name));
        
            sr.Assume(s);
        
            fold.Add(sr);
            
            std::cout << Gorgon::String::Concat("Imported ", file_name, ".\n");
        }

        file.Save(Gorgon::String::Concat(pathTo , "/" ,resourceType, "_Audio.gor"));
        
        std::cout << "Audio files Imported!" << std::endl;
    }

}

