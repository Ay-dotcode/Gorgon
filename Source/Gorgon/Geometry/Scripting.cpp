#include "Point.h"
#include "../Scripting/Embedding.h"
#include "../Scripting/Reflection.h"
#include "../Scripting.h"

namespace Gorgon { namespace Geometry {



	Scripting::Library LibGeometry("Geometry",
								   "Data types under geometry module and their member functions and operators");

	void init_scripting() { 
		if(LibGeometry.Members.GetCount()) return;

		auto point = new Scripting::MappedValueType<Point>(
			"Point",
			"Basic class for representing a point in space."
			"The class is composed of two fields, x and y of type int."
			"See documentation for complete list of overloaded operators and member functions"
		);

		point->AddMembers({
			new Scripting::MappedOperator(
				"+",
				"Adds two point type to each other, returns a new resultant point",
				point, point, point, 
				[] (Point lhs, Point rhs) { return lhs + rhs; }
			),
            
            new Scripting::MappedOperator(
                "-",
                "Subtracts one point from another, returns a new resultant point",
                point,point,point,
                [](Point lhs, Point rhs){return lhs - rhs;}
            ),
            
			new Scripting::Function("Distance",
				"Returns the distance between two Points", point,
				{
					Scripting::MapFunction(
						[] (Point owner, Point other) { 
							return owner.Distance(other); 
						}, Scripting::Types::Float(),
						{
							Scripting::Parameter("Point", "Another point to calculate distance in between", point)
						},
						Scripting::ConstTag
					),
					Scripting::MapFunction(
						[] (Point owner) { 
							return owner.Distance(); 
						}, Scripting::Types::Float(),
						{ },
						Scripting::ConstTag
					),
				}
			),
            
            new Scripting::Function("Slope",
                "Returns the slope of the point (Gradient)", point,
                {
                    Scripting::MapFunction(
                        [](Point owner){
                            return owner.Slope();
                        },Scripting::Types::Float(),
                        { },
                        Scripting::ConstTag
                    )
                }
            )
            
		});
        
		point->AddConstructors({
			Scripting::MapFunction([]{ return Point(0,0); }, point, {})
		});
		point->MapConstructor<int, int>({
			Scripting::Parameter("x", "X coordinate", Scripting::Types::Int()),
			Scripting::Parameter("x", "Y coordinate", Scripting::Types::Int())
		});

		point->AddMembers({
			new Scripting::MappedInstanceMember<Point, int>(&Point::X, "x", "X coordinate", Scripting::Types::Int()),
			new Scripting::MappedInstanceMember<Point, int>(&Point::Y, "y", "Y coordinate", Scripting::Types::Int())
		});
        
		LibGeometry.AddMember(point);
	}
} }
