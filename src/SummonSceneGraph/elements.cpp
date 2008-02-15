/***************************************************************************
* Summon
* Matt Rasmussen
* elements.cpp
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
#include "Element.h"
#include "Transform.h"
#include "Element.h"
#include "Graphic.h"
#include "Group.h"
#include "Hotspot.h"
#include "Transform.h"
#include "TextElement.h"
#include "ZoomClamp.h"


namespace Summon {



ElementFactory g_elementFactory;


void elementsInit()
{

    // structure
    g_elementFactory.Register(new Group(), GROUP_CONSTRUCT);
    g_elementFactory.Register(new CustomGroup(), CUSTOM_GROUP_CONSTRUCT);
    g_elementFactory.Register(new Hotspot(), HOTSPOT_CONSTRUCT);
    
    // graphics
    g_elementFactory.Register(new Graphic(), POINTS_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), LINES_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), LINE_STRIP_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), TRIANGLES_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), TRIANGLE_STRIP_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), TRIANGLE_FAN_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), QUADS_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), QUAD_STRIP_CONSTRUCT);
    g_elementFactory.Register(new Graphic(), POLYGON_CONSTRUCT);
    g_elementFactory.Register(new TextElement(), TEXT_CONSTRUCT);
    g_elementFactory.Register(new TextElement(), TEXT_SCALE_CONSTRUCT);
    g_elementFactory.Register(new TextElement(), TEXT_CLIP_CONSTRUCT);
    
    // primitives
    //g_elementFactory.Register(new (VerticesConstruct)
    g_elementFactory.Register(new Graphic(), COLOR_CONSTRUCT);
    
    // transforms
    g_elementFactory.Register(new Transform(), TRANSLATE_CONSTRUCT);
    g_elementFactory.Register(new Transform(), ROTATE_CONSTRUCT);
    g_elementFactory.Register(new Transform(), SCALE_CONSTRUCT);
    g_elementFactory.Register(new Transform(), FLIP_CONSTRUCT);
    g_elementFactory.Register(new ZoomClamp(), ZOOM_CLAMP_CONSTRUCT);
}


}
