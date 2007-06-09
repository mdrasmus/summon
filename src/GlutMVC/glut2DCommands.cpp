/***************************************************************************
* Summon
* Matt Rasmussen
* glut2DCommands.cpp
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

