#include "Point.h"
#include "../Scripting/Embedding.h"
#include "../Scripting/Reflection.h"
#include "../Scripting.h"

namespace Gorgon { namespace Geometry {



	Scripting::Library LibGeometry;

	void init_scripting() { 

		auto point = new Scripting::MappedValueType<Point>(
			"Point",
			"Basic class for representing a point in space."
			"The class is composed of two fields, x and y of type int."
			"See documentation for complete list of overloaded operators and member functions"
		);

		point->AddFunctions({
			new Scripting::MappedOperator(
				"+",
				"Adds two point type to each other, returns a new resultant point",
				point, point, point, 
				[] (Point lhs, Point rhs) { return lhs + rhs; }
			),
			new Scripting::MappedFunction(
				"Distance",
				"Returns the distance between two Points",
				Scripting::Integrals.Types["Float"], point,
				Scripting::ParameterList{
					new Scripting::Parameter(
						"Point",
						"Another point to calculate distance in between",
						point,
						Scripting::OptionalTag
					),
				},	
				Scripting::MappedFunctions(
					[] (Point owner, Point other) { return owner.Distance(other); },
					[] (Point owner) { return owner.Distance(); }
				),
				Scripting::MappedMethods()
			)
		});

		point->AddDataMembers({
			new Scripting::MappedData<Point, int>(&Point::X, "x", "field storing location on x coordinate", Scripting::Integrals.Types["Int"]),
			new Scripting::MappedData<Point, int>(&Point::Y, "y", "field storing location on y coordinate", Scripting::Integrals.Types["Int"])
		});

		LibGeometry = {
			"Geometry",
			"Data types under geometry module and their member functions and operators",
			Scripting::TypeList {
				point,
			},
			Scripting::FunctionList{},
			Scripting::ConstantList{},
		};
	}
} }