/***************************************************************************
* Summon
* Matt Rasmussen
* glutInputs.cpp
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
#include "glutInputs.h"

namespace Summon
{

void glutInputsInit()
{
    RegisterInput(KeyboardInput)
    RegisterInput(SpecialKeyInput)
    RegisterInput(MouseMotionInput)
    RegisterInput(MouseClickInput)
}

}



