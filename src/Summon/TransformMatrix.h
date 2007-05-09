/***************************************************************************
* Vistools
* Matt Rasmussen
* TransformMatrix.h
*
***************************************************************************/

#ifndef TRANSFORM_MATRIX_H
#define TRANSFORM_MATRIX_H



namespace Vistools {

using namespace std;


void MultMatrix(float *mat1, float *mat2, float *mat3);
void MakeTransMatrix(float *vec, float *m);
void MakeRotateMatrix(float angle, float *vec, float *m);
void MakeScaleMatrix(float *vec, float *m);
void MakeIdentityMatrix(float *m);
void CopyMatrix(float *des, float *src);


class TransformMatrix
{
public:
    inline void VecMult(float x1, float y1, float *x2, float *y2)
    {
        *x2 = x1*mat[0] + y1*mat[1] + mat[3];
        *y2 = x1*mat[4] + y1*mat[5] + mat[7];
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


}


#endif
