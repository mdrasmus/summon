/***************************************************************************
* Vistools
* Matt Rasmussen
* DrawWindow.h
*
***************************************************************************/

#ifndef DRAW_WINDOW_H
#define DRAW_WINDOW_H

#include "Script.h"
#include "Window.h"
#include "DrawController.h"
#include "DrawModel.h"
#include "DrawView.h"


namespace Vistools
{

//typedef Window<DrawModel, DrawView, DrawController> DrawWindow;


class DrawWindow
{
public:
    DrawWindow(int id, CommandExecutor *global,
               int width=400, int height=400, const char *name="") :
        m_id(id),
        m_global(global),
        m_view(NULL, width, height, name),
        m_controller(global, &m_view, NULL),
        m_worldModel(NULL),
        m_screenModel(NULL)
    {
    }
    
    virtual ~DrawWindow() {}
   
    void ExecCommand(Command &cmd)
    {}

    inline void SetActive(bool b = true) { m_view.SetActive(b); }
    
    void SetWorldModel(DrawModel *model)
    {
        // detach old model from view
        if (m_worldModel) {
            m_worldModel->RemoveView(&m_view);
        }
        
        m_worldModel = model;
        m_view.SetWorldModel(model);
        m_controller.SetModel(model);
        if (model) {
            model->AddView(&m_view);
        }
        m_view.Redisplay();
    }

    void SetScreenModel(DrawModel *model)
    {
        // detach old model from view
        if (m_screenModel) {
            m_screenModel->RemoveView(&m_view);
        }
    
        m_screenModel = model;
        m_view.SetScreenModel(model);
        if (model) {
            model->AddView(&m_view);
        }
        m_view.Redisplay();
    }    
    
    inline DrawModel *GetWorldModel() { return m_worldModel; }
    inline DrawModel *GetScreenModel() { return m_screenModel; }    
    inline int GetId() { return m_id; }
    
    inline DrawView *GetView() { return &m_view; }
    inline DrawController *GetController() { return &m_controller; }
    
    inline void SetName(string name)
    { m_view.SetName((char*) name.c_str()); }
    
    inline string GetName()
    { return m_view.GetName(); }
    
    void Close()
    {
        m_view.Close();
    }
    
protected:
    int m_id;
    CommandExecutor *m_global;
    DrawView m_view;
    DrawController m_controller;
    DrawModel *m_worldModel;
    DrawModel *m_screenModel;
};

}

#endif
