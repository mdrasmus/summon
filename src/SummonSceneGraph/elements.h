/***************************************************************************
* Summon
* Matt Rasmussen
* elements.h
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

#ifndef SUMMON_ELEMENTS_H
#define SUMMON_ELEMENTS_H

#include "Factory.h"

namespace Summon
{

typedef unsigned char ElementId;

// constructs
enum {    
    // structure
    GROUP_CONSTRUCT,
    CUSTOM_GROUP_CONSTRUCT,
    DYNAMIC_GROUP_CONSTRUCT,
    HOTSPOT_CONSTRUCT,
    
    // graphics
    POINTS_CONSTRUCT,
    LINES_CONSTRUCT,
    LINE_STRIP_CONSTRUCT,
    TRIANGLES_CONSTRUCT,
    TRIANGLE_STRIP_CONSTRUCT,
    TRIANGLE_FAN_CONSTRUCT,
    QUADS_CONSTRUCT,
    QUAD_STRIP_CONSTRUCT,
    POLYGON_CONSTRUCT,
    TEXT_CONSTRUCT,
    TEXT_SCALE_CONSTRUCT,
    TEXT_CLIP_CONSTRUCT,
    
    // primitives
    VERTICES_CONSTRUCT,
    COLOR_CONSTRUCT,
    
    // transforms
    TRANSFORM_CONSTRUCT, // internal use only
    TRANSLATE_CONSTRUCT,
    ROTATE_CONSTRUCT,
    SCALE_CONSTRUCT,
    FLIP_CONSTRUCT,
    ZOOM_CLAMP_CONSTRUCT,
    NOZOOM_CONSTRUCT,
    SIDE_ALIGN_CONSTRUCT,
    
    // inputs
    INPUT_KEY_CONSTRUCT,
    INPUT_CLICK_CONSTRUCT,
    INPUT_MOTION_CONSTRUCT,

};

void elementsInit();

// element factory and registration
class Element;
typedef FactoryArray<ElementId, Element> ElementFactory;

extern ElementFactory g_elementFactory;


} // namespace Summon

#endif
