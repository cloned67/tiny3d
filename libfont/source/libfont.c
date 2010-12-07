#include <stdio.h>
#include <string.h>
#include <stdarg.h> 
#include <libfont.h>

struct t_font_description
{
    int w, h;
    
    u8 first_char;
    u8 last_char;
    
    u32 rsx_text_offset;
    u32 rsx_bytes_per_char; 
    u32 color_format;
};

static struct t_font_datas
{

    int number_of_fonts;

    int current_font;

    struct t_font_description fonts[8];

    int sx, sy;

    u32 color, bkcolor;

    int autocenter;

    float Z;

} font_datas;


void ResetFont()
{
    font_datas.current_font = font_datas.number_of_fonts =0;

    font_datas.color = 0xffffffff;
    font_datas.bkcolor = 0;
    font_datas.autocenter = 0;
    font_datas.Z = 0.0f;

    font_datas.sx = font_datas.sy = 8;
}

u8 * AddFontFromBitmapArray(u8 *font, u8 *texture, u8 first_char, u8 last_char, int w, int h, int bits_per_pixel, int byte_order)
{
    int n, a, b;
    u8 i;
    
    if(font_datas.number_of_fonts >= 8) return texture;

    font_datas.fonts[font_datas.number_of_fonts].w = w;
    font_datas.fonts[font_datas.number_of_fonts].h = h;
    font_datas.fonts[font_datas.number_of_fonts].color_format = TINY3D_TEX_FORMAT_A1R5G5B5; //TINY3D_TEX_FORMAT_A8R8G8B8;
    font_datas.fonts[font_datas.number_of_fonts].first_char = first_char;
    font_datas.fonts[font_datas.number_of_fonts].last_char  = last_char;
    font_datas.autocenter =0;

    font_datas.color = 0xffffffff;
    font_datas.bkcolor = 0x0;

    font_datas.sx = w;
    font_datas.sy = h;

    font_datas.Z = 0.0f;
       
    for(n = first_char; n <= last_char; n++) {

        texture = (u8 *) ((((long) texture) + 15) & ~15);

        if(n == first_char) font_datas.fonts[font_datas.number_of_fonts].rsx_text_offset = tiny3d_TextureOffset(texture);

        if(n == first_char+1) font_datas.fonts[font_datas.number_of_fonts].rsx_bytes_per_char = tiny3d_TextureOffset(texture)
            - font_datas.fonts[font_datas.number_of_fonts].rsx_text_offset;

        for(a = 0; a < h; a++) {
            for(b = 0; b < w; b++) {
                
                i = font[(b * bits_per_pixel)/8];

                if(byte_order) 
                    i= (i << ((b & (7/bits_per_pixel)) * bits_per_pixel))>> (8-bits_per_pixel);
                else
                    i >>= (b & (7/bits_per_pixel)) * bits_per_pixel;
                
                i = (i & ((1 << bits_per_pixel)-1)) * 255 / ((1 << bits_per_pixel)-1);

                if(i) {//TINY3D_TEX_FORMAT_A1R5G5B5
                    i>>=3;
                    *((u16 *) texture) = (1<<15) | (i<<10) | (i<<5) | (i);
                } else {
              
                    texture[0] = texture[1] = 0x0; //texture[2] = 0x0;
                    //texture[3] = 0x0; // alpha
                } 
                texture+=2;
               
            }

            font += (w * bits_per_pixel) / 8;
                
        }
    
    }

    texture = (u8 *) ((((long) texture) + 15) & ~15);

    font_datas.number_of_fonts++;

    return texture;
}

void SetCurrentFont(int nfont)
{
    if(nfont < 0 || nfont >= font_datas.number_of_fonts) nfont = 0;

    font_datas.current_font = nfont;
}

void SetFontSize(int sx, int sy)
{
    if(sx < 8) sx = 8;
    if(sy < 8) sy = 8;

    font_datas.sx = sx;
    font_datas.sy = sy;
}

void SetFontColor(u32 color, u32 bkcolor)
{
    font_datas.color   = color;
    font_datas.bkcolor = bkcolor;
}

void SetFontAutocenter(int on_off)
{
    font_datas.autocenter  = on_off;
}

void SetFontZ(float z)
{
    font_datas.Z  = z;
}


void DrawChar(float x, float y, float z, u8 chr)
{
    float dx= font_datas.sx, dy = font_datas.sy;
    
    if(font_datas.number_of_fonts <= 0) return;

    if(chr < font_datas.fonts[font_datas.current_font].first_char) return;

    if(font_datas.bkcolor) {
        tiny3d_SetPolygon(TINY3D_QUADS);

        tiny3d_VertexPos(x     , y     , z);
        tiny3d_VertexColor(font_datas.bkcolor);

        tiny3d_VertexPos(x + dx, y     , z);

        tiny3d_VertexPos(x + dx, y + dy, z);

        tiny3d_VertexPos(x     , y + dy, z);

        tiny3d_End();
    }

    if(chr > font_datas.fonts[font_datas.current_font].last_char) return;

    // Load sprite texture
    tiny3d_SetTexture(0, font_datas.fonts[font_datas.current_font].rsx_text_offset + font_datas.fonts[font_datas.current_font].rsx_bytes_per_char 
        * (chr - font_datas.fonts[font_datas.current_font].first_char), font_datas.fonts[font_datas.current_font].w,
        font_datas.fonts[font_datas.current_font].h, font_datas.fonts[font_datas.current_font].w * 
        ((font_datas.fonts[font_datas.current_font].color_format == TINY3D_TEX_FORMAT_A8R8G8B8) ? 4 : 2), 
        font_datas.fonts[font_datas.current_font].color_format, 1);

    tiny3d_SetPolygon(TINY3D_QUADS);

    tiny3d_VertexPos(x     , y     , z);
    tiny3d_VertexColor(font_datas.color);
    tiny3d_VertexTexture(0.0f, 0.0f);

    tiny3d_VertexPos(x + dx, y     , z);
    tiny3d_VertexTexture(0.95f, 0.0f);

    tiny3d_VertexPos(x + dx, y + dy, z);
    tiny3d_VertexTexture(0.95f, 0.95f);

    tiny3d_VertexPos(x     , y + dy, z);
    tiny3d_VertexTexture(0.0f, 0.95f);

    tiny3d_End();

}

float DrawString(float x, float y, char *str)
{

    if(font_datas.autocenter) {
    
    x= (848 - strlen(str) * font_datas.sx)/ 2;

    }

    while (*str) {
        if(*str=='\n') {x = 0.0f; y += font_datas.sy; str++; continue;}
        DrawChar(x, y, font_datas.Z, (u8) *str);
        str++; x += font_datas.sx;
    }

    return x;
}

float DrawFormatString(float x, float y, char *format, ...)
{
    va_list	opt;
	static  u8 buff[2048];

	memset(buff, 0, 2048);
	va_start(opt, format);
	vsprintf( (void *) buff, format, opt);
	va_end(opt);

	char *str = (char *) buff;

    if(font_datas.autocenter) {
    
    x= (848 - strlen(str) * font_datas.sx)/ 2;

    }

    while (*str) {
        if(*str=='\n') {x = 0.0f; y += font_datas.sy; str++; continue;}
        DrawChar(x, y, font_datas.Z, (u8) *str);
        str++; x += font_datas.sx;
    }

    return x;
}


