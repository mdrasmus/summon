/***************************************************************************
* Summon
* Matt Rasmussen
* SummonWindow.h
*
***************************************************************************/

#ifndef DRAW_WINDOW_H
#define DRAW_WINDOW_H

#include "Script.h"
#include "Window.h"
#include "SummonController.h"
#include "SummonModel.h"
#include "SummonView.h"


namespace Summon
{



class SummonWindow
{
public:
    SummonWindow(int id, CommandExecutor *global,
               int width=400, int height=400, const char *name="",
               int left=-1, int top=-1) :
        m_id(id),
        m_global(global),
        m_view(NULL, width, height, name, left, top),
        m_controller(global, &m_view),
        m_worldModel(NULL),
        m_screenModel(NULL)
    {
    }
    
    virtual ~SummonWindow() {}
   
    void ExecCommand(Command &cmd)
    {}

    inline void SetActive(bool b = true) { m_view.SetActive(b); }
    
    void SetWorldModel(SummonModel *model)
    {
        // detach old model from view
        if (m_worldModel) {
            m_worldModel->RemoveView(&m_view);
        }
        
        m_worldModel = model;
        m_view.SetWorldModel(model);
        m_controller.SetWorldModel(model);
        if (model) {
            model->AddView(&m_view);
            model->SetKind(MODEL_WORLD);
        }
        m_view.Redisplay();
    }

    void SetScreenModel(SummonModel *model)
    {
        // detach old model from view
        if (m_screenModel) {
            m_screenModel->RemoveView(&m_view);
        }
    
        m_screenModel = model;
        m_view.SetScreenModel(model);
        m_controller.SetScreenModel(model);
        if (model) {
            model->AddView(&m_view);
            model->SetKind(MODEL_SCREEN);
        }
        m_view.Redisplay();
    }    
    
    inline SummonModel *GetWorldModel() { return m_worldModel; }
    inline SummonModel *GetScreenModel() { return m_screenModel; }    
    inline int GetId() { return m_id; }
    
    inline SummonView *GetView() { return &m_view; }
    inline SummonController *GetController() { return &m_controller; }
    
    inline void SetName(string name)
    { m_view.SetName((char*) name.c_str()); }
    
    inline string GetName()
    { return m_view.GetName(); }
    
    void Close()
    {
        if (m_worldModel)
            m_worldModel->RemoveView(&m_view);
        if (m_screenModel)
            m_screenModel->RemoveView(&m_view);
        m_view.Close();
    }
    
protected:
    int m_id;
    CommandExecutor *m_global;
    SummonView m_view;
    SummonController m_controller;
    SummonModel *m_worldModel;
    SummonModel *m_screenModel;
};

}

#endif
