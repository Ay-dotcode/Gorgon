#pragma once

#include "../Geometry/Margin.h"
#include "../Geometry/Size.h"
#include "../Geometry/Bounds.h"

namespace Gorgon { namespace UI {
    /// Dimension data for components. Allows relative position and sizing.
    class Dimension {
    public:

        /// Unit for dimensions. Dimensions in UI system does not allow floating
        /// point numbers as floating point numbers are not precise and may cause
        /// issues. Additionally, final values always rounded, so that the symbols
        /// are always on full pixels.
        enum Unit {
            /// Fixed pixel based dimensions
            Pixel,

            /// Dimension will be relative to the parent and given in percent.
            /// If higher resolution is necessary use BasisPoint.
            Percent,

            /// 1/1000th of a pixel, there are only few places that this will be used.
            /// Currently only rotation center use non-integer pixels
            MilliPixel,

            /// Dimension will be relative to the parent and given in 1/10000.
            BasisPoint,

            /// Dimension will be relative to the text size, given value is the
            /// percent of the width of an EM dash. If no font information is 
            /// found, 10px will be used for EM dash. Thus, 1 unit will be 0.1
            /// pixels.
            EM,
            
            //todo add line height
        };

        /// Constructs a new dimension or type casts integer to dimension		 
        Dimension(int value = 0, Unit unit = Pixel) : value(value), unit(unit) {/* implicit */
        }

        /// Returns the calculated dimension in pixels
        int operator ()(int parentwidth, int emwidth = 10) const {
            return Calculate(parentwidth, emwidth);
        }

        /// Returns the calculated dimension in pixels
        int Calculate(int parentwidth, int emwidth = 10) const {
            switch(unit) {
                case Percent:
                    return int(std::round((double)value * parentwidth / 100));
                case MilliPixel:
                    return int(std::round((double)value / 1000));
                case BasisPoint:
                    return int(std::round((double)value * parentwidth / 10000));
                case EM:
                    return int(std::round(value * emwidth / 100));
                case Pixel:
                default:
                    return value;
            }
        }

        /// Returns the calculated dimension in pixels
        float CalculateFloat(float parentwidth, float emwidth = 10) const {
            switch(unit) {
                case Percent:
                    return (float)value * parentwidth / 100.f;
                case BasisPoint:
                    return (float)value * parentwidth / 10000.f;
                case MilliPixel:
                    return (float)value / 1000;
                case EM:
                    return (float)value * emwidth / 100.f;
                case Pixel:
                default:
                    return (float)value;
            }
        }

        /// Returns the value of the dimension, should not be considered as
        /// pixels
        int GetValue() const {
            return value;
        }

        /// Returns the unit of the dimension
        Unit GetUnit() const {
            return unit;
        }

        /// Changes the value of the dimension without modifying the units
        void Set(int value) {
            this->value = value;
        }

        /// Changes the value and unit of the dimension.
        void Set(int value, Unit unit) {
            this->value = value;
            this->unit = unit;
        }

    private:
        int value;
        Unit unit;
    };

    /// This class stores the location information for a box object
    using Point = Geometry::basic_Point<Dimension>;

    /// This class stores the size information for a box object
    using Size = Geometry::basic_Size<Dimension>;

    /// This class stores the margin information for a box object
    using Margin = Geometry::basic_Margin<Dimension>;
    
    /// Converts a dimension based point to pixel based point
    inline Geometry::Point Convert(const Point &p, const Geometry::Size &parent, int emwidth = 10) {
        return {p.X(parent.Width, emwidth), p.Y(parent.Height, emwidth)};
    }
    
    /// Converts a dimension based size to pixel based size
    inline Geometry::Size Convert(const Size &s, const Geometry::Size &parent, int emwidth = 10) {
        return {s.Width(parent.Width, emwidth), s.Height(parent.Height, emwidth)};
    }
    
    /// Converts a dimension based margin to pixel based margin
    inline Geometry::Margin Convert(const Margin &m, const Geometry::Size &parent, int emwidth = 10) {
        return {m.Left(parent.Width, emwidth), m.Top(parent.Height, emwidth), m.Right(parent.Width, emwidth), m.Bottom(parent.Height, emwidth), };
    }

} }
