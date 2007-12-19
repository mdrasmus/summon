/***************************************************************************
* Summon
* Matt Rasmussen
* ZoomClamp.cpp
*
***************************************************************************/

#include "first.h"
#include <assert.h>
#include "ZoomClamp.h"

namespace Summon {



bool ZoomClamp::Build(int header, const Scm &code)
{
    if (!ParseScm(code, "ffffbb", &minx, &miny, &maxx, &maxy, &clip, &link)) {
        Error("Bad format for zoom_clamp construct");
        return false;
    }
    
    return Element::Build(header, ScmCdddr(ScmCdddr(code)));
}


Scm ZoomClamp::GetContents()
{
    return BuildScm("ffffbb", minx, miny, maxx, maxy, clip, link);
}


TransformMatrix &ZoomClamp::GetTransform(TransformMatrix &matrix)
{
    if (m_transformParent == this) {
        matrix.SetIdentity();
    } else {
        m_transformParent->GetTransform(matrix);
    }
    
    return matrix;
}

} // namespace Summon
