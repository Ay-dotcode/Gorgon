#pragma once
#include "../PugiXML/pugixml.hpp"
#include <Gorgon/Struct.h>
#include <Gorgon/String.h>
#include <string>

class ParseFailed : public std::exception {
    std::string msg; 
    public: 
    ParseFailed(const char * msg = "Parse failed!") : msg(msg) {}
    const char * what() {
        return msg.c_str();
    }
};

namespace Filler {
    namespace internal {
        template<class T_, int Ind>
        void SetByName_setif(T_ &obj, const std::string &name, const std::string &value) {
            if(T_::Reflection().Names[Ind] == name)
                obj.*(T_::ReflectionType::template Member<Ind>::MemberPointer()) = Gorgon::String::To<typename T_::ReflectionType::template Member<Ind>::Type>(value);
        }
        template<class T_, int ...S_>
        void SetByName_expand(Gorgon::TMP::Sequence<S_...>, T_ &obj, const std::string &name, const std::string &value) {
            (SetByName_setif<T_, S_>(obj, name, value), ...);
        }
    }

    template<class T_>
    void SetByName(T_ &obj, const std::string &name, const std::string &value) {
        internal::SetByName_expand(typename Gorgon::TMP::Generate<T_::ReflectionType::MemberCount>::Type{}, obj, name, value);
    }

    template<size_t Index, size_t ObjArrSize, size_t AttrCount, class Structure>
    constexpr void Fill(std::array<Structure, ObjArrSize>& obj, std::string file_name ,std::string firstNode) {
        static pugi::xml_document doc; 
        static pugi::xml_parse_result res;
        static const auto& name = Structure::tag; 

        if(std::string(doc.name()) == "") {
            res = doc.load_file(file_name.c_str());
        }
        if(!res) {
            throw ParseFailed((std::string("Parsing failed with error: ") + res.description()).c_str());
        }

        auto attrlist = obj[Index].Reflection().Names;
        auto map = doc.child(firstNode.c_str()); 


        if constexpr (Index < ObjArrSize) {
            auto child = map.begin(); 
            for(int j = 0; j < Index; j++) {
                child++; 
            }
            if(name == std::string((*child).name())) {
                for(int i = 0; i < obj[Index].Reflection().MemberCount; i++) {
                    SetByName(obj[Index], attrlist[i], (*child).attribute(attrlist[i]).value());
                }
            }; 
            Fill<Index + 1, ObjArrSize, AttrCount, Structure>(obj, file_name, firstNode);
        }
    } 

}