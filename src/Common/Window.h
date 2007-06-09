/***************************************************************************
* Summon
* Matt Rasmussen
* Window.h
*
***************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

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
