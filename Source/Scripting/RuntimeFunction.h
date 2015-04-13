
/// @file RuntimeFunction.h This file contains classes that supports functions that are defined in runtime

#pragma once

#include <ostream>
#include <string>


#include "Reflection.h"
#include "Data.h"
#include "Input.h"
#include "Scope.h"
#include "VirtualMachine.h"


namespace Gorgon { namespace Scripting {
		
	class RuntimeOverload : public Function::Overload {
		RuntimeOverload(
			Scope &parent,
			const Type* returntype, ParameterList parameters, 
			bool stretchlast, bool repeatlast, 
			bool accessible, bool constant, 
			bool returnsref, bool returnsconst, 
			bool implicit
		) : 
		  scope(parent), 
		  Function::Overload(
			returntype, parameters, stretchlast, repeatlast, accessible,
			constant, returnsref, returnsconst, implicit
		  ) 
		{ }
		
		Scope &GetParentScope() {
			scope.GetParent();
		}
		
		void SaveInstruction(Instruction inst, long pline) {
			scope.SaveInstruction(inst, pline);
		}
		
		ScopeInstance &Instantiate(ScopeInstance &current) {
			scope.Instantiate(current);
		}
		
		virtual Data Call(bool ismethod, const std::vector<Data> &parameters) const override {
			auto &scope=Instantiate();
			VirtualMachine::Get().Run();
			
			return Data::Invalid();
		}
		
	private:
		SourceMarker definedin;
		Scope scope;
	};
		
} }
