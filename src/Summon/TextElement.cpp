/***************************************************************************
* Summon
* Matt Rasmussen
* TextElement.cpp
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
    if (!ParseScm("Bad format for text construct", code2,
                  "sffff", &text, &x1, &y1, &x2, &y2))
        return false;
    
    Scm code = ScmCddr(ScmCdddr(code2));
    
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
        if (!ParseScm("Bad format for text construct", code,
                  "ff", &minHeight, &maxHeight))
            return false;
        code = ScmCddr(code);
    }
    
    
    // parse justification
    justified = 0; 
    for (; ScmConsp(code) && ScmStringp(ScmCar(code)); code = ScmCdr(code)) {
        string str = Scm2String(ScmCar(code));
        
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
    
    return ScmAppend(Py2ScmTake(Py_BuildValue("isffff", id, text.c_str(), 
                                              pos1.x, pos1.y, pos2.x, pos2.y)),
                    just);

}


} // namespace Summon
