/***************************************************************************
* Summon
* Matt Rasmussen
* glutInputs.cpp
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



