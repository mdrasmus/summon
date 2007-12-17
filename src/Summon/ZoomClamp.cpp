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
    if (!ParseScm(code, "ffffbb", &minx, &miny, &maxx, &maxy, &clip, &linked)) {
        Error("Bad format for zoom_clamp construct");
        return false;
    }
    
    return Element::Build(header, ScmCdddr(ScmCdddr(code)));
}


Scm ZoomClamp::GetContents()
{
    return BuildScm("ffffbb", minx, miny, maxx, maxy, clip, linked);
}


} // namespace Summon
