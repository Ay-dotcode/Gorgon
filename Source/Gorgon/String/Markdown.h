#pragma once

#include <string>
#include <vector>

namespace Gorgon { namespace String {
   
    /**
     * This function parses markdown formatted text to advanced text formating. Advanced text format
     * can be printed on the screen using AdvancedPrinter which can be provided by the widget 
     * registry: `Gorgon::Widgets::Registry::Active().Printer()`. This function is unicode aware. 
     * Not all markdown spec is supported. This is a very quick parser, performing a single pass
     * except for a few cases which require a single look forward. The following markdown features
     * are currently supported:
     * * Space and new line folding
     * * Block code segments using spacing (no fence support)
     * * Inline code
     * * ATX headers (up to level 4, level 4+ is considered as 4)
     * * Bullets using *, +, and -. Single level for now
     * * Italic with a single * , bold with double **. Note: Unlike standard markdown, you can have
     * multiline bold and italic statements, terminating them at the end of lines require multiple
     * passes.
     * * Strike using ~~
     * 
     * Short term planned features
     * * Line break without paragraph using \
     * * Links
     * * Numbered lists
     * * Multi level lists
     * 
     * Returns the advanced string with the links that are extracted. Link order is the same as
     * link regions that will be returned by AdvancedPrint function, allowing a clicked region to
     * be converted to link without much of a work.
     */
    std::pair<std::string, std::vector<std::string>> ParseMarkdown(const std::string &markdown, bool useinfofont = false);
    
} }
