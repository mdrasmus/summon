/***************************************************************************
* Summon
* Matt Rasmussen
* Controller.h
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

#ifndef SUMMON_CONTROLLER_H
#define SUMMON_CONTROLLER_H

#include "Binding.h"
#include "Command.h"
#include "Input.h"

namespace Summon
{

class Controller : public CommandExecutor {
public:
    Controller() :
        m_binding(&m_defaultBinding) {}
    virtual ~Controller() {}
   
    virtual void ExecCommand(Command &command) {}
    
    inline virtual void ProcessInput(Input &input) {
        list<Command*> cmds = m_binding->GetCommand(input);
        
        for (list<Command*>::iterator i=cmds.begin(); i!=cmds.end(); i++) {
            ExecCommand(*(*i));
            delete (*i);
        }
    }
    
    inline Binding *GetBinding()
    { return m_binding; }
    
    inline void SetBinding(Binding *binding)
    { m_binding = binding; }

protected:
    Binding m_defaultBinding;
    Binding *m_binding;
};

}

#endif
