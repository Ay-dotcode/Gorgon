#ifndef NULLABLE_H
#define NULLABLE_H

#include <any>
#include <ostream>
#include <format>

/* 
    Experimental Nullable Type
*/
namespace Gorgon::Game::Parser::Experimental::Any {
    /* If type deduction fails... 
    TODO: Get rid of this safely */
    class unknown_type {};

    /* Name is self explanitory */
    template<typename  Ty = unknown_type>
    class nullable_type {
        private: 
            std::any a;
        public: 
            nullable_type(Ty value) : a(value) {}
            nullable_type(std::any a) : a(a) {}
            nullable_type() : a(std::any()) {}

            void operator=(nullable_type<Ty> obj) {
                a = obj.a;
            }
            void operator=(std::any obj) {
                a = obj;
            }
            void operator=(const Ty &obj) {
                a = obj;
            }


            Ty Get() const{
                return std::any_cast<Ty>(a);
            }

            // If somehow type deduction fails and you need to use the item and if you know the type of the item
            // use this to access to data. 
            template<class T>
            T Get() const {
                return std::any_cast<T>(a);
            }

            operator Ty() const {
                return Get();
            }
            
            friend std::ostream& operator<<(std::ostream &os, const nullable_type<Ty> &a) {
                os << std::format("{}", a.Get());
                return os;
            }

            // Necessary check.
            constexpr const bool has_value() const {
                return a.has_value();
            }

            // Bool check operation.
            bool operator<(Ty item) const {
                return (has_value() ? Ty(*this) < item : false); 
            }

            bool operator==(Ty item) const {
                return (has_value() ? Ty(*this) == item : false); 
            }

            // Type conversion for incrementing. If type has a increment operator call, it'll be called.
            nullable_type& operator++() {
                auto it = Get(); 
                ++it;
                (*this) = it;   
                return *this;
            }
            nullable_type operator++(int) {
                nullable_type copy(*this); 
                operator++();
                return copy; 
            }

            // Type conversion for pre decrementing. If type has a decrement operator call, it'll be called. 
            nullable_type& operator--() {
                auto it = Get(); 
                --it; 
                (*this) = it; 
                return *this;
            }
            nullable_type operator--(int) {
                nullable_type copy(*this); 
                operator--();
                return copy; 
            }


    };


}

template<class T, class CharT>
struct std::formatter<Gorgon::Game::Parser::Experimental::Any::nullable_type<T>, CharT> : std::formatter<T, CharT> 
{
    template<class FormatContext>
    auto format(Gorgon::Game::Parser::Experimental::Any::nullable_type<T> t, FormatContext& fc) const 
    {
      if(!t.has_value()) {
        return std::formatter<T, CharT>::format(T{}, fc);
      }
        return std::formatter<T, CharT>::format(t.Get(), fc);
    }
};
 

#endif 