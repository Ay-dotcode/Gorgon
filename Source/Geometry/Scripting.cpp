#include "Point.h"
#include "../Scripting/Embedding.h"

namespace Gorgon { namespace Geometry {

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
				new Scripting::Parameter(
					"Point",
					"Another point to calculate distance in between",
					point
				),
				Scripting::MappedFunctions(
					[] (Point owner, Point other) { return owner.Distance(other); },
					[] (Point owner) { return owner.Distance(); }
				),
				Scripting::MappedMethods(),
				Scripting::OptionalTag
			)
		});
	}
} }