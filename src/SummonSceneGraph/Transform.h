/***************************************************************************
* Summon
* Matt Rasmussen
* Transform.h
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

#ifndef SUMMON_TRANSFORM_H
#define SUMMON_TRANSFORM_H

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
        Element(id),
        m_cached(false),
        m_dynamicTransformParent(NULL)
    {}
    Transform(int kind=-1, float param1 = 0.0, float param2 = 0.0);
    
    virtual int GetSpecificId()
    { return m_kind; }
    
    virtual Element *Create()
    { return new Transform(); }
    
    inline int GetKind() { return m_kind; }
    inline float GetParam1() { return m_param1; }    
    inline float GetParam2() { return m_param2; }    
    inline float *GetMatrix() { return m_matrix.mat; }
    inline void SetMatrix(float *mat)
    {
        CopyMatrix(m_matrix.mat, mat);
    }
    
    
    virtual bool SetContents(const Scm &code);
    virtual bool Build(int header, const Scm &code);
    virtual Scm GetContents();
    
    virtual void FindBounding(float *top, float *bottom, float *left, float *right,
                              const TransformMatrix *matrix,
                              const Camera &camera=g_defaultCamera);
    
    virtual const TransformMatrix *GetTransform(TransformMatrix *matrix,
                                                const Camera &camera);
    
    virtual void Update();
    virtual Transform *GetTransformParent()
    {
        return this;
    }
    
    virtual Transform *GetDynamicTransformParent()
    {
        return m_dynamicTransformParent;
    }
    
    TransformMatrix *GetCacheTransform();
    
    
protected:
    

    void Set(int kind, float param1, float param2=0.0);

    int m_kind;
    float m_param1;
    float m_param2;
    bool m_cached;
    TransformMatrix m_matrix;
    TransformMatrix m_matrixCache;
    Transform *m_dynamicTransformParent;
};




}

#endif
