// -alpha_dither yes -dither 2 -non_opaque_image_format ARGB8888 -opaque_image_format RGB565 0x98233b2e
// Generated by imageconverter. Please, do not edit!

#include <touchgfx/Bitmap.hpp>
#include <BitmapDatabase.hpp>

#ifndef NO_USING_NAMESPACE_TOUCHGFX
using namespace touchgfx;
#endif

extern const unsigned char _blue_icons_add_new_48[];
extern const unsigned char _blue_icons_down_arrow_48[];
extern const unsigned char _blue_icons_home_48[];
extern const unsigned char _blue_icons_settings_48[];
extern const unsigned char _blue_icons_tools_48[];
extern const unsigned char _blue_icons_up_arrow_48[];
extern const unsigned char _blue_icons_user_48[];
extern const unsigned char _blue_slider_horizontal_medium_indicators_slider2_round_nob[];
extern const unsigned char _blue_slider_horizontal_medium_slider2_round_back[];
extern const unsigned char _blue_slider_horizontal_medium_slider2_round_fill[];
extern const unsigned char _blue_togglebars_toggle_round_large_button_off[];
extern const unsigned char _blue_togglebars_toggle_round_large_button_on[];
extern const unsigned char _miss_logo_640px[];
extern const unsigned char _start[];
extern const unsigned char _start_disabled[];
extern const unsigned char _stop[];
extern const unsigned char _stop_disabled[];

const touchgfx::Bitmap::BitmapData bitmap_database[] =
{
    { _blue_icons_add_new_48, 0, 46, 46, 21, 14, 4, 18, touchgfx::Bitmap::ARGB8888 },
    { _blue_icons_down_arrow_48, 0, 46, 26, 17, 15, 12, 7, touchgfx::Bitmap::ARGB8888 },
    { _blue_icons_home_48, 0, 46, 44, 36, 15, 4, 29, touchgfx::Bitmap::ARGB8888 },
    { _blue_icons_settings_48, 0, 46, 46, 43, 19, 2, 8, touchgfx::Bitmap::ARGB8888 },
    { _blue_icons_tools_48, 0, 48, 46, 32, 5, 6, 11, touchgfx::Bitmap::ARGB8888 },
    { _blue_icons_up_arrow_48, 0, 46, 26, 17, 4, 12, 7, touchgfx::Bitmap::ARGB8888 },
    { _blue_icons_user_48, 0, 46, 46, 6, 43, 34, 3, touchgfx::Bitmap::ARGB8888 },
    { _blue_slider_horizontal_medium_indicators_slider2_round_nob, 0, 54, 34, 8, 4, 38, 24, touchgfx::Bitmap::ARGB8888 },
    { _blue_slider_horizontal_medium_slider2_round_back, 0, 332, 20, 10, 0, 312, 20, touchgfx::Bitmap::ARGB8888 },
    { _blue_slider_horizontal_medium_slider2_round_fill, 0, 332, 20, 10, 0, 312, 20, touchgfx::Bitmap::ARGB8888 },
    { _blue_togglebars_toggle_round_large_button_off, 0, 128, 38, 13, 1, 102, 36, touchgfx::Bitmap::ARGB8888 },
    { _blue_togglebars_toggle_round_large_button_on, 0, 128, 38, 13, 1, 102, 36, touchgfx::Bitmap::ARGB8888 },
    { _miss_logo_640px, 0, 640, 202, 0, 0, 640, 202, touchgfx::Bitmap::RGB565 },
    { _start, 0, 48, 48, 0, 0, 48, 48, touchgfx::Bitmap::RGB565 },
    { _start_disabled, 0, 48, 48, 0, 0, 48, 48, touchgfx::Bitmap::RGB565 },
    { _stop, 0, 96, 96, 0, 0, 96, 96, touchgfx::Bitmap::RGB565 },
    { _stop_disabled, 0, 96, 96, 0, 0, 96, 96, touchgfx::Bitmap::RGB565 }
};

namespace BitmapDatabase
{
const touchgfx::Bitmap::BitmapData* getInstance()
{
    return bitmap_database;
}
uint16_t getInstanceSize()
{
    return (uint16_t)(sizeof(bitmap_database) / sizeof(touchgfx::Bitmap::BitmapData));
}
}

