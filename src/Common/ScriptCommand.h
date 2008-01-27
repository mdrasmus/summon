/***************************************************************************
* Summon
* Matt Rasmussen
* ScriptCommand.h
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

#ifndef SUMMON_SCRIPT_COMMAND_H
#define SUMMON_SCRIPT_COMMAND_H

#include <set>
#include "StringCommand.h"
#include "Script.h"

namespace Summon
{


class ScriptCommand : public StringCommand
{
public:
    ScriptCommand() : m_return(Scm_NONE) {}
    virtual ~ScriptCommand() {}

    virtual bool Setup(Scm lst) { return true; }
    
    inline void SetReturn(Scm scm) { m_return = scm; }
    inline Scm GetReturn() { return m_return; }
    
protected:
    Scm m_return;
};


// script attribute
extern CommandAttr g_scriptAttr;


#define RegisterScriptCommand(cmd) \
    RegisterStringCommand(cmd) \
    AddAttr(g_scriptAttr)

}


#endif
