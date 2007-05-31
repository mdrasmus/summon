/***************************************************************************
* Vistools
* Matt Rasmussen
* Binding.h
*
***************************************************************************/

#ifndef BINDING_H
#define BINDING_H

#include <list>
#include "Command.h"
#include "Input.h"
#include "HashTable.h"

namespace Vistools
{



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
        
        m_bindings.clear();            
        m_inputs.clear();
        m_cmds.clear();
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
