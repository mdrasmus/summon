/***************************************************************************
* Summon
* Matt Rasmussen
* glut2DCommands.cpp
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
#include "glut2DCommands.h"
#include "glutInputs.h"

namespace Summon
{

void glutCommandsInit() {
    glutInputsInit();
    
    RegisterCommand(RedisplayCommand)
    RegisterStringCommand(TranslateCommand)
    RegisterStringCommand(ZoomCommand)
    RegisterStringCommand(ZoomXCommand)
    RegisterStringCommand(ZoomYCommand)
    RegisterStringCommand(FocusCommand)
}

}

