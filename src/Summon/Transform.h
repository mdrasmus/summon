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
    Transform(int kind=-1, float param1 = 0.0, float param2 = 0.0);
    
    virtual int GetSpecificId()
    { return m_kind; }
    
    virtual Element *Create()
    { return new Transform(); }
    
    inline int GetKind() { return m_kind; }
    inline float GetParam1() { return m_param1; }    
    inline float GetParam2() { return m_param2; }    
    inline float *GetMatrix() { return m_matrix; }
    inline void SetMatrix(float *mat)
    {
        CopyMatrix(m_matrix, mat);
    }
    
    void Set(int kind, float param1, float param2 = 0.0);
    
    virtual bool Build(int header, const Scm &code);
    virtual Scm GetContents();
    
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
