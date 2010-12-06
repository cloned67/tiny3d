/* 
   TINY3D (c) 2010 Hermes  <www.elotrolado.net>
   This work is based in ElSemi, Phire, AerialX and may be, others PSL1GHT contributors

*/

#ifndef MATRIX_H
#define MATRIX_H

typedef struct {

    float data[4][4];

} MATRIX;

typedef struct {

    float x, y, z;

} VECTOR;


MATRIX MatrixProjPerspective(float fov, float aspect, float near, float far);

MATRIX MatrixMultiply(MATRIX old_matrix, MATRIX new_matrix);

MATRIX MatrixIdentity(void);

MATRIX MatrixTranslation(float x, float y, float z);

MATRIX MatrixTranspose(MATRIX src);

MATRIX MatrixScale(float x, float y, float z);

MATRIX MatrixRotationX(float angle);

MATRIX MatrixRotationY(float angle);

MATRIX MatrixRotationZ(float angle);

MATRIX MatrixRotationAxis(float angle, VECTOR v);


#endif