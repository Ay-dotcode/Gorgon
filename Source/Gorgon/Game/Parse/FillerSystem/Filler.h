#pragma once
#include <Gorgon/Struct.h>
#include <Gorgon/String.h>
#include <Gorgon/External/PugiXML/Pugi.h>
#include <string>
#include <any>


namespace Gorgon::Game::Parse::Filler {
    /* Exception class */
    // TODO Move that to exceptions file when more exceptio types created. 
    class ParseFailed : public std::exception {
        std::string msg; 
        public: 
        ParseFailed(const char * msg = "Parse failed!") : msg(msg) {}
        const char * what() {
            return msg.c_str();
        }
    };

    // Using Gorgon's Reflection system to handle member assignments in compile. 
    namespace internal {
        template<class T_, int Ind>
        // Sets the member by name if the name exists in the structure's reflection.
        void SetByName_setif(T_ &obj, const std::string &name, const std::string &value) {
            if(T_::Reflection().Names[Ind] == name)
                obj.*(T_::ReflectionType::template Member<Ind>::MemberPointer()) = Gorgon::String::To<typename T_::ReflectionType::template Member<Ind>::Type>(value);
        }
        // Since we can't travel template members in run time, we use Gorgon Sequence for to do that using template parameterrs. 
        template<class T_, int ...S_>
        void SetByName_expand(Gorgon::TMP::Sequence<S_...>, T_ &obj, const std::string &name, const std::string &value) {
            (SetByName_setif<T_, S_>(obj, name, value), ...);
        }
        // I couldn't come up with a better idea to get the value by name,
        // so I used std::any.
        // I don't have an idea to get the value by name with it's type in 
        // compile time.
        // FIXME: Find a way to get the value by name with it's type in compile time.
        class Get {
        private: 
            mutable std::any value; 
            template<class T_, int Ind> 
            void ByName_getif(const T_& obj, const std::string& name) const {
                if (T_::Reflection().Names[Ind] == name)   {
                    value = obj.*(T_::ReflectionType::template Member<Ind>::MemberPointer()); 
                }
            }
            template <class T_, int ...S_>
            void ByName_expand(Gorgon::TMP::Sequence<S_...>, const T_& obj, const std::string& name) const {
                (ByName_getif<T_, S_>(obj, name), ...); 
            }

        public:
            template <class T_>  
            std::any ByName(const T_& obj, const std::string& name) const {
                ByName_expand(typename Gorgon::TMP::Generate<T_::ReflectionType::MemberCount>::Type{}, obj, name);
                auto ret = value;
                value.reset(); 
                return ret; 
            }
            template <class T_> 
            std::any operator()(const T_& obj, const std::string& name) const {
                return ByName(obj, name);
            }
            template<class Cast, class T_>
            Cast To(const T_& obj, const std::string& name) const {
                auto ret = ByName(obj, name);
                return (ret.has_value()) ? std::any_cast<Cast>(ret) : Cast{};
            }
        };
    }
    const internal::Get GetByName;

    // Wrapper for the above. 
    template<class T_>
    void SetByName(T_ &obj, const std::string &name, const std::string &value) {
        internal::SetByName_expand(typename Gorgon::TMP::Generate<T_::ReflectionType::MemberCount>::Type{}, obj, name, value);
    }

    // This filler is written keeping tiled's xml system in mind. 
    // It takes the structer as an array, if you are going to give only one object, 
    // give it as std::array<Structure, 1> struct; 
    // this is a recursive function, 
    // if possible it's going to loop. 
    // TODO: Handle inner nodes.
    // TODO: Implement a way to find how many times the tags appear in the XML file.
    template<size_t Index, size_t ObjArrSize, class Structure>
    void Fill(std::array<Structure, ObjArrSize>& obj, std::string file_name ,std::string firstNode) {
        // PugiXML parser is used for parsing the data in the first step. 
        // this part of the system requires us to use C++ 23 if we are going to use constexpr function.
        // C++ 20 doesn't support constexpr version of this. 
        pugi::xml_document doc; 
        pugi::xml_parse_result res = doc.load_file(file_name.c_str());
        const auto& name = Structure::tag; 
        // if somehow parsing files, Pugi supplies error information.
        // so we throw ParseFaield exception to abort the program. 
        // user can ignore this exception by catching it
        if(!res) {
            throw ParseFailed((std::string("Parsing failed with error: ") + res.description()).c_str());
        }

        // for type shortening purposes, this is set.
        auto attrlist = obj[Index].Reflection().Names;
        auto map = doc.child(firstNode.c_str()); 

        // since the function is recursive, this is necessary to prevent
        // infinite loop
        if constexpr (Index < ObjArrSize) {
            // Pugi doesn't supply binary + operator. 
            // TODO: Implement binary + operator for pugi::xml_node.
            // thus we nned to iterate through the iterator
            auto child = map.begin();
            // pugi child function actually gets the first sibling 
            // of the child (node), but for continuesly iterating, 
            // it doesn't supply a iterator to that child. 
            // so we first iterate to first sibling of the wanted node
            // if name doesn't exist, it's gonna go into infinite loop 
            // when pugi reaches the end of the file or end of the nodes, 
            // it's gonna return an empty string as name.
            while(std::string((*child).name()) != name or std::string((*child).name()) == "") {
                child++; 
            }
            // then we iterate to the node we want using Index
            for(int j = 0; j < Index; j++) {
                child++; 
            }
            // below is self explonatery, yet let me explain. 
            // if the tag set by structure is matches with any child
            // do below; 
            if(name == std::string((*child).name())) {
                // set the matching variables with attribute list. 
                // example; 
                // let's say attribute is width and is in the child, 
                // and if it's also in the structure (must use the same name)
                // it'll be set. 
                for(int i = 0; i < obj[Index].Reflection().MemberCount; i++) {
                    SetByName(obj[Index], attrlist[i], (*child).attribute(attrlist[i]).value());
                }
                // TODO: handle inners as generic as this is. 
                obj[Index].SetInner((*child).first_child());
            }; 
            // Recursive call
            Fill<Index + 1, ObjArrSize, Structure>(obj, file_name, firstNode);
        }
    } 

    // Wrapper for the above.
    // Calls the above function with a vector of structures.
    template<class Structure> 
    void Fill(std::vector<Structure>& obj, std::string file_name, std::string firstNode) {
        for(auto& o : obj) {
            std::array<Structure, 1> arr{o};
            Fill<0, 1, Structure>(arr, file_name, firstNode);
            o = arr[0];
        }
    } 
}