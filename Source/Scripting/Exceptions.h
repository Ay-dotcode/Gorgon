#pragma once

#include <string>
#include <stdexcept>

#include "../Enum.h"

namespace Gorgon {
	namespace Scripting {
		
		enum class ExceptionType {
			Parse,
			OutofBounds,
			AmbiguousSymbol,
			SymbolNotFound,
			NullValue,
			UnexpectedKeyword,
			FlowError,
		};
		
		DefineEnumStrings(ExceptionType,
			{ExceptionType::Parse, "Parse error"},
			{ExceptionType::OutofBounds, "Index out of bounds"},
			{ExceptionType::AmbiguousSymbol, "Ambiguous symbol"},
			{ExceptionType::SymbolNotFound, "Symbol not found"},
			{ExceptionType::NullValue, "Value is null"},
			{ExceptionType::UnexpectedKeyword, "Unexpected keyword"},
			{ExceptionType::FlowError, "Flow error"}
		);
		
		enum class SymbolType {
			Type,
			Variable,
			Constant,
			ConstantOrVariable,
			Function,
			Operator,
			Namespace
		};
		
		DefineEnumStrings(SymbolType,
			{SymbolType::Type, "Type"},
			{SymbolType::Variable, "Variable"},
			{SymbolType::Constant, "Constant"},
			{SymbolType::ConstantOrVariable, "Constant or variable"},
			{SymbolType::Function, "Function"},
			{SymbolType::Operator, "Operator"},
			{SymbolType::Namespace, "Namespace"}
		);
		
		class Exception : public std::runtime_error {
		public:
			
		explicit Exception(ExceptionType type, const std::string &message) : std::runtime_error(message),
		type(type) { }
		
		ExceptionType GetType() const {
			return type;
		}
		
		virtual std::string GetMessage() const {
			return what();
		}
		
		virtual std::string GetDetails() const {
			if(details=="") {
				return what();
			}
			else {
				return details;
			}
		}
			
		protected:
			ExceptionType type;
			std::string details;
		};
		
		class AmbiguousSymbolException : public Exception {
		public:
			explicit AmbiguousSymbolException(const std::string &symbolname, SymbolType type, const std::string &details="") : 
			Exception(ExceptionType::AmbiguousSymbol, String::From(type) + " " + symbolname + " is ambiguous.") { 
				this->details = details;
			}
		};
		
		class SymbolNotFoundException : public Exception {
		public:
			explicit SymbolNotFoundException(const std::string &symbolname, SymbolType type, const std::string &details="") : 
			Exception(ExceptionType::SymbolNotFound, "Cannot find " + String::ToLower(String::From(type)) + " " + symbolname) { 
				this->details = details;
			}
		};
		
		class NullValueException : public Exception {
		public:
			explicit NullValueException(const std::string &symbolname, const std::string &details="") : 
			Exception(ExceptionType::NullValue, "The value of " + symbolname + " is NULL") { 
				this->details = details;
			}
		};
		
		class UnexpectedKeywordException : public Exception {
		public:
			explicit UnexpectedKeywordException(const std::string &keyword, const std::string &details="") : 
			Exception(ExceptionType::UnexpectedKeyword, "The keyword " + keyword + " is not expected") { 
				this->details = details;
			}
		};
		
		class FlowException : public Exception {
		public:
			explicit FlowException(const std::string &message, const std::string &details="") : 
			Exception(ExceptionType::FlowError, message) { 
				this->details = details;
			}
		};
		
	}
}