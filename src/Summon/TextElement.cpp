/***************************************************************************
* Summon
* Matt Rasmussen
* TextElement.cpp
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
#include "TextElement.h"

namespace Summon {



bool TextElement::Build(int header, const Scm &code2)
{
    //int header;
    float x1, y1, x2, y2;
    if (!ParseScm(code2, "sffff", &text, &x1, &y1, &x2, &y2))
    {
        Error("Bad format for text construct");
        return false;
    }
    
    Scm code = code2.Slice(5); //ScmCddr(ScmCdddr(code2));
    
    switch (header) {
        case TEXT_CONSTRUCT:        kind = KIND_BITMAP; break;
        case TEXT_SCALE_CONSTRUCT:  kind = KIND_SCALE;  break;
        case TEXT_CLIP_CONSTRUCT:   kind = KIND_CLIP;   break;
        default:
            Error("unknown TextElement kind: %d", header);
            return false;
    }
    
    // clip text takes two more arguments
    if (kind == KIND_CLIP) {
        if (!ParseScm(code, "ff", &minHeight, &maxHeight)) {
            Error("Bad format for text construct");
            return false;
        }
        code = code.Slice(2);
    }
    
    
    // parse justification
    justified = 0; 
    
    for (int i=0; i<code.Size(); i++) {
        string str = Scm2String(code.GetScm(i));
                
        if (str == "left")          justified |= TextElement::LEFT;
        else if (str == "center")   justified |= TextElement::CENTER;
        else if (str == "right")    justified |= TextElement::RIGHT;
        else if (str == "top")      justified |= TextElement::TOP;
        else if (str == "middle")   justified |= TextElement::MIDDLE;
        else if (str == "bottom")   justified |= TextElement::BOTTOM;
        else if (str == "vertical") justified |= TextElement::VERTICAL;
        else {
            Error("unknown justification '%s'", str.c_str());
            return false;
        }
    }
    
    // apply default justification
    if (justified == 0) {
        justified = TextElement::LEFT | TextElement::MIDDLE;
    }

    
    // set bounding rectangle
    Vertex2f pos1(x1, y1), pos2(x2, y2);
    SetRect(pos1, pos2);

    return true;
}


Scm TextElement::GetContents()
{
    Scm just = Scm_EOL;

    if (justified & LEFT)
        just = ScmCons(String2Scm("left"), just);
    if (justified & CENTER)
        just = ScmCons(String2Scm("center"), just);
    if (justified & RIGHT)
        just = ScmCons(String2Scm("right"), just);
    if (justified & TOP)
        just = ScmCons(String2Scm("top"), just);
    if (justified & MIDDLE)
        just = ScmCons(String2Scm("middle"), just);
    if (justified & BOTTOM)
        just = ScmCons(String2Scm("bottom"), just);
    if (justified & VERTICAL)
        just = ScmCons(String2Scm("vertical"), just);

    int id = 0;

    switch (kind) {
        case KIND_BITMAP:
            id = TEXT_CONSTRUCT;
            break;
        case KIND_SCALE:
            id = TEXT_SCALE_CONSTRUCT;
            break;
        case KIND_CLIP:
            id = TEXT_CLIP_CONSTRUCT;

            // add extra height fields                
            just = ScmCons(Float2Scm(minHeight),
                    ScmCons(Float2Scm(maxHeight), just));
            break;
        default:
            assert(0);
    }
    
    return ScmAppend(BuildScm("dsffff", id, text.c_str(), 
                                        pos1.x, pos1.y, pos2.x, pos2.y),
                    just);

}


} // namespace Summon
