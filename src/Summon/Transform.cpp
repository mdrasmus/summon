/***************************************************************************
* Summon
* Matt Rasmussen
* Transform.cpp
*
***************************************************************************/

#include "first.h"
#include <assert.h>
#include "Transform.h"

namespace Summon {


Transform::Transform(int kind, float param1, float param2) :
    Element(TRANSFORM_CONSTRUCT),
    m_cached(false),
    m_dynamicTransformParent(NULL)
{
    Set(kind, param1, param2);
}


void Transform::Set(int kind, float param1, float param2)
{
    m_kind = kind;
    m_param1 = param1;
    m_param2 = param2;
    
    float vec[3];
    
    switch (kind) {
        case -1:
            break;
            
        case TRANSLATE_CONSTRUCT:
            vec[0] = param1;
            vec[1] = param2;
            vec[2] = 0;
            MakeTransMatrix(vec, m_matrix.mat);
            break;
            
        case ROTATE_CONSTRUCT:
            vec[0] = 0;
            vec[1] = 0;
            vec[2] = 1;
            MakeRotateMatrix(param1, vec, m_matrix.mat);            
            break;
        
        case SCALE_CONSTRUCT:
            vec[0] = param1;
            vec[1] = param2;
            vec[2] = 0;
            MakeScaleMatrix(vec, m_matrix.mat);
            break;
        
        case FLIP_CONSTRUCT:
            vec[0] = param1;
            vec[1] = param2;
            vec[2] = 0;
            MakeRotateMatrix(180, vec, m_matrix.mat);            
            break;
        
        default:
            Error("Unknown transform");
            assert(0);
    }
}


bool Transform::Build(int header, const Scm &code)
{
    m_kind = header; //Scm2Int(ScmCar(code));
    
    switch (m_kind) {
        case TRANSLATE_CONSTRUCT:
        case SCALE_CONSTRUCT:
        case FLIP_CONSTRUCT: {
            Scm first  = ScmCar(code);
            Scm second = ScmCadr(code);

            if (!ScmFloatp(first) || !ScmFloatp(second))
                return false;

            Set(m_kind, Scm2Float(first), Scm2Float(second));

            Scm code2 = ScmCddr(code);
            return Element::Build(header, code2);
            }

        case ROTATE_CONSTRUCT: {
            Scm first  = ScmCar(code);

            if (!ScmFloatp(first))
                return false;
            
            Set(m_kind, Scm2Float(first));

            Scm code2 = ScmCdr(code);
            return Element::Build(header, code2);
            }
            
        default:
            Error("Unknown transform");
            assert(0);
    }
    
    return false;
}

Scm Transform::GetContents()
{
    switch (m_kind) {
        case TRANSLATE_CONSTRUCT:
        case SCALE_CONSTRUCT:
        case FLIP_CONSTRUCT:
            return BuildScm("dff", m_kind, m_param1, m_param2);

        case ROTATE_CONSTRUCT:
            return BuildScm("df", m_kind, m_param1);
            
        default:
            Error("Unknown transform");
            assert(0);
    }
}


void Transform::FindBounding(float *top, float *bottom, 
                             float *left, float *right,
                             TransformMatrix *matrix)
{
    TransformMatrix matrix2;
    MultMatrix(matrix->mat, GetMatrix(), matrix2.mat);

    // loop through children of this element
    for (Element::Iterator i=Begin(); i!=End(); i++) {
        (*i)->FindBounding(top, bottom, left, right, &matrix2);
    }
}


void Transform::Update()
{
    // element tree structure has probably changed.
    // delete any cached transforms
    m_cached = false;
    
    // propograte transformParent from parent
    if (m_parent == NULL) {
        m_transformParent = NULL;        
    } else {
        m_transformParent = m_parent->GetTransformParent();
    }
    
    // propogate dynamicTransformParent from transformParent
    if (m_transformParent == NULL) {
        m_dynamicTransformParent = NULL;
    } else {
        m_dynamicTransformParent = ((Transform *) m_transformParent)->GetDynamicTransformParent();
    }
}


const TransformMatrix *Transform::GetTransform(TransformMatrix *matrix,
                                               const Camera &camera)
{
    if (m_transformParent == NULL) {
        // there are no further transforms ancestral to us, thus the total 
        // transform is simply our transform
        CopyMatrix(m_matrixCache.mat, m_matrix.mat);
        m_cached = true;        
        return &m_matrix;
    } else {
        // ancestral transforms exist.  compute total transform
        
        GetCacheTransform();
        
        // transformations upto the next dynamic transform are cached.
        // apply the cached transform to the last dynamic transform

        if (m_dynamicTransformParent == NULL) {
            // there are no dynamic transforms ancestral to us, thus our
            // cached transform is the actual transform            
            return &m_matrixCache;
        } else {
            // get total transform directly from last dynamicTransform and
            // multiply it with our cached transform
            const TransformMatrix *parent = 
                m_dynamicTransformParent->GetTransform(matrix, camera);
            MultMatrix(parent->mat, m_matrixCache.mat, matrix->mat);
            return matrix;
        }
    }
    
    // NOTE: the post-condition of this function is that m_cached == True
    // and m_matrixCache is also a valid cached matrix
}



TransformMatrix *Transform::GetCacheTransform()
{
    if (!m_cached) {
        // create new cached matrix.  Use transformParent->m_matrixCache
        // to build apon running product.

        if (m_transformParent == (Element*) m_dynamicTransformParent) {
            // the parental transform is dynamic, thus we start a new 
            // running product
            CopyMatrix(m_matrixCache.mat, m_matrix.mat);
        } else {
            // multiply our transform (m_matrix.mat) onto the running
            // product
            MultMatrix(((Transform*) m_transformParent)->GetCacheTransform()->mat,
                       m_matrix.mat, m_matrixCache.mat);
        }
        
        m_cached = true;            
    }
    
    return &m_matrixCache;
}


} // namespace Summon
