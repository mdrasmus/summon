/***************************************************************************
* Vistools
* Matt Rasmussen
* Transform.h
*
***************************************************************************/

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Script.h"
#include "Element.h"


namespace Vistools {

using namespace std;


void MultMatrix(float *mat1, float *mat2, float *mat3);
void MakeTransMatrix(float *vec, float *m);
void MakeRotateMatrix(float angle, float *vec, float *m);
void MakeScaleMatrix(float *vec, float *m);
void MakeIdentityMatrix(float *m);
void CopyMatrix(float *des, float *src);

inline bool IsTransform(int header)
{
    switch (header) {
        case TRANSFORM_CONSTRUCT:
        case TRANSLATE_CONSTRUCT:
        case ROTATE_CONSTRUCT:
        case SCALE_CONSTRUCT:        
        case FLIP_CONSTRUCT:
            return true;
        default:
            return false;
    }
}

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



class Transform : public Element
{
public:
    Transform(int kind, float param1, float param2 = 0.0);
    
    inline int GetKind() { return m_kind; }
    inline float GetParam1() { return m_param1; }    
    inline float GetParam2() { return m_param2; }    
    inline float *GetMatrix() { return m_matrix; }
    inline void SetMatrix(float *mat)
    {
        CopyMatrix(m_matrix, mat);
    }

    
protected:
    int m_kind;
    float m_param1;
    float m_param2;
    float m_matrix[16];
};




}

#endif
