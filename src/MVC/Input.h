/***************************************************************************
* Summon
* Matt Rasmussen
* Input.h
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

#ifndef SUMMON_INPUT_H
#define SUMMON_INPUT_H

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
