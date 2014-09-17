#pragma once

#include <tuple>

#include "Reflection.h"
#include "../Scripting.h"
#include "../TMP.h"


namespace Gorgon {
	
	namespace Scripting {
		
		/// This is the function type definition for function stubs
		typedef std::function<Data(const std::vector<Data>&)> FunctionStub;
		
		/// This is the function type definition for method stubs
		typedef std::function<void(const std::vector<Data>&)> MethodStub;
		
		/// Using this value instead of method stub will tell the system that this function does not
		/// have a method.
		static const MethodStub NoMethodVariant = {};
		
		/// This class is used to create linking to an embedded function. You may use MakeStub
		/// to create stub function that handles parameter unboxing
		class EmbeddedFunction : public Function {
		public:
			
			/// Constructor, returntype and tags are optional, if this function has no method variant
			/// NoMethodVariant should be passed to method parameter.
			template<class ...P_>
			EmbeddedFunction(const std::string &name, const std::string &help, ParameterList parameters,
					 const Type &returntype, FunctionStub function, MethodStub method, P_ ...tags) : 
			Function(name, help, std::move(parameters), returntype, tags...), function(function), method(method)
			{ 
				assert(this->HasMethod()==(bool)method && "MethodTag and method parameter must match");
			}
			
			/// @cond INTERNAL
			template<class ...P_>
			EmbeddedFunction(const std::string &name, const std::string &help, ParameterList parameters,
					 FunctionStub function, MethodStub method, Tag firsttag, P_ ...tags) : 
			Function(name, help, std::move(parameters), firsttag, tags...), function(function), method(method)
			{ 
				assert(this->HasMethod()==(bool)method && "MethodTag and method parameter must match");
			}
			
			EmbeddedFunction(const std::string &name, const std::string &help, ParameterList parameters, 
							 FunctionStub function) : 
			Function(name, help, std::move(parameters)), function(function)
			{ 
				assert(this->HasMethod()==(bool)method && "MethodTag and method parameter must match");
			}
			/// @endcond
			
			virtual Data Call(bool ismethod, const std::vector<Data> &parameters) const override;
			
		private:
			/// Implementation for this function
			FunctionStub function;
			
			/// Implementation for the method variant of this function
			MethodStub method;
		};
		
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
				fnstorageimpl(std::tuple<Fns_...> fns) : fns(fns) { }
				
				static const int maxarity=TMP::FunctionTraits< typename std::tuple_element<0, std::tuple<Fns_...>> >::Arity;
				
				template<int num>
				typename TMP::FunctionTraits<typename std::tuple_element<0, std::tuple<Fns_...> >::type>::template Arguments<num>::Type 
				castparam(Data data) const {
					return data.GetValue<typename TMP::FunctionTraits<typename std::tuple_element<0, std::tuple<Fns_...> >::type>::template Arguments<num>::Type>();
				}
				
				template<class R_, int variant, int ...S>
				typename std::enable_if<std::is_same<R_, void>::value, Data>::type 
				callfn(TMP::Sequence<S...>, const std::vector<Data> &data) const {
					auto fn = std::get<variant>(fns);
					
					fn(castparam<S>(data[S])...);
					
					return Data::Invalid();
				}
				
				template<int variant>
				Data callvariant(const std::vector<Data> &data) const {
					auto fn = std::get<variant>(fns);
					
					typedef TMP::FunctionTraits<decltype(fn)> traits;
					
					return callfn<typename traits::ReturnType, variant>(typename TMP::Generate<traits::Arity>::Type(), data);
				}
				
				template<int ...variants>
				Data expandvariants(TMP::Sequence<variants...>, int variant, const std::vector<Data> &data) const {
					typedef Data(fnstorageimpl::*calltype)(const std::vector<Data>&) const;
					static calltype list[]={&fnstorageimpl::callvariant<variants>...};
					return (this->*list[variant])(data);
				}
				
				virtual Data call(const std::vector<Data> &data) const override {
					return expandvariants(typename TMP::Generate<sizeof...(Fns_)>::Type(), TMP::FunctionTraits<typename std::tuple_element<0, std::tuple<Fns_...> >::type>::Arity-data.size(), data);
				}
				
				virtual bool isempty() const override {
					return sizeof...(Fns_)==0;
				}
				
				std::tuple<Fns_...> fns;
			};
			
			void initmethods(std::tuple<> methods) {
			}
			
			template<class ...T_>
			void initmethods(std::tuple<T_...> methods) {
				this->methods  =new fnstorageimpl<T_...>(methods);
			}
			/// @endcond
			
		public:
			
			/// Constructor, returntype and tags are optional, 
			template<class ...P_, class ...Fns1_, class ...Fns2_>
			MappedFunction(const std::string &name, const std::string &help, ParameterList parameters,
					 const Type &returntype, std::tuple<Fns1_...> functions, std::tuple<Fns2_...> methods, P_ ...tags) : 
			Function(name, help, std::move(parameters), returntype, tags...)
			{ 
				this->functions=new fnstorageimpl<Fns1_...>(functions);
				initmethods(methods);
				
				//static_assert(!std::is_same<Ret_, void>::value, "Return type cannot be given for a void function");
				assert(this->HasMethod()==(bool)method && "MethodTag and method parameter must match");
				//assert(sizeof...(Params_) == this->parameters.GetCount() && "Number of actual parameters are different than the parameters given");
			}
			
			/// @cond INTERNAL
			template<class ...P_, class ...Fns1_, class ...Fns2_>
			MappedFunction(const std::string &name, const std::string &help, ParameterList parameters,
						   std::tuple<Fns1_...> functions, std::tuple<Fns2_...> methods, P_ ...tags) : 
			Function(name, help, std::move(parameters), tags...)
			{ 
				this->functions=new fnstorageimpl<Fns1_...>(functions);
				initmethods(methods);
				
				//static_assert(std::is_same<Ret_, void>::value, "Return type should be given for a non-void function");
				assert(this->HasMethod()==(bool)method && "MethodTag and method parameter must match");
				//assert(sizeof...(Params_) == this->parameters.GetCount() && "Number of actual parameters are different than the parameters given");
			}
			/// @endcond
			
			/// Destructor
			~MappedFunction() {
				delete functions;
				delete methods;
			}
			
			virtual Data Call(bool ismethod, const std::vector<Data> &parameters) const override {
				if(ismethod) {
					if(!methods) {
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