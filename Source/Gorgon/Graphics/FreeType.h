#pragma once

#include "Font.h"


namespace Gorgon { namespace Graphics {
    
    /// @cond internal
    struct ftlib;
    /// @endcond
    
    /**
     * This class allows using font files for text rendering. FreeType
     * supports unicode and allows glyphs to be created on the fly. It
     * is also possible to pre-load glyphs for performance reasons.
     * 
     * This class uses FreeType library to read fonts and requires 
     * FREETYPE to be set ON. 
     * 
     * 
     * Every instance of FreeType is separate and can be invoked
     * from multiple threads. However, preparing and creating atlas from
     * images require invokation from graphics thread.
     */
    class FreeType : public GlyphRenderer { 
        /// to be used internally.
        class GlyphDescriptor {
        public:
        };
        
    public:
        /// Initializes the class without loading any glyphs or files.
        /// Any calls to render or get size attempts will not perform
        /// any operation or returns 0. Use this constructor to font data
        /// from memory.
        FreeType();
        
        /// Initializes the class by loading the given file. After this step
        /// only generic information about the font file will be provided.
        /// You may continue loading by LoadMetrics function.
        FreeType(const std::string &filename) : FreeType() {
            LoadFile(filename);
        }
        
        /// Initializes the class by loading the given file.
        FreeType(const std::string &filename, int size, bool loadascii = true) : FreeType() {
            LoadFile(filename, size, loadascii);
        }
        
        /// Destructor
        virtual ~FreeType();
        
        /// Loads the given file. Unloads the previous file, if it exists.
        /// This operation will not unloaded loaded glyphs. Use Clear function
        /// to unload already loaded glyphs. After this step
        /// only generic information about the font file will be provided.
        /// You may continue loading by LoadMetrics function.
        bool LoadFile(const std::string &filename);
        
        /// Loads the given file. Unloads the previous file, if it exists.
        /// This operation will not unloaded loaded glyphs. Use Clear function
        /// to unload already loaded glyphs.
        bool LoadFile(const std::string &filename, int size, bool loadascii = true);
        
        /// Loads the given data. Unloads the previous file, if it exists.
        /// This operation will not unloaded loaded glyphs. Use Clear function
        /// to unload already loaded glyphs. After this step
        /// only generic information about the font file will be provided.
        /// You may continue loading by LoadMetrics function.
        bool Load(const std::vector<Byte> &data);
        
        /// Loads the given data. Unloads the previous file, if it exists.
        /// This operation will not unloaded loaded glyphs. Use Clear function
        /// to unload already loaded glyphs.
        bool Load(const std::vector<Byte> &data, int size, bool loadascii = true);
        
        /// Loads the given data. Unloads the previous file, if it exists.
        /// This operation will not unloaded loaded glyphs. Use Clear function
        /// to unload already loaded glyphs. After this step
        /// only generic information about the font file will be provided.
        /// You may continue loading by LoadMetrics function.
        bool Load(const Byte *data, int datasize);
        
        /// Loads the given data. Unloads the previous file, if it exists.
        /// This operation will not unloaded loaded glyphs. Use Clear function
        /// to unload already loaded glyphs.
        bool Load(const Byte *data, int datasize, int size, bool loadascii = true);
        
        /// Continues loading a file by setting font size and obtaining
        /// necessary information. If the given size is invalid, this
        /// function will return 0. Use GetPresetSizes function to get
        /// list of sizes that the font supports. You may also check
        /// IsScalable return to check whether using an arbitrary size is
        /// allowed.
        bool LoadMetrics(int size);
        
        /// Loads the glyphs in the given range. It is best to use single 
        /// LoadGlyphs function call for optimal character packing. However,
        /// it is possible to LoadGlyphs functions multiple times. If a
        /// glyph in the given range is already loaded, it will not be loaded
        /// again.
        bool LoadGlyphs(Glyph start, Glyph end);
        
        /// Loads the glyphs in the given range. It is best to use single 
        /// LoadGlyphs function call for optimal character packing. However,
        /// it is possible to LoadGlyphs functions multiple times. If a
        /// glyph in the given range is already loaded, it will not be loaded
        /// again. This overload will load all ranges given in the vector.
        bool LoadGlyphs(std::vector<std::pair<Glyph, Glyph>> ranges);
        
        /// Returns if the loaded font is scalable. This information is not
        /// very useful once the glyphs are loaded, however, you may check this
        /// information to ensure LoadMetrics function is presented with a valid
        /// size. Will return false if no file is loaded.
        bool IsScalable() const;
        
        /// Returns the preset sizes in the font file. If the font is not
        /// scalable, these are the only sizes that will work. Will return
        /// empty vector if no file is loaded.
        std::vector<int> GetPresetSizes() const;
        
        /// Returns whether the file is loaded. Check IsReady
        bool IsFileLoaded() const;
        
        /// Returns if the FreeType is ready to work. If the metrics are set
        /// and either the file is loaded or there are loaded glyphs this 
        /// function will return true.
        bool IsReady() const;
        
        /// Discards intermediate files. New glyphs cannot be loaded
        /// automatically after this function is issued.
        void Discard();
        
    private:
        ftlib *lib;
        
        std::map<Glyph, GlyphDescriptor> glyphmap;

        int isfixedw = false;
        
        int maxwidth = 0;
        
        int height = 0;
        
        int baseline = 0;

		int digw = 0;

		bool isascii = true;
        
        int spacing = 0;

		int linethickness = 1;

		int underlinepos = 0;
    };
    
} }
