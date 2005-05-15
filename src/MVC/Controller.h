/***************************************************************************
* Vistools
* Matt Rasmussen
* Controller.h
*
***************************************************************************/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Binding.h"
#include "Command.h"
#include "Input.h"

namespace Vistools
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
