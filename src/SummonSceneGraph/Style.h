/***************************************************************************
* Summon
* Matt Rasmussen
* Style.h
*
* This class contains all style attributes in a summon group.
*   - color
*   - line width
*   - point size
*   - stipple pattern
*   - etc
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

#ifndef SUMMON_STYLE_H
#define SUMMON_STYLE_H

#include "Color.h"


namespace Summon {

using namespace std;

class Style
{
public:
    Style()
    {}

    Style(const Color &color) :
        color(color)
    {}
    
    Color color;
};

} // namespace Summon

#endif
