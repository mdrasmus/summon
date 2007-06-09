/***************************************************************************
* Summon
* Matt Rasmussen
* Transform.h
*
***************************************************************************/

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Script.h"
#include "Element.h"
#include "TransformMatrix.h"


namespace Summon {

using namespace std;


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

    void FindBounding(float *top, float *bottom, float *left, float *right,
                      TransformMatrix *matrix);
    
protected:
    int m_kind;
    float m_param1;
    float m_param2;
    float m_matrix[16];
};




}

#endif
