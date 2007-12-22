/***************************************************************************
* Summon
* Matt Rasmussen
* TransformMatrix.h
*
***************************************************************************/

#ifndef TRANSFORM_MATRIX_H
#define TRANSFORM_MATRIX_H

#include <stdio.h>

namespace Summon {


void MultMatrix(const float *mat1, const float *mat2, float *mat3);
void MakeTransMatrix(const float *vec, float *m);
void MakeRotateMatrix(float angle, const float *vec, float *m);
void MakeScaleMatrix(const float *vec, float *m);
void MakeIdentityMatrix(float *m);
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
