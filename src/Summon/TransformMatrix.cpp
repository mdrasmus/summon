/***************************************************************************
* Summon
* Matt Rasmussen
* TransformMatrix.cpp
*
***************************************************************************/


#include "TransformMatrix.h"
#include <math.h>

namespace Summon {

TransformMatrix g_transformIndentity(true);


/*
void MultMatrix(const float *mat1, const float *mat2, float *mat3)
{
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            mat3[i*4+j] = 0;
            for (int k=0; k<4; k++) {
                mat3[i*4+j] += mat1[i*4+k] * mat2[k*4+j];
            }
        }
    }
}
*/


void MultMatrix(const float *a, const float *b, float *c)
{
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            c[i*4+j] = a[i*4+0] * b[0*4+j] +
                       a[i*4+1] * b[1*4+j] +
                       a[i*4+2] * b[2*4+j] +
                       a[i*4+3] * b[3*4+j];
        }
    }
}


void MakeTransMatrix(const float *vec, float *m)
{
    m[0] =1; m[1] =0; m[2] =0; m[3] =vec[0];
    m[4] =0; m[5] =1; m[6] =0; m[7] =vec[1];
    m[8] =0; m[9] =0; m[10]=1; m[11]=vec[2];
    m[12]=0; m[13]=0; m[14]=0; m[15]=1;
}

void MakeRotateMatrix(float angle, const float *vec, float *m)
{
    float s = sin(angle * (3.1415926/180.0));
    float c = cos(angle * (3.1415926/180.0));
    float t = 1 - c;
    float x = vec[0];
    float y = vec[1];
    float z = vec[2];
    float n = sqrt(x*x + y*y + z*z);
    
    // normalize
    x /= n;
    y /= n;
    z /= n;
    
    m[0] = t*x*x + c;   m[1] = t*x*y - s*z; m[2] = t*x*z + s*y; m[3] = 0;
    m[4] = t*y*x + s*z; m[5] = t*y*y + c;   m[6] = t*y*z - s*x; m[7] = 0;
    m[8] = t*z*x - s*y; m[9] = t*z*y + s*x; m[10]= t*z*z + c;   m[11]= 0;
    m[12]= 0;           m[13]= 0;           m[14]= 0;           m[15]= 1;
}


void MakeScaleMatrix(const float *vec, float *m)
{
    m[0] =vec[0]; m[1] =0;      m[2] =0;      m[3] =0;
    m[4] =0;      m[5] =vec[1]; m[6] =0;      m[7] =0;
    m[8] =0;      m[9] =0;      m[10]=vec[2]; m[11]=0;
    m[12]=0;      m[13]=0;      m[14]=0;      m[15]=1;
}

void MakeIdentityMatrix(float *m)
{
    m[0] =1; m[1] =0; m[2] =0; m[3] =0;
    m[4] =0; m[5] =1; m[6] =0; m[7] =0;
    m[8] =0; m[9] =0; m[10]=1; m[11]=0;
    m[12]=0; m[13]=0; m[14]=0; m[15]=1;
}


void CopyMatrix(float *m, const float *n)
{
    // for speed
    m[0] =n[0];  m[1] =n[1];  m[2] =n[2];  m[3] =n[3];
    m[4] =n[4];  m[5] =n[5];  m[6] =n[6];  m[7] =n[7];
    m[8] =n[8];  m[9] =n[9];  m[10]=n[10]; m[11]=n[11];
    m[12]=n[12]; m[13]=n[13]; m[14]=n[14]; m[15]=n[15];
}

}
