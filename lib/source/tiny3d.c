/* 
   TINY3D (c) 2010 Hermes  <www.elotrolado.net>
   This work is based in ElSemi, Phire, AerialX and may be, others PSL1GHT contributors

*/

#include "tiny3d.h"
#include "matrix.h"

#include <string.h>
#include <math.h>
#include <assert.h>

#include "rsxutil.h"
#include "nv_shaders.h"
#include "vshader_color.vcg.h"
#include "vshader_text.vcg.h"
#include "vshader_text_color.vcg.h"


static int use_2d = 0; // MODE 2D/3D

#define Z_SCALE 1.0/65536.0

int Video_currentBuffer = 0;

static u8 *rsx_vertex = NULL;           // vertex memory

static int size_rsx_vertex = 1024*1024; // size of vertex memory

static u32 pos_rsx_vertex = 0;          // offset to vertex memory

static int polygon = -1;


static u32 off_head_vertex = 0;  // index from the first vertex from shader list
static u32 off_start_vertex = 0; // memory offset of the first vertex of the polygon

static int min_vertex = 0x7fffffff; // min vertex for polygon requested
static int mod_vertex = 0x7fffffff; // module for the next vertex for polygon requested

static int n_vertex = 0; // number of vertex counted for the current polygon

#define VERTEX_LOCK      256
#define VERTEX_SETMATRIX 128
#define VERTEX_SETPOS      1
#define VERTEX_SETFCOL     2
#define VERTEX_SETCOL      4
#define VERTEX_SETTEXT     8

static int flag_vertex = 0; // flags to determine the shader and the datas used

static struct
{
    float x, y, z, w;

    float r, g, b, a;

    u32 rgba;

    float u, v;

} vertex_data;   // temp vertex data


static int current_shader = -1; // current shader ID

static struct _data_shader {
    int off_matrix;
    int off_position;
    int off_color;
    int off_texture;

    void *vp;
    void *fp;

    int size_vertex;

} data_shader[5];


static MATRIX matrix_ident =
{
    {
    {1.0, 0.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 0.0},
    {0.0, 0.0, 1.0, 0.0},
    {0.0, 0.0, 0.0, 1.0}
    }
};

// Projection Matrix (used in 3D)

static MATRIX project_mat;

// Model/View matrix

static MATRIX model_view;

// shader matrix
static MATRIX matrix_out =
{
    {
    {1.0, 0.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 0.0},
    {0.0, 0.0, 1.0, 0.0},
    {0.0, 0.0, 0.0, 1.0}
    }
};

#define CASE_POLYGON1(x, min, mod) case x: \
       min_vertex = min; mod_vertex = mod; break;



/***********************************************************************************************************/
/* INIT                                                                                                    */
/***********************************************************************************************************/

int tiny3d_Init(int vertex_buff_size)
{
    static int inited= 0;

    flag_vertex = VERTEX_LOCK;

    if(inited) return TINY3D_CANNOTINIT;

    inited = 1;

    init_screen();

    // install fragments shaders in rsx memory
    u32 *frag_mem = rsxMemAlign(256, 256);
    
    if(!frag_mem) return TINY3D_OUTMEMORY;

    realityInstallFragmentProgram(context, &nv30_fp_color, frag_mem);
    
    u32 *frag_mem2 = rsxMemAlign(256, 256);
    
    if(!frag_mem2) return TINY3D_OUTMEMORY;

    realityInstallFragmentProgram(context, &nv30_fp_texture, frag_mem2);
    
    u32 *frag_mem3 = rsxMemAlign(256, 256);
    
    if(!frag_mem3) return TINY3D_OUTMEMORY;
    
    realityInstallFragmentProgram(context, &nv30_fp_texture_color, frag_mem3);

    // shaders datas

    data_shader[0].off_matrix   = realityVertexProgramGetConstant((realityVertexProgram*)       vshader_color_bin, "TransMatrix");
    data_shader[0].off_position = realityVertexProgramGetInputAttribute((realityVertexProgram*) vshader_color_bin, "inputvertex.vertex");
    data_shader[0].off_texture  = -1;
    data_shader[0].off_color    = realityVertexProgramGetInputAttribute((realityVertexProgram*) vshader_color_bin, "inputvertex.color");
    data_shader[0].vp           = (void *) vshader_color_bin;
    data_shader[0].fp           = &nv30_fp_color;
    data_shader[0].size_vertex  = 16+16;

    data_shader[1]              = data_shader[0];
    data_shader[1].size_vertex  = 16+4;

    data_shader[2].off_matrix   = realityVertexProgramGetConstant((realityVertexProgram*)       vshader_text_bin, "TransMatrix");
    data_shader[2].off_position = realityVertexProgramGetInputAttribute((realityVertexProgram*) vshader_text_bin, "inputvertex.vertex");
    data_shader[2].off_texture  = realityVertexProgramGetInputAttribute((realityVertexProgram*) vshader_text_bin, "inputvertex.texcoord");
    data_shader[2].off_color    = -1;
    data_shader[2].vp           = (void *) vshader_text_bin;
    data_shader[2].fp           = &nv30_fp_texture;
    data_shader[2].size_vertex  = 16+8;

    data_shader[3].off_matrix   = realityVertexProgramGetConstant((realityVertexProgram*)       vshader_text_color_bin, "TransMatrix");
    data_shader[3].off_position = realityVertexProgramGetInputAttribute((realityVertexProgram*) vshader_text_color_bin, "inputvertex.vertex");
    data_shader[3].off_texture  = realityVertexProgramGetInputAttribute((realityVertexProgram*) vshader_text_color_bin, "inputvertex.texcoord");
    data_shader[3].off_color    = realityVertexProgramGetInputAttribute((realityVertexProgram*) vshader_text_color_bin, "inputvertex.color");
    data_shader[3].vp           = (void *) vshader_text_color_bin;
    data_shader[3].fp           = &nv30_fp_texture_color;
    data_shader[3].size_vertex  = 16+16+8;

    data_shader[4]              = data_shader[3];
    data_shader[4].size_vertex  = 16+4+8;

    current_shader = -1;

    if(vertex_buff_size <= 1024*1024) vertex_buff_size = 1024*1024;

    rsx_vertex = rsxMemAlign(64, vertex_buff_size);

    pos_rsx_vertex = 0;

    polygon = -1;
    off_head_vertex = off_start_vertex = 0;
    
    return TINY3D_OK;
}

/***********************************************************************************************************/
/* CHANGE SHADER CONTEXT / PUT_VERTEX                                                                      */
/***********************************************************************************************************/

static void set_shader_context()
{
    u32 offset;
    u32 text_off = 16;
    
    // Load shaders, because the rsx won't do anything without them.
    realityLoadVertexProgram(context, (realityVertexProgram*) data_shader[current_shader].vp);
    realityLoadFragmentProgram(context, data_shader[current_shader].fp); 

    // calculate projection matrix
    if(flag_vertex & VERTEX_SETMATRIX) {
        
        if(use_2d) matrix_out = model_view;
            else
        matrix_out =  MatrixMultiply(model_view, project_mat);

    }
    //Pass the matrix to the shader
    realitySetVertexProgramConstant4fBlock(context, data_shader[current_shader].off_matrix,  4, (float*)(matrix_out.data));
    flag_vertex &= ~VERTEX_SETMATRIX;

    off_head_vertex = 0;
    //Bind the memory array to the input attributes
    //rsx requires the offset in his memory area
    realityAddressToOffset(&rsx_vertex[off_start_vertex],&offset);

    //stride is the distance (in bytes) from the attribute in a vertex to the same attribute in the next vertex (that is, the size of a single vertex struct)
    //elements is the number of components of this attribute that will be passed to this input parameter in the vertex program (max 4)
    realityBindVertexBufferAttribute(context, data_shader[current_shader].off_position, offset, data_shader[current_shader].size_vertex, 4, 
        REALITY_BUFFER_DATATYPE_FLOAT, REALITY_RSX_MEMORY);

    if(data_shader[current_shader].off_color >= 0) {
        //now the color
        realityAddressToOffset(&rsx_vertex[off_start_vertex+16],&offset);
        if(current_shader == 1 || current_shader == 4) {
            text_off += 4;
            realityBindVertexBufferAttribute(context, data_shader[current_shader].off_color, offset, data_shader[current_shader].size_vertex, 4,
                REALITY_BUFFER_DATATYPE_BYTE, REALITY_RSX_MEMORY);
        } else {
            text_off += 16;
            realityBindVertexBufferAttribute(context, data_shader[current_shader].off_color, offset, data_shader[current_shader].size_vertex, 4,
                REALITY_BUFFER_DATATYPE_FLOAT, REALITY_RSX_MEMORY);
        }
    }

    if(data_shader[current_shader].off_texture >= 0) {
        //now the texture coords
        realityAddressToOffset(&rsx_vertex[off_start_vertex + text_off], &offset);
        realityBindVertexBufferAttribute(context, data_shader[current_shader].off_texture, offset, data_shader[current_shader].size_vertex, 2,
            REALITY_BUFFER_DATATYPE_FLOAT, REALITY_RSX_MEMORY);
    }
}


static void put_vertex()
{
    
    if(pos_rsx_vertex > (size_rsx_vertex - 128)) {flag_vertex =(flag_vertex & ~VERTEX_SETPOS) | VERTEX_LOCK; return;}

    if(flag_vertex & VERTEX_SETPOS) {

        // POSITION
        memcpy((void *) &rsx_vertex[pos_rsx_vertex], (void *) &vertex_data.x, 16);
        pos_rsx_vertex += 16;
        
        // FLOAT COLOR
        if(flag_vertex & VERTEX_SETFCOL) {

            memcpy((void *) &rsx_vertex[pos_rsx_vertex], (void *) &vertex_data.r, 16);
            pos_rsx_vertex += 16;

        } else if(flag_vertex & VERTEX_SETCOL) {

        // UINT32 COLOR

            memcpy(&rsx_vertex[pos_rsx_vertex], (void *) &vertex_data.rgba, 4);
            pos_rsx_vertex += 4;

        } else if(!(flag_vertex & VERTEX_SETTEXT)){ // default

        // UINT32 COLOR (default auto color)
            vertex_data.rgba = 0xffffffff;
            flag_vertex |= VERTEX_SETCOL;
            memcpy(&rsx_vertex[pos_rsx_vertex], (void *) &vertex_data.rgba, 4);
            pos_rsx_vertex += 4;
            
        }
        
        // TEXTURE
        if(flag_vertex & VERTEX_SETTEXT) {
            memcpy((void *) &rsx_vertex[pos_rsx_vertex], (void *) &vertex_data.u, 8);
            pos_rsx_vertex += 8;
        }

        n_vertex++;
    }
   
   flag_vertex &= ~VERTEX_SETPOS;

}

/***********************************************************************************************************/
/* POLYGON / VERTEX                                                                                        */
/***********************************************************************************************************/

int tiny3d_SetPolygon(type_polygon type)
{
    n_vertex = 0;
    
    flag_vertex = (flag_vertex &  VERTEX_SETMATRIX) | VERTEX_LOCK;

    if(polygon > 0) return TINY3D_BUSY;

    if(type < TINY3D_POINTS || type > TINY3D_POLYGON) return TINY3D_INVALID;

    polygon = type;

    switch(polygon) {
        
        CASE_POLYGON1(TINY3D_POINTS,         1, 1);
        CASE_POLYGON1(TINY3D_LINES,          2, 2);
        CASE_POLYGON1(TINY3D_LINE_LOOP,      2, 1);
        CASE_POLYGON1(TINY3D_LINE_STRIP,     2, 1);
        CASE_POLYGON1(TINY3D_TRIANGLES,      3, 3);
        CASE_POLYGON1(TINY3D_TRIANGLE_STRIP, 3, 1);
        CASE_POLYGON1(TINY3D_TRIANGLE_FAN,   3, 1);
        CASE_POLYGON1(TINY3D_QUADS,          4, 4);
        CASE_POLYGON1(TINY3D_QUAD_STRIP,     4, 2);
        CASE_POLYGON1(TINY3D_POLYGON,        3, 1)
    }
    
    off_start_vertex = pos_rsx_vertex;

    flag_vertex &= ~VERTEX_LOCK;

    return TINY3D_OK;

}

void tiny3d_VertexPos(float x, float y, float z)
{
    
    if(flag_vertex & VERTEX_LOCK) return;

    put_vertex(); // previous vertex;
    
    vertex_data.x = x; vertex_data.y = y; vertex_data.z = z; vertex_data.w = 1.0f;

    flag_vertex |= VERTEX_SETPOS;
}

void tiny3d_VertexPos4(float x, float y, float z, float w)
{
    if(flag_vertex & VERTEX_LOCK) return;

    put_vertex(); // previous vertex;
    
    vertex_data.x = x; vertex_data.y = y; vertex_data.z = z; vertex_data.w = w;

    flag_vertex |= VERTEX_SETPOS;
}

void tiny3d_VertexColor(u32 rgba)
{
    
    vertex_data.rgba = rgba;

    flag_vertex |= VERTEX_SETCOL;

}

void tiny3d_VertexFcolor(float r, float g, float b, float a)
{
    
    vertex_data.r = r; vertex_data.g = g; vertex_data.b = b; vertex_data.a = a;

    flag_vertex |= VERTEX_SETFCOL;
}

void tiny3d_VertexTexture(float u, float v)
{
    
    vertex_data.u = u; vertex_data.v = v;

    flag_vertex |= VERTEX_SETTEXT;
}


int tiny3d_End()
{
    put_vertex(); // set the last vertex
    
    if(polygon > 0 && n_vertex >= min_vertex) {
        
        int temp_shader=0;

        n_vertex -= ((n_vertex - min_vertex) % mod_vertex);
        
        if(flag_vertex & VERTEX_SETTEXT) { // texture
            temp_shader = 2;
            if(flag_vertex & VERTEX_SETFCOL) { // with color float
                temp_shader = 3; 
            } else if(flag_vertex & VERTEX_SETCOL) { // with color u32
                temp_shader = 4;
            }

        } else if(flag_vertex & VERTEX_SETCOL) {
            temp_shader = 1;  // with color u32
        }
        
        if(temp_shader != current_shader) { // set the shader
        
            current_shader = temp_shader;
            set_shader_context();
        }

        if(flag_vertex & VERTEX_SETMATRIX) { // update matrix
            
            if(use_2d) matrix_out = model_view;
                else
            matrix_out =  MatrixMultiply(model_view, project_mat);

            realitySetVertexProgramConstant4fBlock(context, data_shader[current_shader].off_matrix, 4, (float*)(matrix_out.data));
            flag_vertex &= ~VERTEX_SETMATRIX;
        }

        realityDrawVertexBuffer(context, polygon, off_head_vertex, n_vertex);

        off_head_vertex += n_vertex;

        n_vertex = 0;

        polygon = -1;
        flag_vertex = VERTEX_LOCK;

    }


    return TINY3D_OK;

}

/***********************************************************************************************************/
/* MATRIX                                                                                                  */
/***********************************************************************************************************/

void tiny3d_SetMatrixModelView(MATRIX *mat)
{
    if(!mat) 
        model_view = matrix_ident;
    else
        model_view = *mat;

    flag_vertex |= VERTEX_SETMATRIX;
}

void tiny3d_SetProjectionMatrix(MATRIX *mat)
{
    if(!mat) 
        project_mat= matrix_ident;
    else
        project_mat = *mat;

    if(!use_2d)
        flag_vertex |= VERTEX_SETMATRIX;
}


/***********************************************************************************************************/
/* SCREEN CLEAR / FLIP / CHANGE 2D/3D MODE                                                                 */
/***********************************************************************************************************/

void tiny3d_Flip()
{
    
    tiny3d_End();

    flip(Video_currentBuffer); // Flip buffer onto screen
    
    Video_currentBuffer = !Video_currentBuffer;

    waitFlip();

    pos_rsx_vertex = 0;
    current_shader = -1;
    polygon = -1;
    off_head_vertex = off_start_vertex = 0;
    flag_vertex = VERTEX_LOCK;

}

void tiny3d_Project2D()
{
    use_2d = 1;

    // 480P / 576P
    if(Video_Resolution.width<1280) {

        realityViewportTranslate(context, 38.0 , 16.0, 0.0, 0.0);

        realityViewportScale(context, (float) (Video_Resolution.width-72) / 848.0, (Video_Resolution.height==480) ? (512.0) / 576.0 : 548.0 / 512.0, Z_SCALE, 1.0);
    
    } else if(Video_Resolution.width==1280) {
    // 720P
        
        realityViewportTranslate(context, 54.0, 24.0, 0.0, 0.0);

        realityViewportScale(context, 848.0 / 611.0 ,  674.0 / 512.0, Z_SCALE, 1.0);
    
    } else {
    // 1080i
        
        realityViewportTranslate(context, 63.0, 40.0, 0.0, 0.0);

        realityViewportScale(context, 848.0 / 400.0 ,  952.0 / 512.0, Z_SCALE, 1.0);
    
    }
}

void tiny3d_Project3D()
{
    use_2d = 0;

    realityViewportTranslate(context, (float) Video_Resolution.width / 2.0f, (float) Video_Resolution.height / 2.0, 0.0, 0.0);
    realityViewportScale(context, (float) Video_Resolution.width/1920.0f, (float) Video_Resolution.height/1080.0f , Z_SCALE, 1.0);
  
}


void tiny3d_Clear(u32 color, clear_flags flags)
{
    flag_vertex |= VERTEX_LOCK;

    if(flags & TINY3D_CLEAR_COLOR) {
       
        tiny3d_Project2D();

        tiny3d_SetProjectionMatrix(&matrix_ident);
        tiny3d_SetMatrixModelView(&matrix_ident);
        
        realityZControl(context, 0, 1, 1); // disable viewport culling
 
        realityDepthTestFunc(context, REALITY_ZFUNC_LESSOREQUAL);
        realityDepthWriteEnable(context, 1);
        realityDepthTestEnable(context, 1);

        realityAlphaEnable(context, 0);

        realityBlendEnable(context, 0);

        realityViewport(context, Video_Resolution.width, Video_Resolution.height);
        
        int n;
        for(n = 0; n < 8; n++)
            realityViewportClip(context, n, Video_Resolution.width, Video_Resolution.height);

        setupRenderTarget(Video_currentBuffer);

        realitySetClearColor(context, color);
    }

    if(flags & TINY3D_CLEAR_ZBUFFER) {

        realitySetClearDepthValue(context, 0xffffffff);
    }

    realityClearBuffers(context, flags);
}

/***********************************************************************************************************/
/* ALPHA / BLEND func                                                                                      */
/***********************************************************************************************************/

void tiny3d_AlphaTest (int enable, u8 ref, alpha_func func)
{
    realityAlphaFunc(context, func);

    realityAlphaRef(context, (u32) ref);

    realityAlphaEnable(context, enable!=0);
}

void tiny3d_BlendFunc (int enable, blend_src_func src_fun, blend_dst_func dst_func, blend_func func)
{
    realityBlendFunc(context, src_fun, dst_func);
    realityBlendEquation(context, func);
    realityBlendEnable(context, enable!=0);
}

/***********************************************************************************************************/
/* TEXTURES                                                                                                */
/***********************************************************************************************************/

void tiny3d_SetTexture(u32 unit, u32 offset, u32 width, u32 height, u32 stride, text_format fmt, int smooth)
{
  realityTexture tex;

  tex.swizzle =
    NV30_3D_TEX_SWIZZLE_S0_X_S1 | NV30_3D_TEX_SWIZZLE_S0_Y_S1 |
    NV30_3D_TEX_SWIZZLE_S0_Z_S1 | NV30_3D_TEX_SWIZZLE_S0_W_S1 |
    NV30_3D_TEX_SWIZZLE_S1_X_X | NV30_3D_TEX_SWIZZLE_S1_Y_Y |
    NV30_3D_TEX_SWIZZLE_S1_Z_Z | NV30_3D_TEX_SWIZZLE_S1_W_W ;

  tex.offset = offset;

  tex.format = fmt |
    NV40_3D_TEX_FORMAT_LINEAR  | 
    NV30_3D_TEX_FORMAT_DIMS_2D |
    NV30_3D_TEX_FORMAT_DMA0 |
    NV30_3D_TEX_FORMAT_NO_BORDER | (0x8000) |
    (1 << NV40_3D_TEX_FORMAT_MIPMAP_COUNT__SHIFT);

  tex.wrap =  NV30_3D_TEX_WRAP_S_REPEAT |
    NV30_3D_TEX_WRAP_T_REPEAT |
    NV30_3D_TEX_WRAP_R_REPEAT;

  tex.enable = NV40_3D_TEX_ENABLE_ENABLE;

  if(smooth)
    tex.filter = NV30_3D_TEX_FILTER_MIN_LINEAR |
	       NV30_3D_TEX_FILTER_MAG_LINEAR | 0x3fd6;
  else
    tex.filter = NV30_3D_TEX_FILTER_MIN_NEAREST |
	       NV30_3D_TEX_FILTER_MAG_NEAREST | 0x3fd6;

  tex.width = width;
  tex.height = height;
  tex.stride = stride;

  realitySetTexture(context, unit, &tex);
}

void * tiny3d_AllocTexture(u32 size)
{
   return rsxMemAlign(16, size);
}

u32 tiny3d_TextureOffset(void * text)
{
    u32 offset = 0;

    assert(realityAddressToOffset(text, &offset)==0);
    
    return offset;
}