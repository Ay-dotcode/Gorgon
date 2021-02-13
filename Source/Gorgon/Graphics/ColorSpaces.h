#pragma once

#include "Color.h"

namespace Gorgon { namespace Graphics {
    
    
    
    /**
    * Represents XYZ color space. This space is generally used to convert RGB from and to
    * other color channels
    */
    class XYZAf {
    public:
        /// Data type for each channel
        typedef float ChannelType;


        /// Default constructor zero initializes the color except for alpha which is set to 1.
        XYZAf() { }
        
        /// Fills all channels
        XYZAf(float x, float y, float z, float a = 1.f) : 
            X(x), Y(y),
            Z(z), A(a)
        { }
        
        XYZAf(RGBAf other);
        
        operator RGBAf() const;
        
        /// X color channel
        float X = 0.0f;
        
        /// Y color channel
        float Y = 0.0f;
        
        /// Z color channel
        float Z = 0.0f;
        
        /// Alpha color channel
        float A = 0.0f;
    };

    inline std::ostream &operator <<(std::ostream &out, const XYZAf &color) {
        out << "XYZ(" << color.X << ", " << color.Y << ", " << color.Z << ")";
        
        return out;
    }

    /**
    * CIE Luv color system. Device independent and linear in terms of lightness.
    */
    class LuvAf {
    public:
        /// Data type for each channel
        typedef float ChannelType;


        /// Default constructor zero initializes the color except for alpha which is set to 1.
        LuvAf() { }
        
        /// Fills all channels
        LuvAf(float l, float u, float v, float a = 1.f) : 
            L(l), u(u),
            v(v), A(a)
        { }
        
        LuvAf(XYZAf other);
        
        LuvAf(RGBAf other);
        
        operator XYZAf() const;
        
        operator RGBAf() const;
        
        /// Luminance channel
        float L = 0.0f;
        
        /// u color channel
        float u = 0.0f;
        
        /// v color channel
        float v = 0.0f;
        
        /// Alpha color channel
        float A = 0.0f;
    };

    inline std::ostream &operator <<(std::ostream &out, const LuvAf &color) {
        out << "Luv(" << color.L << ", " << color.u << ", " << color.v << ")";
        
        return out;
    }

    /**
    * CIE Lab color system. Device independent and linear in terms of lightness.
    */
    class LabAf {
    public:
        /// Data type for each channel
        typedef float ChannelType;


        /// Defaalt constractor zero initializes the color except for alpha which is set to 1.
        LabAf() { }
        
        /// Fills all channels
        LabAf(float l, float a, float b, float alpha = 1.f) : 
            L(l), a(a),
            b(b), A(alpha)
        { }
        
        LabAf(XYZAf other);
        
        LabAf(RGBAf other);
        
        operator XYZAf() const;
        
        operator RGBAf() const;
        
        /// Luminance channel
        float L = 0.0f;
        
        /// a color channel
        float a = 0.0f;
        
        /// b color channel
        float b = 0.0f;
        
        /// Alpha color channel
        float A = 0.0f;
    };

    inline std::ostream &operator <<(std::ostream &out, const LabAf &color) {
        out << "Lab(" << color.L << ", " << color.a << ", " << color.b << ")";
        
        return out;
    }

    /**
    * CIE LCh(uv) color system. Circular color system to select colors using luminance, chromacity
    * (saturation) and hue. Far better than HSL or HSV color system. Linear and hue does not effect
    * perceived lightness.
    */
    class LChAf {
    public:
        /// Data type for each channel
        typedef float ChannelType;


        /// Default constructor zero initializes the color except for alpha which is set to 1.
        LChAf() { }
        
        /// Fills all channels
        LChAf(float l, float c, float h, float a = 1.f) : 
            L(l), C(c),
            h(h), A(a)
        { }
        
        LChAf(LuvAf other);
        
        LChAf(XYZAf other);
        
        LChAf(RGBAf other);
        
        operator LuvAf() const;
        
        operator XYZAf() const;
        
        operator RGBAf() const;
        
        /// Luminance color channel
        float L = 0.0f;
        
        /// Chromacity channel
        float C = 0.0f;
        
        /// hue channel
        float h = 0.0f;
        
        /// Alpha color channel
        float A = 0.0f;
    };

    inline std::ostream &operator <<(std::ostream &out, const LChAf &color) {
        out << "LCh(" << color.L << ", " << color.C << ", " << color.h << "°)";
        
        return out;
    }

    /**
    * CIE LCh(ab) color system. Circular color system to select colors using luminance, chromacity
    * (saturation) and hue. Far better than HSL or HSV color system. Linear and hue does not effect
    * perceived lightness.
    */
    class LChAf_ab {
    public:
        /// Data type for each channel
        typedef float ChannelType;


        /// Default constructor zero initializes the color except for alpha which is set to 1.
        LChAf_ab() { }
        
        /// Fills all channels
        LChAf_ab(float l, float c, float h, float a = 1.f) : 
            L(l), C(c),
            h(h), A(a)
        { }
        
        LChAf_ab(LabAf other);
        
        LChAf_ab(XYZAf other);
        
        LChAf_ab(RGBAf other);
        
        operator LabAf() const;
        
        operator XYZAf() const;
        
        operator RGBAf() const;
        
        /// Luminance color channel
        float L = 0.0f;
        
        /// Chromacity channel
        float C = 0.0f;
        
        /// hue channel
        float h = 0.0f;
        
        /// Alpha color channel
        float A = 0.0f;
    };

    inline std::ostream &operator <<(std::ostream &out, const LChAf_ab &color) {
        out << "LCh_ab(" << color.L << ", " << color.C << ", " << color.h << "°)";
        
        return out;
    }

    /// Converts a color in RGB color space to XYZ color space.
    XYZAf RGBToXYZ(RGBAf color);
    
    /// Converts a color in XYZ color space to RGB color space..
    RGBAf XYZToRGB(XYZAf color, bool unbounded = false);
    
    /// Converts a color in XYZ color space to CIE Luv color space.
    LuvAf XYZToLuv(XYZAf color);
    
    /// Converts a color in CIE Luv color space to XYZ color space.
    XYZAf LuvToXYZ(LuvAf color);
    
    /// Converts a color in XYZ color space to CIE Lab color space.
    LabAf XYZToLab(XYZAf color);
    
    /// Converts a color in CIE Lab color space to XYZ color space.
    XYZAf LabToXYZ(LabAf color);
    
    /// Converts a color in CIE Luv color space to CIE LCh_uv color space.
    LChAf LuvToLCh(LuvAf color);
    
    /// Converts a color in CIE LCh_uv color space to CUE Luv color space.
    LuvAf LChToLuv(LChAf color);
    
    /// Converts a color in CIE Lab color space to CIE LCh_ab color space.
    LChAf_ab LabToLCh(LabAf color);
    
    /// Converts a color in CIE LCh_ab color space to CUE Lab color space.
    LabAf LChToLab(LChAf_ab color);
    
} }
