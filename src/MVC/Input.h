/***************************************************************************
* Summon
* Matt Rasmussen
* Input.h
*
***************************************************************************/

#ifndef INPUT_H
#define INPUT_H

#include "Factory.h"

namespace Summon
{

typedef int InputId;

enum {
    NULL_INPUT = -1
};

class Input {
public:
    Input() {}
    virtual ~Input() {}
    
    virtual Input* Create() { return new Input(); }
    virtual InputId GetId() { return NULL_INPUT; }
    virtual int GetHash() { return 0; }
    virtual const char *GetString() { return ""; }
};

typedef Factory<InputId, Input> InputRegistry;

extern InputRegistry g_inputRegistry;

#define RegisterInput(input) \
    static input input ## _c; \
    static InputRegistry::Registrator \
        input ## _r(g_inputRegistry, &input ## _c, input ## _c.GetId());


}

#endif
