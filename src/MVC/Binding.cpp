/***************************************************************************
* Vistools
* Matt Rasmussen
* Binding.cpp
*
***************************************************************************/

#include "first.h"
#include "Binding.h"

namespace Vistools
{

Binding::Binding()
{}

Binding::~Binding()
{
    for (int i=0; i<m_bindings.size(); i++)
        delete m_bindings[i];
    for (int i=0; i<m_inputs.size(); i++)
        delete m_inputs[i];
    for (int i=0; i<m_cmds.size(); i++)
        delete m_cmds[i];
    
}



void Binding::AddBinding(Input *input, Command *command)
{
    int hash = input->GetHash();

    // increase vector if necessary
    while (input->GetId() >= m_bindings.size()) {
        m_bindings.push_back(new BindType(TABLE_SIZE, (CommandList*) NULL));
    }
    
    // add command list if one does not exist for this input
    CommandList *cmdList = m_bindings[input->GetId()]->Get(hash);
    if (!cmdList) {
        cmdList = new CommandList();
        m_bindings[input->GetId()]->Insert(hash, cmdList);
        
        // remember ownership of command list
        m_lists.push_back(cmdList);
    }
        
    // get command list and append new command
    cmdList->push_back(command);
    
    // take ownership of pointers
    m_inputs.push_back(input);
    m_cmds.push_back(command);
}

list<Command*> Binding::GetCommand(Input &input)
{
    int hash = input.GetHash();
    CommandList cmds;
    
    if (input.GetId() < 0 || input.GetId() >= m_bindings.size()) {
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

