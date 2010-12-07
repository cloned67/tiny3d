#ifndef LIBFONT_H
#define LIBFONT_H

#include <tiny3d.h>

/* NOTE: LIBFONT is thinkin to work with Tiny3D 2D mode: you need to call tiny3d_Project2D() before to work with draw functions */

// initialize all datas. After you call it you don't have any font to use

void ResetFont();

// used as byte_order in AddFontFromBitmapArray()

#define BIT0_FIRST_PIXEL 0
#define BIT7_FIRST_PIXEL 1

/* add one font from one bitmap array. You can define the font range with first_char and last_char (in my sample i use two fonts that starts
at chr 32 and finish iat chr 255 and other font starts at 0 and finish at 254. fonts can have one depth of 1, 2, 4, 8 bits (used as pixel intensity)
to create smooth fonts and you can select the byte order (some fonts can use bit 7 as first pixel or bit 0) .

w and h must be 8, 16, 32, .....

It receive one RSX texture pointer and return the texture pointer increased and aligned to 16 bytes think to use this pointer to build the next
RSX texture.
*/

u8 * AddFontFromBitmapArray(u8 *font, u8 *texture, u8 first_char, u8 last_char, int w, int h, int bits_per_pixel, int byte_order);

/* function to select the current font to use (the first is 0. if you select an undefined font, it uses font 0) */

void SetCurrentFont(int nfont);

// font are resizable: the minimun is 8 x 8 but you can use as you want. Remember the font quality depends of the original size

void SetFontSize(int sx, int sy);

// select the color and the background color for the font. if you use 0 for bkcolor background is not drawing

void SetFontColor(u32 color, u32 bkcolor);

// enable/disable the autocenter feature. don't use '\n' or unsupported characters here

void SetFontAutocenter(int on_off);

// Z used to draw the font. Usually is 0.0f

void SetFontZ(float z);

// function to draw one character

void DrawChar(float x, float y, float z, u8 chr);

// function to draw one string. It return X incremented

float DrawString(float x, float y, char *str);

// function to draw with fomat string similar to printf. It return X incremented

float DrawFormatString(float x, float y, char *format, ...);

#endif