/***************************************************************************
* Summon
* Matt Rasmussen
* Binding.h
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

#ifndef SUMMON_BINDING_H
#define SUMMON_BINDING_H

#include <list>
#include "Command.h"
#include "Input.h"
#include "HashTable.h"

namespace Summon
{


/*
  Each input type (KEYBOARD_INPUT, SPECIAL_KEY_INPUT, MOUSE_MOTION_INPUT,
  MOUSE_CLICK_INPUT, etc) has its own HashTable mapping input hashids 
  to command lists.
 */


class Binding {
public:
    Binding();    
    virtual ~Binding();

    typedef list<Command*> CommandList;    
   
    inline void AddBinding(Input *input, CommandId commandId)
    { AddBinding(input, g_commandRegistry.Create(commandId)); }
    
    void AddBinding(Input *input, Command* commandId);
    void ClearBinding(Input &input);
    
    CommandList GetCommand(Input &input);
    inline vector<Input*> *GetInputs() { return &m_inputs; }
    inline void Clear() {
        for (unsigned int i=0; i<m_bindings.size(); i++)
            delete m_bindings[i];
        for (unsigned int i=0; i<m_inputs.size(); i++)
            delete m_inputs[i];
        for (unsigned int i=0; i<m_cmds.size(); i++)
            delete m_cmds[i];
        for (unsigned int i=0; i<m_lists.size(); i++)
            delete m_lists[i];
        
        m_bindings.clear();            
        m_inputs.clear();
        m_cmds.clear();
        m_lists.clear();
    }
   
protected:
    enum { TABLE_SIZE = 257 };
    
    typedef HashTable<int, CommandList*, HashInt> BindType;
    vector<BindType*> m_bindings;
    vector<Input*> m_inputs;
    vector<Command*> m_cmds;
    vector<CommandList*> m_lists;
};

}

#endif
