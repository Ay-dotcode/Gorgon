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
			MissingParameter,
			TooManyParameters,
			NoReturn,
			CastError,
			InstructionError
		};
		
		DefineEnumStrings(ExceptionType,
			{ExceptionType::Parse, "Parse error"},
			{ExceptionType::OutofBounds, "Index out of bounds"},
			{ExceptionType::AmbiguousSymbol, "Ambiguous symbol"},
			{ExceptionType::SymbolNotFound, "Symbol not found"},
			{ExceptionType::NullValue, "Value is null"},
			{ExceptionType::UnexpectedKeyword, "Unexpected keyword"},
			{ExceptionType::FlowError, "Flow error"},
			{ExceptionType::MissingParameter, "Missing parameter"},
			{ExceptionType::TooManyParameters, "Too many parameters"},
			{ExceptionType::NoReturn, "No return type"},
			{ExceptionType::CastError, "Cast error"},
			{ExceptionType::InstructionError, "Instruction error"}
		);
		
		enum class SymbolType {
			Type,
			Variable,
			Constant,
			ConstantOrVariable,
			Function,
			Operator,
			Namespace,
			Library,
			Unknown
		};
		
		DefineEnumStrings(SymbolType,
			{SymbolType::Type, "Type"},
			{SymbolType::Variable, "Variable"},
			{SymbolType::Constant, "Constant"},
			{SymbolType::ConstantOrVariable, "Constant or variable"},
			{SymbolType::Function, "Function"},
			{SymbolType::Operator, "Operator"},
			{SymbolType::Namespace, "Namespace"},
			{SymbolType::Library, "Library"},
			{SymbolType::Unknown, "Unknown"}
		);
		
		class Exception : public std::runtime_error {
		public:
			
		explicit Exception(ExceptionType type, const std::string &message, long linenumber=-1) : linenumber(linenumber), 
			std::runtime_error(message),
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

		long GetLine() const { return linenumber; }

		void SetLine(long line) {
			linenumber=line;
		}



		protected:
			ExceptionType type;
			std::string details;
			long linenumber;
		};
		
		class OutofBoundsException : public Exception {
		public:
			explicit OutofBoundsException(long index, long max, const std::string &objectype, const std::string &details="", long linenumber=-1) :
			Exception(ExceptionType::OutofBounds, objectype+" index "+String::From(index)+" is out of bounds."
					  "should be [0, "+String::From(max)+").", linenumber) { 
				this->details = details;
			}
		}; 
		
		class AmbiguousSymbolException : public Exception {
		public:
			explicit AmbiguousSymbolException(const std::string &symbolname, SymbolType type, const std::string &details="", long linenumber=-1) :
			Exception(ExceptionType::AmbiguousSymbol, String::From(type) + " " + symbolname + " is ambiguous.", linenumber) { 
				this->details = details;
			}
		};
		
		class SymbolNotFoundException : public Exception {
		public:
			explicit SymbolNotFoundException(const std::string &symbolname, SymbolType type, const std::string &details="", long linenumber=-1) :
			Exception(ExceptionType::SymbolNotFound, "Cannot find " + String::ToLower(String::From(type)) + " " + symbolname, linenumber) { 
				this->details = details;
			}
		};
		
		class NullValueException : public Exception {
		public:
			explicit NullValueException(const std::string &symbolname, const std::string &details="", long linenumber=-1) :
			Exception(ExceptionType::NullValue, "The value of " + symbolname + " is NULL", linenumber) { 
				this->details = details;
			}
		};
		
		class UnexpectedKeywordException : public Exception {
		public:
			explicit UnexpectedKeywordException(const std::string &keyword, const std::string &details="", long linenumber=-1) :
			Exception(ExceptionType::UnexpectedKeyword, "The keyword " + keyword + " is not expected", linenumber) { 
				this->details = details;
			}
		};

		class FlowException : public Exception {
		public:
			explicit FlowException(const std::string &message, const std::string &details="", long linenumber=-1) :
			Exception(ExceptionType::FlowError, message, linenumber) {
				this->details = details;
			}
		};

		class MissingParameterException : public Exception {
		public:
			explicit MissingParameterException(const std::string &parameter, int index, const std::string &type, const std::string &details="", long linenumber=-1) :
				Exception(ExceptionType::MissingParameter,
				"Missing parameter "+String::From(index+1)+", "+parameter+" ("+type+")", linenumber) {

				this->details = details;
			}
		};

		class TooManyParametersException : public Exception {
		public:
			explicit TooManyParametersException(int given, int total, const std::string &details="", long linenumber=-1) :
				Exception(ExceptionType::TooManyParameters,
				"Too many parameters, "+String::From(given)+" given, should be "+String::From(total), linenumber) {

				this->details = details;
			}
		};

		class NoReturnException : public Exception {
		public:
			explicit NoReturnException(const std::string &function, const std::string &details="", long linenumber=-1) :
				Exception(ExceptionType::NoReturn,
				"Function "+function+" does not return a value", linenumber) {

				this->details = details;
			}
		};

		class CastException : public Exception {
		public:
			explicit CastException(const std::string &from, const std::string &to, const std::string &details="", long linenumber=-1) :
				Exception(ExceptionType::CastError,
				"Cannot cast from "+from+" to "+to, linenumber) {

				this->details = details;
			}
		};
		
		class InstructionException : public Exception {
		public:
			explicit InstructionException(const std::string& message, const std::string &details="", long int linenumber = -1) :
			Exception(ExceptionType::InstructionError, message, linenumber) {
				this->details=details;
			}
		};



	}
}