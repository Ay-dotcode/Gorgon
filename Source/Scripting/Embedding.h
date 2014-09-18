#pragma once

#include <tuple>

#include "Reflection.h"
#include "../Scripting.h"
#include "../TMP.h"


namespace Gorgon {
	
	namespace Scripting {
		
		template <class ...T_>
		std::tuple<T_...> MappedFunctions(T_...args) {
			return std::make_tuple(args...);
		}
		
		template <class ...T_>
		std::tuple<T_...> MappedMethods(T_...args) {
			return std::make_tuple(args...);
		}
		
		/// This class is used to create linking to a c++ function
		class MappedFunction : public Function {
			/// @cond INTERAL
			struct fnstorage {
				virtual ~fnstorage() {}
				virtual Data call(const std::vector<Data> &data) const = 0;
				virtual bool isempty() const = 0;
			};
			
			template<class ...Fns_>
			struct fnstorageimpl : public fnstorage {
				using tupletype=std::tuple<Fns_...>;
				template<int level>
				using functionat = typename std::tuple_element<level, tupletype>::type;
				template<int level>
				using traitsof = TMP::FunctionTraits<functionat<level>>;
				
				template<int level, int param>
				void checkfnparam() {
					static_assert(std::is_same<
						typename traitsof<level>::template Arguments<param>::Type,
						typename traitsof<0>::template Arguments<param>::Type>::value,
						"Function types do not match!"
					);
				}
				
				template<int level, int ...S>
				void checkfnlevel(TMP::Sequence<S...>) {
					static_assert(traitsof<level>::Arity == traitsof<0>::Arity-level, 
								  "Number of function parameters does not match");
					
					static_assert(std::is_same<typename traitsof<level>::ReturnType,
								  typename traitsof<0>::ReturnType>::value,
								  "Return type of parameters does not match");
					
					char dummy[] = {(checkfnparam<level, S>(),'\0')...};
				}
				
				template<int ...S>
				void checkallfns(TMP::Sequence<S...>) {
					char dummy[]={(checkfnlevel<S>(typename TMP::Generate<traitsof<S>::Arity>::Type()),'\0')...};
				}

				fnstorageimpl(MappedFunction &parent, std::tuple<Fns_...> fns) : parent(parent), fns(fns) {
					
					static_assert(sizeof...(Fns_)<=traitsof<0>::Arity+1,
								  "Number of functions are more than possible");
					
					
					checkallfns(typename TMP::Generate<sizeof...(Fns_)>::Type());
					
					bool shouldallbeoptional = false;
					if(parent.Parameters.GetCount()>0 && parent.Parameters[0].IsOptional() && 
						parent.Parameters.Last()->IsOptional()) {
						shouldallbeoptional=true;
						}
						
						int optionalcount=0;
					bool passedfirstnonoptional=false;
					bool optionalatstart=false;
					bool first=true;
					for(const auto &param : parent.Parameters) {
						if(param.IsOptional()) {
							optionalcount++;
							if(first) optionalatstart=true;
							if(passedfirstnonoptional) {
								assert(!optionalatstart && 
								"Optional parameters should be at the start or at the end");
							}
						}
						else {
							assert(!shouldallbeoptional && 
							"All optional function parameters should be either at the beginning or at the end, never at both sides");
							
							passedfirstnonoptional=true;
						}
						first=false;
					}
					
					assert(optionalcount+1 >= sizeof...(Fns_) && 
						"Too many function definitions, might be caused by a missing OptionalTag");
					
					assert(optionalcount+1 <= sizeof...(Fns_) && 
						"Missing function definitions");
				}
				
				static const int maxarity=traitsof<0>::Arity;
				
				/// Casts the given data to the type of the num^{th} parameter. Parameter list is always taken from 
				/// first function
				template<int num>
				typename traitsof<0>::template Arguments<num>::Type 
				castparam(Data data) const {
					return data.GetValue<typename traitsof<0>::template Arguments<num>::Type>();
				}
				
				/// Casts the given data to the type of the num^{th} parameter. Parameter list is always taken from 
				/// first function. This one casts first parameter to non-pointer
				template<int num>
				typename TMP::Choose<num==0, typename traitsof<0>::template Arguments<num>::Type, 
				typename std::remove_pointer<typename traitsof<0>::template Arguments<num>::Type>::type>::Type
				castparam_firstnonptr(Data data) const {
					return data.GetValue<typename TMP::Choose<num==0, typename traitsof<0>::template Arguments<num>::Type, 
					typename std::remove_pointer<typename traitsof<0>::template Arguments<num>::Type>::type>::Type>();
				}
				
				/// Calls the correct function variant. This function casts arguments while extracting them
				template<class R_, int variant, int ...S>
				typename std::enable_if<std::is_same<R_, void>::value && traitsof<0>::IsMember, Data>::type 
				callfn(TMP::Sequence<S...>, const std::vector<Data> &data) const {
					assert(data.size()==sizeof...(S) && 
					"Size of data does not match to the number of parameters!");
					
					auto fn = std::get<variant>(fns);
					
					if((parent.HasParent() && !parent.GetParent().IsReferenceType()) || 
						(!parent.HasParent() && parent.Parameters.GetCount() && !parent.Parameters[0].GetType().IsReferenceType())
					) {
						std::bind(fn, castparam_firstnonptr<S>(data[S])...)();
					}
					else {
						std::bind(fn, castparam<S>(data[S])...)();
					}
					
					return Data::Invalid();
				}
				
				template<class R_, int variant, int ...S>
				typename std::enable_if<!std::is_same<R_, void>::value && traitsof<0>::IsMember, Data>::type 
				callfn(TMP::Sequence<S...>, const std::vector<Data> &data) const {
					assert(data.size()==sizeof...(S) && 
					"Size of data does not match to the number of parameters!");
					
					auto fn = std::get<variant>(fns);
					
					if((parent.HasParent() && !parent.GetParent().IsReferenceType()) || 
						(!parent.HasParent() && parent.Parameters.GetCount() && !parent.Parameters[0].GetType().IsReferenceType())
					) {
						return Data{parent.GetReturnType(), Any(std::bind(fn, castparam_firstnonptr<S>(data[S])...)())};
					}
					else {
						return Data{parent.GetReturnType(), Any(std::bind(fn, castparam<S>(data[S])...)())};
					}
				}
				
				/// Calls the correct function variant. This function casts arguments while extracting them
				template<class R_, int variant, int ...S>
				typename std::enable_if<std::is_same<R_, void>::value && !traitsof<0>::IsMember, Data>::type 
				callfn(TMP::Sequence<S...>, const std::vector<Data> &data) const {
					assert(data.size()==sizeof...(S) && 
					"Size of data does not match to the number of parameters!");
					
					auto fn = std::get<variant>(fns);
					
					std::bind(fn, castparam<S>(data[S])...)();
					
					return Data::Invalid();
				}
				
				template<class R_, int variant, int ...S>
				typename std::enable_if<!std::is_same<R_, void>::value && !traitsof<0>::IsMember, Data>::type 
				callfn(TMP::Sequence<S...>, const std::vector<Data> &data) const {
					assert(data.size()==sizeof...(S) && 
					"Size of data does not match to the number of parameters!");
					
					auto fn = std::get<variant>(fns);
					
					return Data{parent.GetReturnType(), Any(std::bind(fn, castparam<S>(data[S])...)())};
				}
				
				/// Calls callfn for the requested variant
				template<int variant>
				Data callvariant(const std::vector<Data> &data) const {
					static_assert(variant<sizeof...(Fns_), "Variant number is out of bounds.");
						
					auto fn = std::get<variant>(fns);
					
					typedef TMP::FunctionTraits<decltype(fn)> traits;
					
					return callfn<typename traits::ReturnType, variant>(
						typename TMP::Generate<traits::Arity>::Type(),
						data
					);
				}
				
				/// Expand all variants and chooses the correct one call
				template<int ...variants>
				Data expandvariants(TMP::Sequence<variants...>, int variant, const std::vector<Data> &data) const {
					assert(variant<sizeof...(Fns_) && "Variant number is out of bounts");
					typedef Data(fnstorageimpl::*calltype)(const std::vector<Data>&) const;
					static calltype list[]={&fnstorageimpl::callvariant<variants>...};
					return (this->*list[variant])(data);
				}
				
				virtual Data call(const std::vector<Data> &data) const override {
					assert(traitsof<0>::Arity == parent.Parameters.GetCount() + parent.HasParent() && 
						"Defined parameters does not match the number of function parameters");
					
					return expandvariants(typename TMP::Generate<sizeof...(Fns_)>::Type(), 
										  traitsof<0>::Arity-data.size(), data);
				}
				
				virtual bool isempty() const override {
					return sizeof...(Fns_)==0;
				}
				
				std::tuple<Fns_...> fns;
				MappedFunction &parent;
			};
			
			void initmethods(std::tuple<> methods) {
				assert(!HasMethod() && "Method implementation is missing");
			}
			
			template<class ...T_>
			void initmethods(std::tuple<T_...> methods) {
				assert(HasMethod() && "MethodTag is missing");
				
				this->methods  =new fnstorageimpl<T_...>(*this, methods);
			}
			/// @endcond
			
		public:
			
			/// Constructor, returntype and tags are optional, 
			template<class ...P_, class ...Fns1_, class ...Fns2_>
			MappedFunction(const std::string &name, const std::string &help, ParameterList parameters,
					 const Type &returntype, std::tuple<Fns1_...> functions, std::tuple<Fns2_...> methods, P_ ...tags) : 
			Function(name, help, std::move(parameters), returntype, tags...)
			{ 
				this->functions=new fnstorageimpl<Fns1_...>(*this, functions);
				initmethods(methods);
				
				static_assert(!std::is_same<typename TMP::FunctionTraits<typename std::tuple_element<0, std::tuple<Fns1_...>>::type>::ReturnType, void>::value, 
							  "The given function does not return a value, however, a type has been specified as return type.");
				
				assert(this->HasMethod()==(bool)method && "MethodTag and method parameter must match");
			}
			
			/// @cond INTERNAL
			template<class ...P_, class ...Fns1_, class ...Fns2_>
			MappedFunction(const std::string &name, const std::string &help, ParameterList parameters,
						   std::tuple<Fns1_...> functions, std::tuple<Fns2_...> methods, P_ ...tags) : 
			Function(name, help, std::move(parameters), tags...)
			{ 
				this->functions=new fnstorageimpl<Fns1_...>(*this, functions);
				initmethods(methods);
				
				static_assert(std::is_same<typename TMP::FunctionTraits<typename std::tuple_element<0, std::tuple<Fns1_...>>::type>::ReturnType, void>::value, 
							  "The given function returns a value, however, a type has not been specified as return type.");
				
				assert(this->HasMethod()==(bool)method && "MethodTag and method parameter must match");
			}
			/// @endcond
			
			/// Destructor
			~MappedFunction() {
				delete functions;
				delete methods;
			}
			
			virtual Data Call(bool ismethod, const std::vector<Data> &parameters) const override {
				if(ismethod) {
					if(methods) {
						methods->call(parameters);
					}
					else {
						auto ret = functions->call(parameters);
						VirtualMachine::Get().GetOutput()<<ret;
					}
					
					return Data::Invalid();
				}
				else {
					return functions->call(parameters);
				}
			}
			
		private:
			
			
			/// Implementation for this function
			fnstorage *functions;
			
			/// Implementation for the method variant of this function
			fnstorage *methods=nullptr;
		};
		
	}
}