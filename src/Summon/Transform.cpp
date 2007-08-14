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
    Element(TRANSFORM_CONSTRUCT)
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
            MakeTransMatrix(vec, m_matrix);
            break;
            
        case ROTATE_CONSTRUCT:
            vec[0] = 0;
            vec[1] = 0;
            vec[2] = 1;
            MakeRotateMatrix(param1, vec, m_matrix);            
            break;
        
        case SCALE_CONSTRUCT:
            vec[0] = param1;
            vec[1] = param2;
            vec[2] = 0;
            MakeScaleMatrix(vec, m_matrix);
            break;
        
        case FLIP_CONSTRUCT:
            vec[0] = param1;
            vec[1] = param2;
            vec[2] = 0;
            MakeRotateMatrix(180, vec, m_matrix);            
            break;
        
        default:
            Error("Unknown transform");
            assert(0);
    }
}


bool Transform::Build(const Scm &code)
{
    m_kind = Scm2Int(ScmCar(code));
    
    switch (m_kind) {
        case TRANSLATE_CONSTRUCT:
        case SCALE_CONSTRUCT:
        case FLIP_CONSTRUCT: {
            Scm first  = ScmCadr(code);
            Scm second = ScmCaddr(code);

            if (!ScmFloatp(first) || !ScmFloatp(second))
                return false;

            Set(m_kind, Scm2Float(first), Scm2Float(second));

            Scm code2 = ScmCdddr(code);
            return Element::Build(code2);
            }

        case ROTATE_CONSTRUCT: {
            Scm first  = ScmCadr(code);

            if (!ScmFloatp(first))
                return false;
            
            Set(m_kind, Scm2Float(first));

            Scm code2 = ScmCddr(code);
            return Element::Build(code2);
            }
            
        default:
            Error("Unknown transform");
            assert(0);
    }
    
    return false;
}


void Transform::FindBounding(float *top, float *bottom, 
                             float *left, float *right,
                             TransformMatrix *matrix)
{
    TransformMatrix matrix2; // = *matrix;
    MultMatrix(matrix->mat, GetMatrix(), matrix2.mat);

    // loop through children of this element
    for (Element::Iterator i=Begin(); i!=End(); i++) {
        (*i)->FindBounding(top, bottom, left, right, &matrix2);
    }
}


}
