#pragma once
#include <Gorgon/External/PugiXML/pugi/pugixml.hpp>
#include <Gorgon/Struct.h>
#include <Gorgon/String.h>
#include <Gorgon/External/PugiXML/Pugi.h>
#include <Gorgon/Game/Exceptions/Exception.h>
#include <Gorgon/TMP.h>
#include <Gorgon/Utils/Assert.h>
#include <cstddef>
#include <format>
#include <string>
#include <any>
#include <vector>


namespace Gorgon::Game::Parse::Filler {

    // Using Gorgon's Reflection system to handle member assignments in compile. 
    namespace internal {
        inline size_t count(const pugi::xml_node_iterator& parent_node, std::string keyword) {
            auto children = parent_node->children("keyword");
            if(children.empty()) {
                return 0; 
            } 
            int count = 0; 
            for(auto child : children) {
                count++; 
            }
            return count; 
        }

        class any  : public std::any {
            public:
            any() : std::any() {}

            any(const std::any& a) : std::any(a) {}

            any(const std::any&& a) : std::any(std::move(a)) {}

            template<typename T_>
            any(const T_& val) : std::any(val) {}

            template<typename T_>
            void operator=(T_ val) {
                this->emplace<T_>(val);
            }

            template<typename T_>
            operator T_() {
                return any_cast<T_>(*this);
            }
       };

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
            mutable any value; 
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
            any ByName(const T_& obj, const std::string& name) const {
                ByName_expand(typename Gorgon::TMP::Generate<T_::ReflectionType::MemberCount>::Type{}, obj, name);
                auto ret = value;
                value.reset(); 
                return ret; 
            }

            template <class T_> 
            any operator()(const T_& obj, const std::string& name) const {
                return ByName(obj, name);
            }

            template<class R_, class T_>
            R_ To(const T_& obj, const std::string& name) const {
                auto ret = ByName(obj, name);
                return (ret.has_value()) ? std::any_cast<R_>((std::any)ret) : R_{};
            }
            
        };
    }
    const internal::Get GetByName;

    // Wrapper for the above. 
    template<class T_>
    void SetByName(T_ &obj, const std::string &name, const std::string &value) {
        internal::SetByName_expand(typename Gorgon::TMP::Generate<T_::ReflectionType::MemberCount>::Type{}, obj, name, value);
    }

}

namespace Gorgon::Game::Parse::Filler {
    using namespace internal;

    template<typename T_> 
    concept has_tag = requires(T_ t) {
        T_::tag; 
        { t.Reflection() }; 
    }; 

    template<typename T_>
    concept has_set = requires(T_ t, pugi::xml_node_iterator& it) {
        { t.Set(it) }; 
    };

    inline pugi::xml_node_iterator& operator+(pugi::xml_node_iterator& node, const size_t& n) {
        for(int i = 0; i < n; i++) {
            node++; 
        }
        return node; 
    }

    inline bool operator==(const char * st1, const std::string& st2) {
        return std::string(st1) == st2; 
    }

    inline bool operator==(const std::string& st1, const char * st2) {
        return std::string(st2) == st1;  
    }

    /**
     * @brief 
     * Object filling system for parsing purposes. 
     * @tparam Index 
     * @tparam Struct 
     * @tparam N 
     * @param obj_list // Object List as an array. 
     * @param first_child // First child of the node for search through.  
     */
    template<int Index, size_t N, has_tag Struct> 
    constexpr void Fill(std::array<Struct, N>& obj_list, pugi::xml_node_iterator& first_child, pugi::xml_node_iterator& last_node ) {
        /*
         * Get tag from class, for node search.
         */
        const std::string tag = Struct::tag; 

        /*
         * This function works in a recursive manner. Thus, it requires a fail check to 
         * prevent infinite loops. 
         */
        if constexpr (Index < N) {
            /* 
             * For type shortening.
             */
            auto attrlist = obj_list[Index].Reflection().Names;

            /*
             * Find if the given tag exist in the nodes. 
             * We don't know if the given tag exist in the list or if it's in the beggining. 
             * So we search till find it. IF we found nothing, pugi node's name will be empty. 
             * And if it's empty, it won't match with our name check, thus it won't run. No big problem. 
             */
            while(std::string((*first_child).name()) != tag and first_child != last_node) {
                first_child++; 
            }

            /*
             * If tag is given wrong, it'll cause a runtime error. 
             */
            if(first_child == last_node) {
                throw Exception::bad_parameter(std::format("Tag {} doesn't exist in the file. Did you make a typo?", tag));
            }

            /*
             * We are trying to access the next node. If we keep returning the values of same node, 
             * it'd be faulty behaviour. 
             */
            first_child = first_child + Index; 

            /*
             * Construct a new if object
             * Check if tag matches with the node. If not, we skip safely.
             */
            if (tag == (*first_child).name()) {
                /*
                 * Set attributes.
                 * We get the class member names from reflection. After that, we check up if 
                 * names match with the attributes of the node. If yes, we assign them. 
                 * Explanation of how SetByName works is it's own documents. 
                 */
                for (int i {0}; i < obj_list[Index].Reflection().MemberCount; i++) {
                    SetByName(obj_list[Index], attrlist[i], (*first_child).attribute(attrlist[i]).value()); 
                }
                /*
                 * Check if set function exist.
                 * Thanks to CPP concepts, we can check if this function exists, 
                 * and calls if exists. 
                 */
                if constexpr (has_set<Struct>) {
                    obj_list[Index].Set(first_child->first_child()); 
                }
            }
            /*
             * Recursive call. 
             */
            Fill<Index + 1, N, Struct>(obj_list, first_child, last_node); 
        }
    }

    /*
     * @brief Wrapper for pointer types.
     * Uses compile time index sequence parameter packs to 
     * initialize an array of structure. 
     * @tparam Index 
     * @tparam N 
     * @tparam Struct 
     * @param obj_list 
     * @param first_node 
     */
    template<int Index, size_t N, has_tag Struct> 
    constexpr void Fill(std::array<Struct*, N>& obj_list, pugi::xml_node_iterator& first_node, pugi::xml_node_iterator& last_node) {
        std::array<Struct, N> arr; 
        for(int i{}; i < N; i++) {
            arr[i] = *obj_list[i];
        }
        Fill<Index, N, Struct>(arr, first_node, last_node); 
        for(int i{}; i < N; i++) {
            *obj_list[i] = arr[i]; 
        }
    }

    /**
     * @brief Non-constexpr re-implementation of fill. 
     * 
     * @tparam ArrSize 
     * @tparam Structure 
     * @param obj_list 
     * @param first_child 
     */
    template<has_tag Structure> 
    void Fill(std::vector<Structure>& obj_list,  pugi::xml_node_iterator first_child, pugi::xml_node_iterator last_node) {
        /*
         * Get tag from class, for node search.
         */
        const std::string tag = Structure::tag;
        /*
         * Find if the given tag exist in the nodes. 
         * We don't know if the given tag exist in the list or if it's in the beggining. 
         * So we search till find it. IF we found nothing, pugi node's name will be empty. 
         * And if it's empty, it won't match with our name check, thus it won't run. No big problem. 
         */
        while(std::string((*first_child).name()) != tag and first_child != last_node) {
            first_child++; 
        }

        for(auto i {0}; i < obj_list.size(); i++) {
            /*
             * For type shortening.
             */
            const auto attrlist = obj_list[i].Reflection().Names; 

            auto child = first_child; 

            /*
             * if tag doesn't exist, it'll throw a runtime error.
             */
            if (first_child == last_node) {
                throw Exception::bad_parameter(std::format("Tag {} doesn't exist in the file. Did you make a typo?", tag));
            }

            /*
             * Access to given tag.
             */
            child + i; 

            /*
             * Double safety check. 
             * If tag doesn't match, it'll do nothing. 
             */
            if (tag == (*child).name()) {
                /*
                 * Set attributes.
                 * We get the class member names from reflection. After that, we check up if 
                 * names match with the attributes of the node. If yes, we assign them. 
                 * Explanation of how SetByName works is it's own documents. 
                 */
                for(int j{}; j < obj_list[i].Reflection().MemberCount; j++) {
                    SetByName(obj_list[i], attrlist[j], (*child).attribute(attrlist[j]).value()); 
                }
                
                /*
                 * Check if set function exist.
                 * Thanks to CPP concepts, we can check if this function exists, 
                 * and calls if exists. 
                 */
                if constexpr (has_set<Structure>) {
                    obj_list[i].Set(child->first_child()); 
                }
            }
        } 
    }

    template<has_tag Structure> 
    void Fill(std::vector<Structure>& obj_list, pugi::xml_object_range<pugi::xml_named_node_iterator> nodes) {

        const std::string tag = Structure::tag; 
        for(auto node : nodes) {
            ASSERT(tag == node.name(), std::format("Error: The tag is not matching. Make sure you gave the corect node list. ({} != {})", Structure::tag, node.name()));  

            obj_list.push_back({});

            const auto attrlist = obj_list.back().Reflection().Names; 

            for(int i{}; i < obj_list.back().Reflection().MemberCount; i++) {
                SetByName(obj_list.back(), attrlist[i], node.attribute(attrlist[i]).value()); 
            }

            if constexpr (has_set<Structure>) {
                obj_list.back().Set(node.first_child()); 
            }
        }

    }
    
}