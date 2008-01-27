/***************************************************************************
* Summon
* Matt Rasmussen
* Window.h
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

#ifndef SUMMON_WINDOW_H
#define SUMMON_WINDOW_H

#include "Command.h"

namespace Summon
{

template<class ModelT, class ViewT, class ControllerT>
class Window
{
public:
    Window(CommandExecutor *global, ModelT *model) :
        m_global(global),
        m_view(model),
        m_controller(global, &m_view, model),
        m_model(model)
    {
        if (model) {
            model->AddView(&m_view);
        }
    }
    
    virtual ~Window() {}
   
    void ExecCommand(Command &cmd)
    { m_controller.ExecCommand(cmd); }

    inline void SetActive(bool b = true) { m_view.SetActive(b); }

    void SetModel(ModelT *model)
    {
        m_view.SetModel(model);
        m_controller.SetModel(model);
        if (model) {            
            model->AddView(&m_view);
        }
    }
    
    inline ModelT *GetModel() { return m_model; }
    
protected:
    CommandExecutor *m_global;
    ViewT m_view;
    ControllerT m_controller;
    ModelT *m_model;
};

}

#endif
