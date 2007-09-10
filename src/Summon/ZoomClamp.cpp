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
    if (!ParseScm("Bad format for zoom_clamp construct", code,
                  "ffffbb", &minx, &miny, &maxx, &maxy, &clip, &linked))
        return false;
    
    return Element::Build(header, ScmCdddr(ScmCdddr(code)));
}


Scm ZoomClamp::GetContents()
{
    return Py2ScmTake(Py_BuildValue("ffffbb", minx, miny, maxx, maxy, 
                                    clip, linked));
}


} // namespace Summon
