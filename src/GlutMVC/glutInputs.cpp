/***************************************************************************
* Vistools
* Matt Rasmussen
* glutInputs.cpp
*
***************************************************************************/

#include "first.h"
#include "glutInputs.h"

namespace Vistools
{

void glutInputsInit()
{
    RegisterInput(KeyboardInput)
    RegisterInput(SpecialKeyInput)
    RegisterInput(MouseMotionInput)
    RegisterInput(MouseClickInput)
}

}



