#pragma once

#include <tuple>
#include <type_traits>

#include "Reflection.h"
#include "VirtualMachine.h"
#include "../Scripting.h"
#include "../TMP.h"

#include "../Utils/Assert.h"


namespace Gorgon {
	
	namespace Scripting {
		
		/**
		 * This class allows embedded types to become scripting types that are passed around
		 * as values. This class requires T_ to be copy constructable.
		 */
		template <
			class T_, 
			std::string(*ToString_)(const T_ &)=&String::From<T_>, 
			T_(*Parse_)(const std::string &)=&String::To<T_>
		>
		class MappedValueType : public Type {
		public:
			MappedValueType(const std::string &name, const std::string &help, const T_ &def) :
			Type(name, help, def, false)
			{
			}
			
			MappedValueType(const std::string &name, const std::string &help) : MappedValueType(name, help, T_()) {
			}
			
			
			/// Converts a data of this type to string. This function should never throw, if there is
			/// no data to display, recommended this play is either [ EMPTY ], or Typename #id
			virtual std::string ToString(const Data &data) const override {
				return ToString_(data.GetValue<T_>());
			}
			
			/// Parses a string into this data. This function is allowed to throw.
			virtual Data Parse(const std::string &str) const override {
				return Data(this, Parse_(str));
			}			
		};
		
		template<class T_>
		T_ ParseThrow(const std::string &) { throw std::runtime_error("This type cannot be parsed."); }
		
		/**
		 * This class allows embedded types to become scripting types that are passed around
		 * as references. Parsing requires a pointer, therefore, a regular parse function will not
		 * be sufficient. Default parsing function throws.
		 */
		template <
		class T_, 
		std::string(*ToString_)(const T_ &)=&String::From<T_>, 
		T_*(*Parse_)(const std::string &)=&ParseThrow<T_*>
		>
		class MappedReferenceType : public Type {
		public:
			MappedReferenceType(const std::string &name, const std::string &help, T_ *def) :
			Type(name, help, def, true)
			{
			}
			
			MappedReferenceType(const std::string &name, const std::string &help) : MappedReferenceType(name, help, nullptr) {
			}
			
			
			/// Converts a data of this type to string. This function should never throw, if there is
			/// no data to display, recommended this play is either [ EMPTY ], or Typename #id
			virtual std::string ToString(const Data &data) const override {
				return ToString_(*data.GetValue<T_*>());
			}
			
			/// Parses a string into this data. This function is allowed to throw.
			virtual Data Parse(const std::string &str) const override {
				return Data(this, Parse_(str));
			}			
		};
		
		Data GetVariableValue(const std::string &varname);
		extern MappedValueType<Data, String::From<Data>, GetVariableValue> Variant;
		
		/// Can be used to combine mapped functions.
		template <class ...T_>
		std::tuple<T_...> MappedFunctions(T_...args) {
			return std::make_tuple(args...);
		}
		
		/// Can be used to combine mapped methods
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
				using paramof = typename traitsof<level>::template Arguments<param>::Type;

				
				template<int level, int param>
				void checkfnparam() {
					static_assert(std::is_same<paramof<level, param>, paramof<0, param>>::value,
						"Function types do not match!"
					);
					
					if(parent.HasParent()) {
						if(param==0) {
							if(traitsof<0>::IsMember && !parent.GetParent().IsReferenceType()) {
								ASSERT( (parent.GetParent().GetDefaultValue().TypeCheck<typename std::remove_pointer<paramof<level, param>>::type>()) , "Function parameter type and designated type does not match.", 5, 2);
							}
							else {
								ASSERT((parent.GetParent().GetDefaultValue().TypeCheck<paramof<level, param>>()) , "Function parameter type and designated type does not match.", 5, 2);
							}
						}
						else if(parent.Parameters[param-1].IsReference()) {
							ASSERT((std::is_same<paramof<level, param>, std::string>::value) , "Function parameter type and designated type does not match, reference parameters are accessed as std::string variables", 5, 2);
						}
						else {
							ASSERT((parent.Parameters[param-1].GetType().GetDefaultValue().TypeCheck<paramof<level, param>>()) , "Function parameter type and designated type does not match.", 5, 2);
						}
					}
					else {
						if(param==0 && traitsof<0>::IsMember && !parent.Parameters[param].GetType().IsReferenceType()) {
							ASSERT((parent.Parameters[param].GetType().GetDefaultValue().TypeCheck<typename std::remove_pointer<paramof<level, param>>::type>()) , "Function parameter type and designated type does not match.", 5, 2);
						}
						else if(parent.Parameters[param].IsReference()) {
							ASSERT((std::is_same<paramof<level, param>, std::string>::value) , "Function parameter type and designated type does not match, reference parameters are accessed as std::string variables", 5, 2);
						}
						else {
							ASSERT((parent.Parameters[param].GetType().GetDefaultValue().TypeCheck<paramof<level, param>>()) , "Function parameter type and designated type does not match.", 5, 2);
						}
					}
				}
				
				template<int level, int ...S>
				void checkfnlevel(TMP::Sequence<S...>) {
					static_assert(traitsof<level>::Arity == traitsof<0>::Arity-level, 
								  "Number of function parameters does not match");
					
					static_assert(std::is_same<typename traitsof<level>::ReturnType,
								  typename traitsof<0>::ReturnType>::value,
								  "Return type of parameters does not match");
					
					char dummy[] = {0, (checkfnparam<level, S>(),'\0')...};
				}
				
				template<int ...S>
				void checkallfns(TMP::Sequence<S...>) {
					char dummy[]={0, (checkfnlevel<S>(typename TMP::Generate<traitsof<S>::Arity>::Type()),'\0')...};
				}

				fnstorageimpl(MappedFunction &parent, std::tuple<Fns_...> fns) : parent(parent), fns(fns) {
					
					static_assert(sizeof...(Fns_)<=traitsof<0>::Arity+1,
								  "Number of functions are more than possible");
					
					ASSERT( (traitsof<0>::Arity == parent.Parameters.GetCount() + parent.HasParent()),  
							String::Concat("Defined parameters (", parent.Parameters.GetCount(),
										   ") does not match the number of function parameters (", 
										   traitsof<0>::Arity - parent.HasParent(), ")") ,2,2);					
					
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
						//optional check
						if(param.IsOptional()) {
							optionalcount++;
							if(first) optionalatstart=true;
							if(passedfirstnonoptional) {
								ASSERT(!optionalatstart,
									String::Concat("Optional parameters should be at the start ",
									"or at the end, offender: ",param.GetName()),2,2);
							}
						}
						else {
							ASSERT(!shouldallbeoptional,
								String::Concat("All optional function parameters should be either at ",
								"the beginning or at the end, never at both sides, offender: ",
								param.GetName()),2,2);
							
							passedfirstnonoptional=true;
						}
						first=false;
					}
					
					ASSERT(optionalcount+1 >= sizeof...(Fns_), 
						   "Too many function definitions, might "
						   "be caused by a missing OptionalTag",2,2);
					
					ASSERT(optionalcount+1 <= sizeof...(Fns_),
						"Missing function definitions",2,2);
				}
				
				static const int maxarity=traitsof<0>::Arity;
				
				/// Casts the given data to the type of the num^{th} parameter. Parameter list is always taken from 
				/// first function
				template<int num>
				typename traitsof<0>::template Arguments<num>::Type 
				castparam(Data data) const {
					//check for data.GetType() to Parameters[?].GetType()
					
					if(parent.HasParent()) {
						if(num==0) {
							if(&data.GetType() != &parent.GetReturnType()) {
								throw std::runtime_error("Cannot convert $this to original parent type.");
							}
						}
						else {
							if(&data.GetType() != &parent.Parameters[num-1].GetType()) {
								throw std::runtime_error("Parameter and data type does not match, and there is no possible conversion.");
							}
						}
					}
					else {					
						if(&data.GetType() != &parent.Parameters[num].GetType()) {
							throw std::runtime_error("Parameter and data type does not match, and there is no possible conversion.");
						}
					}
					
					return data.GetValue<typename traitsof<0>::template Arguments<num>::Type>();
				}
				
				/// Casts the given data to the type of the num^{th} parameter. Parameter list is always taken from 
				/// first function. This one casts first parameter to non-pointer
				template<int num>
				typename TMP::Choose<num==0, typename traitsof<0>::template Arguments<num>::Type, 
				typename std::remove_pointer<typename traitsof<0>::template Arguments<num>::Type>::type>::Type
				castparam_firstnonptr(Data data) const {
					
					if(parent.HasParent()) {
						if(num==0) {
							if(&data.GetType() != &parent.GetReturnType()) {
								throw std::runtime_error("Cannot convert $this to original parent type.");
							}
						}
						else {
							if(&data.GetType() != &parent.Parameters[num-1].GetType()) {
								throw std::runtime_error("Parameter and data type does not match, and there is no possible conversion.");
							}
						}
					}
					else {					
						if(&data.GetType() != &parent.Parameters[num].GetType()) {
							throw std::runtime_error("Parameter and data type does not match, and there is no possible conversion.");
						}
					}
					
					return data.GetValue<typename TMP::Choose<num==0, typename traitsof<0>::template Arguments<num>::Type, 
					typename std::remove_pointer<typename traitsof<0>::template Arguments<num>::Type>::type>::Type>();
				}
				
				/// Calls the correct function variant. This function casts arguments while extracting them
				template<class R_, int variant, int ...S>
				typename std::enable_if<std::is_same<R_, void>::value && traitsof<0>::IsMember, Data>::type 
				callfn(TMP::Sequence<S...>, const std::vector<Data> &data) const {
					ASSERT(data.size()==sizeof...(S), String::Concat("Size of data (",
						data.size() ,") does not match to the number of parameters (",
						sizeof...(S),")!"), 5, 2);
					
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
					ASSERT(data.size()==sizeof...(S), String::Concat("Size of data (",
						data.size() ,") does not match to the number of parameters (",
						sizeof...(S),")!"), 5, 2);
					
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
					ASSERT(data.size()==sizeof...(S), String::Concat("Size of data (",
						data.size() ,") does not match to the number of parameters (",
						sizeof...(S),")!"), 5, 2);
					
					auto fn = std::get<variant>(fns);
					
					std::bind(fn, castparam<S>(data[S])...)();
					
					return Data::Invalid();
				}
				
				template<class R_, int variant, int ...S>
				typename std::enable_if<!std::is_same<R_, void>::value && !traitsof<0>::IsMember, Data>::type 
				callfn(TMP::Sequence<S...>, const std::vector<Data> &data) const {
					ASSERT(data.size()==sizeof...(S), String::Concat("Size of data (",
						data.size() ,") does not match to the number of parameters (",
						sizeof...(S),")!"), 5, 2);
					
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
					ASSERT(variant<sizeof...(Fns_) , "Missing parameter", 3, 2);
					typedef Data(fnstorageimpl::*calltype)(const std::vector<Data>&) const;
					static calltype list[]={&fnstorageimpl::callvariant<variants>...};
					return (this->*list[variant])(data);
				}
				
				virtual Data call(const std::vector<Data> &data) const override {					
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
				ASSERT(!HasMethod() , "Method implementation is missing", 2,2);
			}
			
			template<class ...T_>
			void initmethods(std::tuple<T_...> methods) {
				ASSERT(HasMethod() , "MethodTag is missing", 2,2);
				
				this->methods  =new fnstorageimpl<T_...>(*this, methods);
			}
			/// @endcond
			
		public:
			
			/// Constructor, returntype and parent could be nullptr, tags are optional. 
			template<class ...P_, class ...Fns1_, class ...Fns2_>
			MappedFunction(const std::string &name, const std::string &help, const Type *returntype, 
						   ParameterList parameters, const Type *parent, 
						   std::tuple<Fns1_...> functions, std::tuple<Fns2_...> methods, P_ ...tags) :  
			Function(name, help, returntype, std::move(parameters), parent, tags...)
			{ 
				this->functions=new fnstorageimpl<Fns1_...>(*this, functions);
				initmethods(methods);
				
				if(returntype) {
					using fn = typename fnstorageimpl<Fns1_...>::template traitsof<0>;
					ASSERT(returntype->GetDefaultValue().TypeCheck<typename fn::ReturnType>(), 
						"Function return type does not match with designated return type", 1, 2);
				}
				
				ASSERT(
					(
						std::is_same<typename fnstorageimpl<Fns1_...>::template traitsof<0>::ReturnType, void>::value == 
						(returntype==nullptr)
					),
					"The given function does not return a value, however, a type has been specified as return type.",
					1, 2
				);
				
				ASSERT((!IsOperator() || parent ) , "All operators should be a member function" , 1, 2);
				
				ASSERT(this->HasMethod()==(bool)method , "MethodTag and method parameter must match", 1, 2);
				overrideablechecks();
			}
			
			/// Destructor
			virtual ~MappedFunction() {
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
			
		protected:
			virtual void overrideablechecks() const {
				ASSERT(!IsScoped() , "Regular embedded functions cannot be scoped keywords", 2, 2);
			}
			
		private:
			
			
			/// Implementation for this function
			fnstorage *functions;
			
			/// Implementation for the method variant of this function
			fnstorage *methods=nullptr;
		};
		
		
		/**
		 * Scoped keyword helps to build scoped keywords from embedded functions.
		 * This class features an additional function, end to be added.
		 * 
		 * end function should return true if the scope really ends. In this case, the system will 
		 * clean up current scope. Single time execution scopes should always return true. 
		 * Additionally redirecting keywords should return true after direction is completed. Only 
		 * time this function retuns false is when the scope should be repeated again, like in loops. 
		 * 
		 * Functions should all return Data which contains the data that will be passed to end function.
		 */
		class ScopedKeyword : public MappedFunction {
		public:
			/** 
			 * Constructor, tags are optional.
			 */
			template<class ...P_, class ...Fns1_>
			ScopedKeyword(const std::string &name, const std::string &help, ParameterList parameters, 
						  std::tuple<Fns1_...> functions, std::function<bool(Data)> end, P_ ...tags) :  
			MappedFunction(name, help, &Variant, std::move(parameters), nullptr, ScopedTag, tags...),
			end(end)
			{ }
			
			virtual bool CallEnd(Data data) const override {
				return end(data);
			}
			
		protected:
			virtual void overrideablechecks() const override {
				ASSERT(!IsRedirecting() , "Regular scoped keyword functions cannot be redirecting", 2, 2);
			}
			
			/// The function that will be called at the end of the scope
			std::function<bool(Data)> end;
		};
		
		/**
		 * A redirecting keyword is given chance to read and modify the code inside its own scope.
		 * This is useful for keywords like enum, class or library where either contents of the scope
		 * requires additional parsing (enum) or the functions can be used inside the scope is limited.
		 * Enum like keywords should return "" after performing its own processing. Redirecting keywords
		 * should always allow end function to pass. Redirecting keywords are a type of scoped keywords,
		 * thus they also contain end function.
		 * 
		 * end function should return true if the scope really ends. In this case, the system will 
		 * clean up current scope. Single time execution scopes should always return true. 
		 * Additionally redirecting keywords should return true after direction is completed. Only 
		 * time this function retuns false is when the scope should be repeated again, like in loops. 
		 * 
		 * Functions should all return Data which contains the data that will be passed to end function.
		 * 
		 * While this scope is in effect, all the data that is read from the system will be passed to
		 * redirect function right after the entire logical line is obtained. redirect function should
		 * return a string that should be processed. This can empty the given string, leave it original
		 * or perform a transformation. If there are multiple redirecting scopes exists, the inner 
		 * scope is given priority. Comments are never redirected. 
		 */
		class RedirectingKeyword : public ScopedKeyword {
		public:
			/** 
			 * Constructor, tags are optional.
			 */
			template<class ...P_, class ...Fns1_>
			RedirectingKeyword(const std::string &name, const std::string &help, ParameterList parameters, 
							   std::tuple<Fns1_...> functions, std::function<bool(Data)> end, 
							   std::function<void(Data, std::string &)> redirect, P_ ...tags) :  
			ScopedKeyword(name, help, std::move(parameters), ScopedTag, end, tags...),
			redirect(redirect)
			{ }
			
			virtual void CallRedirect(Data data, std::string &line) const override { 
				redirect(data, line);
			}
			
		protected:
			virtual void overrideablechecks() const override { }
			
			/// The function that the code will be redirected to
			std::function<void(Data, std::string &)> redirect;
		};
		
		
		/**
		 * This class allows a one to one mapping of a data member to a c++ data member
		 */
		template<class C_, class T_>
		class MappedData : public DataMember {
		public:
			/// Constructor
			template<class ...P_>
			MappedData(T_ C_::*member, const std::string &name, const std::string &help, const Type *type, P_ ...tags) :
			DataMember(name, help, *type, tags...), member(member) {
				ASSERT(type, "Type cannot be nullptr", 1, 2);
			}
			
			virtual Data Get(const Data &data) const override {
				return {GetType(), data.GetValue<C_>().*member};
			}			
			
			/// Sets the data of the data member
			virtual void Set(Data &source, const Data &value) const override {
				C_ obj  = source.GetValue<C_>();
				obj.*member = value.GetValue <T_>();
				source={GetType(), obj};
			}
			
		private:
			T_ C_::*member;
		};
		
		/**
		 * This class allows a one to one mapping of a data member to a c++ data member
		 */
		template<class C_, class T_>
		class MappedData<C_*, T_> : public DataMember {
		public:
			/// Constructor
			template<class ...P_>
			MappedData(T_ C_::*member, const std::string &name, const std::string &help, const Type *type, P_ ...tags) :
			DataMember(name, help, *type, tags...), member(member) {
				ASSERT(type, "Type cannot be nullptr", 1, 2);
			}
			
			virtual Data Get(const Data &data) const override {
				return {GetType(), data.GetValue<C_*>()->*member};
			}			
			
			/// Sets the data of the data member
			virtual void Set(Data &source, const Data &value) const override {
				C_ *obj = source.GetValue<C_*>();
				obj->*member = value.GetValue <T_>();
			}			
		private:
			T_ C_::*member;
		};
		
	}
}