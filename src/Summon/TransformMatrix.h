/***************************************************************************
* Summon
* Matt Rasmussen
* TransformMatrix.h
*
***************************************************************************/

#ifndef TRANSFORM_MATRIX_H
#define TRANSFORM_MATRIX_H

#include <stdio.h>
#include <assert.h>

namespace Summon {


void MultMatrix(const float *mat1, const float *mat2, float *mat3);
void MakeTransMatrix(const float *vec, float *m);
void MakeRotateMatrix(float angle, const float *vec, float *m);

// Assume a vector of <0, 0, 1>
void MakeRotateMatrix(float angle, float *m);

void MakeScaleMatrix(const float *vec, float *m);
void MakeIdentityMatrix(float *m);
void MakeTransScaleMatrix(const float *trans, const float *zoom, float *m);
void CopyMatrix(float *des, const float *src);
void PrintMatrix(const float *mat, FILE *out=stdout);


class TransformMatrix
{
public:
    TransformMatrix() {}
    TransformMatrix(bool initIdentity) {
        if (initIdentity)
            SetIdentity();
    }

    inline void VecMult(float x1, float y1, float *x2, float *y2) const 
    {
        *x2 = x1*mat[0] + y1*mat[1] + mat[3];
        *y2 = x1*mat[4] + y1*mat[5] + mat[7];
    }
    
    inline void VecInvMult(float x2, float y2, float *x1, float *y1) const 
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
    
        const float a = mat[0];
        const float b = mat[1];
        const float c = mat[3];
        const float d = mat[4];
        const float e = mat[5];
        const float f = mat[7];
        
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

    // Returns the vector transform(<1,1>)-transform(<0,0>)
    inline void GetScaling(float *zoomx, float *zoomy) const
    {
        *zoomx = mat[0] + mat[1];
        *zoomy = mat[4] + mat[5];
    }
    
    inline void SetIdentity()
    {
        MakeIdentityMatrix(mat);
    }
    
    inline void Set(float *m)
    {
        CopyMatrix(mat, m);
    }
    
    float mat[16];
};


extern TransformMatrix g_transformIndentity;

} // namespace Summon


#endif
