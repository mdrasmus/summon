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
        case ZOOM_CLAMP_CONSTRUCT:
            return true;
        default:
            return false;
    }
}



class Transform : public Element
{
public:
    // NOTE: the bool in this constructor is just a place holder to prevent
    // a ambiguous definition bewteen this constructor and the next one.
    // Used by ZoomClamp
    Transform(int id, bool placeHolder) :
        Element(id)
    {}
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
    
    
    virtual bool Build(int header, const Scm &code);
    virtual Scm GetContents();
    
    void FindBounding(float *top, float *bottom, float *left, float *right,
                      TransformMatrix *matrix);
    
    TransformMatrix &GetTransform(TransformMatrix &matrix,
                                  const Vertex2f &cameraZoom);
    
protected:
    virtual Element *GetTransformParent()
    {
        return this;
    }


    void Set(int kind, float param1, float param2=0.0);

    int m_kind;
    float m_param1;
    float m_param2;
    float m_matrix[16];
};




}

#endif
