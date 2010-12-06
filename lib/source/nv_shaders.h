#ifndef __NV_SHADERS_H__
#define __NV_SHADERS_H__

#include <rsx/commands.h>


/*******************************************************************************
 * NV30/NV40/G70 fragment shaders
 */

static realityFragmentProgram nv30_fp_texture = {
.num_regs = 2,
.size = (2*4),
.data = {
/* TEX R0, fragment.texcoord[0], texture[0], 2D */
0x17009e00, 0x1c9dc801, 0x0001c800, 0x3fe1c800,
/* MOV R0, R0 */
0x01401e81, 0x1c9dc800, 0x0001c800, 0x0001c800,
}
};

static realityFragmentProgram nv30_fp_texture_color = {
.num_regs = 2,
.size = (2*4),
.data = {
// TEXX   R0, f[TEX0], TEX0, 2D;
0x17809e00, 0x1c9dc801, 0x0, 0x0,
// MULX   H0, R0, f[COL0];
0x2803e81, 0x1c9dc800, 0x1c801, 0x0
}
};

static realityFragmentProgram nv30_fp_color = {
.num_regs = 2,
.size = (1*4),
.data = {
// MOV  H0, f[COL0];
0x1003e81, 0x1c9dc801, 0x0, 0x0
}
};
#endif
