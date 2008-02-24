/***************************************************************************
* Summon
* Matt Rasmussen
* TransformMatrix.cpp
*
*
* This file is part of SUMMON.
* 
* SUMMON is free software: you can redistribute it and/or modify
* it under the terms of the Lesser GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
* 
* SUMMON is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
* 
* You should have received a copy of the Lesser GNU General Public License
* along with SUMMON.  If not, see <http://www.gnu.org/licenses/>.
* 
***************************************************************************/


#include "TransformMatrix.h"
#include <math.h>

namespace Summon {

TransformMatrix g_transformIdentity(true);


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

void MultTransMatrix(const float *a, const float *vec, float *c)
{
    /*
    float b[16];
    b[0] =1; b[1] =0; b[2] =0; b[3] =vec[0];
    b[4] =0; b[5] =1; b[6] =0; b[7] =vec[1];
    b[8] =0; b[9] =0; b[10]=1; b[11]=vec[2];
    b[12]=0; b[13]=0; b[14]=0; b[15]=1;

    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            c[i*4+j] = a[i*4+0] * b[0*4+j] +
                       a[i*4+1] * b[1*4+j] +
                       a[i*4+2] * b[2*4+j] +
                       a[i*4+3] * b[3*4+j];
        }    
    }
    
    //
    
    for (int i=0; i<16; i+=4) {
        c[i+0] = a[i+0];
        c[i+1] = a[i+1];
        c[i+2] = a[i+2];
        c[i+3] = a[i+0] * vec[0] +
                 a[i+1] * vec[1] +
                 a[i+2] * vec[2] +
                 a[i+3];
    }
    
    */
    
    c[0] = a[0];
    c[1] = a[1];
    c[2] = a[2];
    c[3] = a[0] * vec[0] + a[1] * vec[1] + a[2] * vec[2] + a[3];
    c[4] = a[4];
    c[5] = a[5];
    c[6] = a[6];
    c[7] = a[4] * vec[0] + a[5] * vec[1] + a[6] * vec[2] + a[7];
    c[8] = a[8];
    c[9] = a[9];
    c[10] = a[10];
    c[11] = a[8] * vec[0] + a[9] * vec[1] + a[10] * vec[2] + a[11];
    c[12] = a[12];
    c[13] = a[13];
    c[14] = a[14];
    c[15] = a[12] * vec[0] + a[13] * vec[1] + a[14] * vec[2] + a[15];
}

void MakeRotateMatrix(float angle, const float *vec, float *m)
{
    float s = sin(angle * (3.1415926/180.0));
    float c = cos(angle * (3.1415926/180.0));
    float t = 1.0 - c;
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


// Assume a vector of <0, 0, 1>
void MakeRotateMatrix(float angle, float *m)
{
    float s = sin(angle * (M_PI/180.0));
    float c = cos(angle * (M_PI/180.0));
    
    m[0] = c; m[1] = -s; m[2] = 0; m[3] = 0;
    m[4] = s; m[5] = c;  m[6] = 0; m[7] = 0;
    m[8] = 0; m[9] = 0;  m[10]= 1; m[11]= 0;
    m[12]= 0; m[13]= 0;  m[14]= 0; m[15]= 1;
}

void MultRotateMatrix(const float *a, float angle, float *c)
{
    /*
    float s = sin(angle * (M_PI/180.0));
    float c = cos(angle * (M_PI/180.0));
    
    m[0] = c; m[1] = -s; m[2] = 0; m[3] = 0;
    m[4] = s; m[5] = c;  m[6] = 0; m[7] = 0;
    m[8] = 0; m[9] = 0;  m[10]= 1; m[11]= 0;
    m[12]= 0; m[13]= 0;  m[14]= 0; m[15]= 1;
    
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            c[i*4+j] = a[i*4+0] * b[0*4+j] +
                       a[i*4+1] * b[1*4+j] +
                       a[i*4+2] * b[2*4+j] +
                       a[i*4+3] * b[3*4+j];
        }
    }
    
    //
    
    for (int i=0; i<16; i+=4) {
        c[i+0] = a[i+0] * o + a[i+1] * s;
        c[i+1] = a[i+0] * -s + a[i+1] * o;
        c[i+2] = a[i+2];
        c[i+3] = a[i+3];
    }
    
    */
    
    float s = sin(angle * (M_PI/180.0));
    float o = cos(angle * (M_PI/180.0));
    
    c[0] = a[0] * o + a[1] * s;
    c[1] = a[1] * o - a[0] * s;
    c[2] = a[2];
    c[3] = a[3];
    c[4] = a[4] * o + a[5] * s;
    c[5] = a[5] * o - a[4] * s;
    c[6] = a[6];
    c[7] = a[7];
    c[8] = a[8] * o + a[9] * s;
    c[9] = a[9] * o - a[8] * s;
    c[10] = a[10];
    c[11] = a[11];
    c[12] = a[12] * o + a[13] * s;
    c[13] = a[13] * o - a[12] * s;
    c[14] = a[14];
    c[15] = a[15];

}



void MakeScaleMatrix(const float *vec, float *m)
{
    m[0] =vec[0]; m[1] =0;      m[2] =0;      m[3] =0;
    m[4] =0;      m[5] =vec[1]; m[6] =0;      m[7] =0;
    m[8] =0;      m[9] =0;      m[10]=vec[2]; m[11]=0;
    m[12]=0;      m[13]=0;      m[14]=0;      m[15]=1;
}

void MultScaleMatrix(const float *a, const float *vec, float *c)
{
    /*
    m[0] =vec[0]; m[1] =0;      m[2] =0;      m[3] =0;
    m[4] =0;      m[5] =vec[1]; m[6] =0;      m[7] =0;
    m[8] =0;      m[9] =0;      m[10]=vec[2]; m[11]=0;
    m[12]=0;      m[13]=0;      m[14]=0;      m[15]=1;

    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            c[i*4+j] = a[i*4+0] * b[0*4+j] +
                       a[i*4+1] * b[1*4+j] +
                       a[i*4+2] * b[2*4+j] +
                       a[i*4+3] * b[3*4+j];
        }
    }
    
    //
    
    for (int i=0; i<16; i++) {
        c[i] = a[i] * vec[0]; i++;
        c[i] = a[i] * vec[1]; i++;
        c[i] = a[i] * vec[2]; i++;
        c[i] = a[i];
    }

    
    */

    c[0] = a[0] * vec[0];
    c[1] = a[1] * vec[1];
    c[2] = a[2] * vec[2];
    c[3] = a[3];
    c[4] = a[4] * vec[0];
    c[5] = a[5] * vec[1];
    c[6] = a[6] * vec[2];
    c[7] = a[7];
    c[8] = a[8] * vec[0];
    c[9] = a[9] * vec[1];
    c[10] = a[10] * vec[2];
    c[11] = a[11];
    c[12] = a[12] * vec[0];
    c[13] = a[13] * vec[1];
    c[14] = a[14] * vec[2];
    c[15] = a[15];

    
}

void MakeIdentityMatrix(float *m)
{
    m[0] =1; m[1] =0; m[2] =0; m[3] =0;
    m[4] =0; m[5] =1; m[6] =0; m[7] =0;
    m[8] =0; m[9] =0; m[10]=1; m[11]=0;
    m[12]=0; m[13]=0; m[14]=0; m[15]=1;
}


void MakeTransScaleMatrix(const float *trans, const float *zoom, float *m)
{
    //  zx 0  0  tx
    //  0  zy 0  ty
    //  0  0  zz tz
    //  0  0  0  1
    
    m[0] =zoom[0]; m[1] =0;       m[2] =0;       m[3] =trans[0];
    m[4] =0;       m[5] =zoom[1]; m[6] =0;       m[7] =trans[1];
    m[8] =0;       m[9] =0;       m[10]=zoom[2]; m[11]=trans[2];
    m[12]=0;       m[13]=0;       m[14]=0;       m[15]=1;
}

void CopyMatrix(float *m, const float *n)
{
    // unwrapped for speed
    m[0] =n[0];  m[1] =n[1];  m[2] =n[2];  m[3] =n[3];
    m[4] =n[4];  m[5] =n[5];  m[6] =n[6];  m[7] =n[7];
    m[8] =n[8];  m[9] =n[9];  m[10]=n[10]; m[11]=n[11];
    m[12]=n[12]; m[13]=n[13]; m[14]=n[14]; m[15]=n[15];
}


void PrintMatrix(const float *mat, FILE *out)
{
    for (int i=0; i<4; i++) {
        fprintf(out, "%8.2f %8.2f %8.2f %8.2f\n", mat[i], mat[i+1], mat[i+2], mat[i+3]);
    }
}


}
