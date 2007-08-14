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



bool TextElement::Build(const Scm &code2)
{
    int header;
    float x1, y1, x2, y2;
    if (!ParseScm("Bad format for text construct", code2,
                  "sffff", &header, &text, &x1, &y1, &x2, &y2))
        return false;
    
    Scm code = ScmCddr(ScmCdddr(code2));
    
    switch (header) {
        case TEXT_CONSTRUCT: 
            kind = KIND_BITMAP;
            break;

        case TEXT_SCALE_CONSTRUCT: 
            kind = KIND_SCALE;
            break;

        case TEXT_CLIP_CONSTRUCT: 
            kind = KIND_CLIP;
            break;
        
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



} // namespace Summon
