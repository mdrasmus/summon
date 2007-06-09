/***************************************************************************
* Summon
* Matt Rasmussen
* View.h
*
***************************************************************************/

#ifndef VIEW_H
#define VIEW_H

#include "Command.h"

namespace Summon
{

class View : public CommandExecutor {
public:
   View() {}
   virtual ~View() {}
};

}

#endif

