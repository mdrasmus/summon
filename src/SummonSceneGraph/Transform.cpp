/***************************************************************************
* Summon
* Matt Rasmussen
* Transform.cpp
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
            MakeRotateMatrix(param1, m_matrix.mat);            
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
    m_kind = header;
    
    switch (m_kind) {
        case TRANSLATE_CONSTRUCT:
        case SCALE_CONSTRUCT:
        case FLIP_CONSTRUCT: {
            Scm first  = code.GetScm(0);
            Scm second = code.GetScm(1);

            if (!ScmFloatp(first) || !ScmFloatp(second)) {
                Error("Expected first and second arguments to be floats");
                return false;
            }

            Set(m_kind, Scm2Float(first), Scm2Float(second));

            Scm code2 = code.Slice(2);
            return Element::Build(header, code2);
            }

        case ROTATE_CONSTRUCT: {
            Scm first  = code.GetScm(0);

            if (!ScmFloatp(first)) {
                Error("Expected first argument to be a float");
                return false;
            }
            
            Set(m_kind, Scm2Float(first));

            Scm code2 = code.Slice(1);
            return Element::Build(header, code2);
            }
            
        default:
            Error("Unknown transform");
            assert(0);
    }
    
    return false;
}


bool Transform::SetContents(const Scm &code)
{
    switch (m_kind) {
        case TRANSLATE_CONSTRUCT:
        case SCALE_CONSTRUCT:
        case FLIP_CONSTRUCT: {
            Scm first  = code.GetScm(0);
            Scm second = code.GetScm(1);

            if (!ScmFloatp(first) || !ScmFloatp(second)) {
                Error("Expected first and second arguments to be floats");
                return false;
            }

            Set(m_kind, Scm2Float(first), Scm2Float(second));
            return true;
            }

        case ROTATE_CONSTRUCT: {
            Scm first  = code.GetScm(0);

            if (!ScmFloatp(first)) {
                Error("Expected first argument to be a float");
                return false;
            }
            
            Set(m_kind, Scm2Float(first));
            return true;
            }
            
        default:
            Error("Unknown transform");
            assert(0);
    }
    
    // NOTE: should never get here
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
                             const TransformMatrix *matrix,
                             const Camera &camera)
{
    TransformMatrix matrix2;
    MultMatrix(matrix->mat, GetMatrix(), matrix2.mat);

    // loop through children of this element
    for (Element::Iterator i=Begin(); i!=End(); i++) {
        (*i)->FindBounding(top, bottom, left, right, &matrix2, camera);
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
