/* 
   TINY3D (c) 2010 Hermes  <www.elotrolado.net>
   This work is based in ElSemi, Phire, AerialX and may be, others PSL1GHT contributors

*/

#ifndef TINY3D_H
#define TINY3D_H

#pragma once

#include <sysutil/video.h>
#include <rsx/gcm.h>

#include <rsx/commands.h>
#include <rsx/nv40.h>
#include <rsx/reality.h>

#include "matrix.h"

enum
{
    TINY3D_CANNOTINIT = -4,
    TINY3D_BUSY,
    TINY3D_OUTMEMORY,
    TINY3D_INVALID,
    TINY3D_OK = 0,

};

typedef enum 
{
    TINY3D_POINTS = 1,
    TINY3D_LINES,
    TINY3D_LINE_LOOP,
    TINY3D_LINE_STRIP,
    TINY3D_TRIANGLES,
    TINY3D_TRIANGLE_STRIP,
    TINY3D_TRIANGLE_FAN,
    TINY3D_QUADS,
    TINY3D_QUAD_STRIP,
    TINY3D_POLYGON

} type_polygon;

typedef enum 
{
    TINY3D_CLEAR_COLOR      = REALITY_CLEAR_BUFFERS_COLOR_R | REALITY_CLEAR_BUFFERS_COLOR_G 
                            | REALITY_CLEAR_BUFFERS_COLOR_B | NV30_3D_CLEAR_BUFFERS_COLOR_A,
    TINY3D_CLEAR_ZBUFFER    = REALITY_CLEAR_BUFFERS_DEPTH,
    
    TINY3D_CLEAR_STENCIL    = NV30_3D_CLEAR_BUFFERS_STENCIL,

    TINY3D_CLEAR_ALL        = TINY3D_CLEAR_COLOR | TINY3D_CLEAR_ZBUFFER | TINY3D_CLEAR_STENCIL

} clear_flags;

typedef enum 
{

    TINY3D_ALPHA_FUNC_NEVER     = 0x200,
    TINY3D_ALPHA_FUNC_LESS,
    TINY3D_ALPHA_FUNC_EQUAL,
    TINY3D_ALPHA_FUNC_LEQUAL,
    TINY3D_ALPHA_FUNC_GREATER,
    TINY3D_ALPHA_FUNC_NOTEQUAL,
    TINY3D_ALPHA_FUNC_GEQUAL,
    TINY3D_ALPHA_FUNC_ALWAYS

} alpha_func;

typedef enum 
{
    TINY3D_BLEND_FUNC_SRC_RGB_ZERO                       = 0x00000000,
    TINY3D_BLEND_FUNC_SRC_RGB_ONE,
    TINY3D_BLEND_FUNC_SRC_RGB_SRC_COLOR                  = 0x00000300,
    TINY3D_BLEND_FUNC_SRC_RGB_ONE_MINUS_SRC_COLOR,
    TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA,
    TINY3D_BLEND_FUNC_SRC_RGB_ONE_MINUS_SRC_ALPHA,
    TINY3D_BLEND_FUNC_SRC_RGB_DST_ALPHA,
    TINY3D_BLEND_FUNC_SRC_RGB_ONE_MINUS_DST_ALPHA,
    TINY3D_BLEND_FUNC_SRC_RGB_DST_COLOR,
    TINY3D_BLEND_FUNC_SRC_RGB_ONE_MINUS_DST_COLOR,
    TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA_SATURATE,
    TINY3D_BLEND_FUNC_SRC_RGB_CONSTANT_COLOR             = 0x00008001,
    TINY3D_BLEND_FUNC_SRC_RGB_ONE_MINUS_CONSTANT_COLOR,
    TINY3D_BLEND_FUNC_SRC_RGB_CONSTANT_ALPHA,
    TINY3D_BLEND_FUNC_SRC_RGB_ONE_MINUS_CONSTANT_ALPHA,
    
    TINY3D_BLEND_FUNC_SRC_ALPHA_ZERO                     = 0x00000000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_ONE                      = 0x00010000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_COLOR                = 0x03000000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_ONE_MINUS_SRC_COLOR      = 0x03010000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA                = 0x03020000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_ONE_MINUS_SRC_ALPHA      = 0x03030000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_DST_ALPHA                = 0x03040000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_ONE_MINUS_DST_ALPHA      = 0x03050000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_DST_COLOR                = 0x03060000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_ONE_MINUS_DST_COLOR      = 0x03070000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA_SATURATE       = 0x03080000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_CONSTANT_COLOR           = 0x80010000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_ONE_MINUS_CONSTANT_COLOR = 0x80020000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_CONSTANT_ALPHA           = 0x80030000,
    TINY3D_BLEND_FUNC_SRC_ALPHA_ONE_MINUS_CONSTANT_ALPHA = 0x80040000,

} blend_src_func;

typedef enum 
{

    TINY3D_BLEND_FUNC_DST_RGB_ZERO                       = 0x00000000,
    TINY3D_BLEND_FUNC_DST_RGB_ONE,
    TINY3D_BLEND_FUNC_DST_RGB_SRC_COLOR                  = 0x00000300,
    TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_COLOR,
    TINY3D_BLEND_FUNC_DST_RGB_SRC_ALPHA,
    TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA,
    TINY3D_BLEND_FUNC_DST_RGB_DST_ALPHA,
    TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_DST_ALPHA,
    TINY3D_BLEND_FUNC_DST_RGB_DST_COLOR,
    TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_DST_COLOR,
    TINY3D_BLEND_FUNC_DST_RGB_SRC_ALPHA_SATURATE,
    TINY3D_BLEND_FUNC_DST_RGB_CONSTANT_COLOR             = 0x00008001,
    TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_CONSTANT_COLOR,
    TINY3D_BLEND_FUNC_DST_RGB_CONSTANT_ALPHA,
    TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_CONSTANT_ALPHA,
   
    TINY3D_BLEND_FUNC_DST_ALPHA_ZERO                     = 0x00000000,
    TINY3D_BLEND_FUNC_DST_ALPHA_ONE                      = 0x00010000,
    TINY3D_BLEND_FUNC_DST_ALPHA_SRC_COLOR                = 0x03000000,
    TINY3D_BLEND_FUNC_DST_ALPHA_ONE_MINUS_SRC_COLOR      = 0x03010000,
    TINY3D_BLEND_FUNC_DST_ALPHA_SRC_ALPHA                = 0x03020000,
    TINY3D_BLEND_FUNC_DST_ALPHA_ONE_MINUS_SRC_ALPHA      = 0x03030000,
    TINY3D_BLEND_FUNC_DST_ALPHA_DST_ALPHA                = 0x03040000,
    TINY3D_BLEND_FUNC_DST_ALPHA_ONE_MINUS_DST_ALPHA      = 0x03050000,
    TINY3D_BLEND_FUNC_DST_ALPHA_DST_COLOR                = 0x03060000,
    TINY3D_BLEND_FUNC_DST_ALPHA_ONE_MINUS_DST_COLOR      = 0x03070000,
    TINY3D_BLEND_FUNC_DST_ALPHA_SRC_ALPHA_SATURATE       = 0x03080000,
    TINY3D_BLEND_FUNC_DST_ALPHA_CONSTANT_COLOR           = 0x80010000,
    TINY3D_BLEND_FUNC_DST_ALPHA_ONE_MINUS_CONSTANT_COLOR = 0x80020000,
    TINY3D_BLEND_FUNC_DST_ALPHA_CONSTANT_ALPHA           = 0x80030000,
    TINY3D_BLEND_FUNC_DST_ALPHA_ONE_MINUS_CONSTANT_ALPHA = 0x80040000

} blend_dst_func;


typedef enum 
{

    TINY3D_TEX_FORMAT_L8       = 0x00000100,
    TINY3D_TEX_FORMAT_A1R5G5B5 = 0x00000200,
    TINY3D_TEX_FORMAT_A4R4G4B4 = 0x00000300,
    TINY3D_TEX_FORMAT_R5G6B5   = 0x00000400,
    TINY3D_TEX_FORMAT_A8R8G8B8 = 0x00000500

} text_format;

typedef enum 
{

    TINY3D_BLEND_RGB_FUNC_ADD                = 0x00008006,
    TINY3D_BLEND_RGB_MIN,
    TINY3D_BLEND_RGB_MAX,
    TINY3D_BLEND_RGB_FUNC_SUBTRACT           = 0x0000800a,
    TINY3D_BLEND_RGB_FUNC_REVERSE_SUBTRACT,

    TINY3D_BLEND_ALPHA_FUNC_ADD              = 0x80060000,
    TINY3D_BLEND_ALPHA_MIN                   = 0x80070000,
    TINY3D_BLEND_ALPHA_MAX                   = 0x80080000,
    TINY3D_BLEND_ALPHA_FUNC_SUBTRACT         = 0x800a0000,
    TINY3D_BLEND_ALPHA_FUNC_REVERSE_SUBTRACT = 0x800b0000

} blend_func;


/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* INITIALIZE                                                                                                                                  */
/*---------------------------------------------------------------------------------------------------------------------------------------------*/

// initialize video and 3D environmment. vertex_buff_size is the size (in bytes) for the buffer that allocates the vertex datas of the polygons

int tiny3d_Init(int vertex_buff_size);

/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* CLEAR                                                                                                                                       */
/*---------------------------------------------------------------------------------------------------------------------------------------------*/

// clear the buffers. Usually call it with tiny3d_Clear(color, TINY3D_CLEAR_ALL)

void tiny3d_Clear(u32 color, clear_flags flags);

/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* CHANGE TO 2D/3D                                                                                                                             */
/*---------------------------------------------------------------------------------------------------------------------------------------------*/

void tiny3d_Project2D();

void tiny3d_Project3D();

/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* FLIP                                                                                                                                        */
/*---------------------------------------------------------------------------------------------------------------------------------------------*/

// change the screen buffer, waits to vblank and reset some variables. 
// Polygons must be writen after tiny3d_Clear() and before tiny3d_Flip() functions

void tiny3d_Flip();

/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* TEXTURES                                                                                                                                    */
/*---------------------------------------------------------------------------------------------------------------------------------------------*/

// used to alloc RSX memory for the textures. The best idea is allocate a big area to work with textures, because you cannot deallocate the memory used
void * tiny3d_AllocTexture(u32 size);

// RSX use the offset to work with textures. This function return the texture offset from the pointer to the RSX memory allocated
u32 tiny3d_TextureOffset(void * text);

// set texture to draw
void tiny3d_SetTexture(u32 unit, u32 offset, u32 width, u32 height, u32 stride, text_format fmt, int smooth);

/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* ALPHA / BLEND                                                                                                                               */
/*---------------------------------------------------------------------------------------------------------------------------------------------*/

// enable/disable the alpha test
void tiny3d_AlphaTest(int enable, u8 ref, alpha_func func);

// enable/disable the blend function
void tiny3d_BlendFunc( int enable, blend_src_func src_fun, blend_dst_func dst_func, blend_func func);

/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* MATRIX                                                                                                                                      */
/*---------------------------------------------------------------------------------------------------------------------------------------------*/

// set projection matrix (used in 3D context)

void tiny3d_SetProjectionMatrix(MATRIX *mat);

// set Model/View matrix: Call before tiny3d_SetPolygon()
void tiny3d_SetMatrixModelView(MATRIX *mat);

/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* DRAW POLYGONS                                                                                                                               */
/*---------------------------------------------------------------------------------------------------------------------------------------------*/

// set the polygon to draw. It must be called before to call tiny3d_Vertex* functions
int tiny3d_SetPolygon(type_polygon type);

// close the list of vertex, update shader and world matrix and write polygons
int tiny3d_End();


// position: it must be the first vertex function to call
void tiny3d_VertexPos(float x, float y, float z);
void tiny3d_VertexPos4(float x, float y, float z, float w);

//color: fix color method
void tiny3d_VertexColor(u32 rgba);
void tiny3d_VertexFcolor(float r, float g, float b, float a);

// texture: texture coords
void tiny3d_VertexTexture(float u, float v);

/*---------------------------------------------------------------------------------------------------------------------------------------------*/
/* VIDEO                                                                                                                                       */
/*---------------------------------------------------------------------------------------------------------------------------------------------*/

extern int Video_currentBuffer;

extern VideoResolution Video_Resolution;

extern u32 * Video_buffer[2];

extern int Video_pitch;

extern u8 Video_aspect; // 1-> 4:3 2-> 16:9

#endif

