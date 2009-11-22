/***************************************************************************
* Summon
* Matt Rasmussen
* TransformMatrix.h
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

#ifndef SUMMON_TRANSFORM_MATRIX_H
#define SUMMON_TRANSFORM_MATRIX_H

#include <stdio.h>
#include <math.h>
#include <assert.h>

namespace Summon {


typedef float Float;


template <class Float>
void MultMatrix(const Float *a, const Float *b, Float *c)
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

template <class Float>
void MakeTransMatrix(const Float *vec, Float *m)
{
    m[0] =1; m[1] =0; m[2] =0; m[3] =vec[0];
    m[4] =0; m[5] =1; m[6] =0; m[7] =vec[1];
    m[8] =0; m[9] =0; m[10]=1; m[11]=vec[2];
    m[12]=0; m[13]=0; m[14]=0; m[15]=1;
}


template <class Float>
void MultTransMatrix(const Float *a, const Float *vec, Float *c)
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


template <class Float>
void MakeRotateMatrix(Float angle, const Float *vec, Float *m)
{
    Float s = sin(angle * (3.1415926/180.0));
    Float c = cos(angle * (3.1415926/180.0));
    Float t = 1.0 - c;
    Float x = vec[0];
    Float y = vec[1];
    Float z = vec[2];
    Float n = sqrt(x*x + y*y + z*z);
    
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
template <class Float>
void MakeRotateMatrix(Float angle, Float *m)
{
    Float s = sin(angle * (M_PI/180.0));
    Float c = cos(angle * (M_PI/180.0));
    
    m[0] = c; m[1] = -s; m[2] = 0; m[3] = 0;
    m[4] = s; m[5] = c;  m[6] = 0; m[7] = 0;
    m[8] = 0; m[9] = 0;  m[10]= 1; m[11]= 0;
    m[12]= 0; m[13]= 0;  m[14]= 0; m[15]= 1;
}


template <class Float>
void MultRotateMatrix(const Float *a, Float angle, Float *c)
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
    
    Float s = sin(angle * (M_PI/180.0));
    Float o = cos(angle * (M_PI/180.0));
    
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


template <class Float>
void MakeScaleMatrix(const Float *vec, Float *m)
{
    m[0] =vec[0]; m[1] =0;      m[2] =0;      m[3] =0;
    m[4] =0;      m[5] =vec[1]; m[6] =0;      m[7] =0;
    m[8] =0;      m[9] =0;      m[10]=vec[2]; m[11]=0;
    m[12]=0;      m[13]=0;      m[14]=0;      m[15]=1;
}

template <class Float>
void MultScaleMatrix(const Float *a, const Float *vec, Float *c)
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

template <class Float>
void MakeIdentityMatrix(Float *m)
{
    m[0] =1; m[1] =0; m[2] =0; m[3] =0;
    m[4] =0; m[5] =1; m[6] =0; m[7] =0;
    m[8] =0; m[9] =0; m[10]=1; m[11]=0;
    m[12]=0; m[13]=0; m[14]=0; m[15]=1;
}

template <class Float>
void MakeTransScaleMatrix(const Float *trans, const Float *zoom, Float *m)
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

template <class Float>
void CopyMatrix(Float *m, const Float *n)
{
    // unwrapped for speed
    m[0] =n[0];  m[1] =n[1];  m[2] =n[2];  m[3] =n[3];
    m[4] =n[4];  m[5] =n[5];  m[6] =n[6];  m[7] =n[7];
    m[8] =n[8];  m[9] =n[9];  m[10]=n[10]; m[11]=n[11];
    m[12]=n[12]; m[13]=n[13]; m[14]=n[14]; m[15]=n[15];
}

template <class Float>
void PrintMatrix(const Float *mat, FILE *out)
{
    for (int i=0; i<4; i++) {
        fprintf(out, "%8.2f %8.2f %8.2f %8.2f\n", mat[i], mat[i+1], mat[i+2], mat[i+3]);
    }
}




//=============================================================================

template <class Float>
class TransformMatrixTemp
{
public:
    TransformMatrixTemp() {}
    TransformMatrixTemp(bool initIdentity) {
        if (initIdentity)
            SetIdentity();
    }

    inline void VecMult(Float x1, Float y1, Float *x2, Float *y2) const 
    {
        *x2 = x1*mat[0] + y1*mat[1] + mat[3];
        *y2 = x1*mat[4] + y1*mat[5] + mat[7];
    }
    
    inline void VecInvMult(Float x2, Float y2, Float *x1, Float *y1) const 
    {
        /*
        x2 = a*x1 + b*y1 + c
        y2 = d*x1 + e*y1 + f
        
        if a != 0:
            x1 = (x2 - b*y1 - c) / a
            y1 = (y2 - d*x1 - f) / e

            y1 = (y2 - f - d*x1) / e
            y1 = (y2 - f - d*(x2 - b*y1 - c) / a) / e
               = (y2 - f)/e - d*(x2 - b*y1 - c) / (a*e)
               = (y2 - f)/e - d*x2/(a*e) + d*b*y1/(a*e) + d*c/(a*e)

            y1 - y1*d*b/(a*e) = (y2 - f)/e - d*x2/(a*e) + d*c/(a*e)
            (1 - d*b/(a*e)) * y1 = (y2 - f)/e - d*x2/(a*e) + d*c/(a*e)
            y1 = ((y2 - f)/e - d*x2/(a*e) + d*c/(a*e)) / (1 - d*b/(a*e))
            y1 = ((y2 - f) - d*x2/a + d*c/a) / (e - d*b/a)
            y1 = (a*(y2 - f) - d*x2 + d*c) / (a*e - d*b)
            y1 = (a*(y2 - f) - d*(x2 - c)) / (a*e - d*b)            
        */
    
        const Float a = mat[0];
        const Float b = mat[1];
        const Float c = mat[3];
        const Float d = mat[4];
        const Float e = mat[5];
        const Float f = mat[7];
        
        if (a != 0.0) {
            if (e != 0.0)
                *y1 = (a*(y2 - f) - d*(x2 - c)) / (a*e - d*b);
            else
                *y1 = 0.0;
            *x1 = (x2 - b*(*y1) - c) / a;
        } else if (b != 0.0) {
            *y1 = (x2 - c) / b;
            if (d != 0.0)
                *x1 = (y2 - e*(*y1) - f) / d;
            else
                *x1 = 0;
        } else {
            // a=0, b=0
            if (e != 0) {
                *y1 = (y2 - f) / e;
                *x1 = 0.0;
            } else {
                assert(d != 0.0);
                *x1 = (y2 - f) / d;
                *y1 = 0.0;
            }
        }
    }
    
    
    // Returns |M*<1,0> - M*<0,0>|, |M*<0,1> - M*<0,0>|     
    inline void GetScaling(Float *zoomx, Float *zoomy) const
    {
        /*        
        Float ax = mat[0];
        Float ay = mat[4];
        
        Float bx = mat[1];
        Float by = mat[5];

        *zoomx = sqrt(ax*ax + ay*ay);
        *zoomy = sqrt(bx*bx + by*by);

        // old
        *zoomx = mat[0] + mat[1];
        *zoomy = mat[4] + mat[5];
        
        */
        
        *zoomx = sqrt(mat[0]*mat[0] + mat[4]*mat[4]);
        *zoomy = sqrt(mat[1]*mat[1] + mat[5]*mat[5]);
    }
    
    inline void SetIdentity()
    {
        MakeIdentityMatrix(mat);
    }
    
    inline void Set(Float *m)
    {
        CopyMatrix(mat, m);
    }
    
    Float mat[16];
};


typedef TransformMatrixTemp<Float> TransformMatrix;

extern TransformMatrix g_transformIdentity;

} // namespace Summon


#endif
