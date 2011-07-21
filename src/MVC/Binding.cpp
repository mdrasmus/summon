/***************************************************************************
* Summon
* Matt Rasmussen
* Binding.cpp
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
#include "Binding.h"
#include <algorithm>



namespace Summon
{

Binding::Binding()
{
}

Binding::~Binding()
{
    Clear();
}



void Binding::AddBinding(Input *input, Command *command)
{
    // get unique id for this input (unique within input type)
    int hash = input->GetHash();

    // increase m_bindings vector if necessary
    while ((unsigned int) input->GetId() >= m_bindings.size()) {
        m_bindings.push_back(new BindType(TABLE_SIZE, (CommandList*) NULL));
    }
    
    // get list of commands bound to this input
    CommandList *cmdList = m_bindings[input->GetId()]->Get(hash);

    // add command list if one does not exist for this input
    if (!cmdList) {
        cmdList = new CommandList();
        m_bindings[input->GetId()]->Insert(hash, cmdList);
        
        // remember ownership of command list
        m_lists.push_back(cmdList);
    }
        
    // append new command
    cmdList->push_back(command);
    
    // take ownership of pointers
    m_inputs.push_back(input);
    m_cmds.push_back(command);
}


void Binding::ClearBinding(Input &input)
{
    // get unique id for this input
    int hash = input.GetHash();
    
    if (input.GetId() >= 0 && (unsigned int) input.GetId() < m_bindings.size()) {
        CommandList *commands = m_bindings[input.GetId()]->Get(hash);
        
        if (commands) {
            for (CommandList::iterator i=commands->begin(); 
                 i!= commands->end(); i++)
            {
                m_cmds.erase(find(m_cmds.begin(), m_cmds.end(), *i));
                delete (*i);                
            }
            commands->clear();
        }
    }
}



list<Command*> Binding::GetCommand(Input &input)
{
    int hash = input.GetHash();
    CommandList cmds;
    
    if (input.GetId() < 0 || (unsigned int) input.GetId() >= m_bindings.size()) {
        return cmds;
    } else {
        CommandList *commands = m_bindings[input.GetId()]->Get(hash);
        
        if (commands) {
            for (CommandList::iterator i=commands->begin(); 
                 i!= commands->end(); i++)
            {
                Command *cmd = (*i)->Create();
                cmd->Setup(input);
                cmds.push_back(cmd);
            }
            return cmds;
        } else {
            return cmds;
        }
    }
}

}

