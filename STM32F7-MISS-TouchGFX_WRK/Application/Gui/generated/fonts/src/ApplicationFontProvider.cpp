/* DO NOT EDIT THIS FILE */
/* This file is autogenerated by the text-database code generator */

#include <fonts/ApplicationFontProvider.hpp>
#include <touchgfx/InternalFlashFont.hpp>

#ifndef NO_USING_NAMESPACE_TOUCHGFX
using namespace touchgfx;
#endif

extern touchgfx::InternalFlashFont& getFont_verdanab_16_4bpp();

touchgfx::Font* ApplicationFontProvider::getFont(touchgfx::FontId fontId)
{
    switch (fontId)
    {
    case Typography::TYPOGRAPHY_00:
        return &(getFont_verdanab_16_4bpp());
    default:
        return 0;
    }
}
