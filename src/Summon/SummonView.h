/***************************************************************************
* Summon
* Matt Rasmussen
* SummonView.h
*
***************************************************************************/

#ifndef DRAW_VIEW_H
#define DRAW_VIEW_H

#include "Script.h"
#include <string>
#include <vector>
#include "Color.h"
#include "Command.h"
#include "Glut2DView.h"
#include "SummonModel.h"
#include "summonCommands.h"
#include "Group.h"
#include "Graphic.h"
#include "TextElement.h"
#include "Transform.h"


namespace Summon
{

using namespace std;

enum {
    TASK_DRAWLIST,
    TASK_ELEMENT
};

class DrawTask
{
public:
    DrawTask(Element *elm) :
        m_id(TASK_ELEMENT),
        m_element(elm),
        m_open(true)
    {}
    
    DrawTask(int drawlist) :
        m_id(TASK_DRAWLIST),
        m_drawlist(drawlist),
        m_open(true)
    {
        glNewList(drawlist, GL_COMPILE_AND_EXECUTE);
    }
    
    ~DrawTask()
    {
        if (m_id == TASK_DRAWLIST) {
            glDeleteLists(m_drawlist, 1);
        }
    }
    
    inline int GetId() { return m_id; }
    
    void Close()
    {
        m_open = false;
        if (m_id == TASK_DRAWLIST)
            glEndList();
    }
    
    inline bool IsOpen() { return m_open; }
    
    int m_id;
    Element *m_element;
    int m_drawlist;
    bool m_open;
};


class SummonView : public Glut2DView 
{
public:
    SummonView(SummonModel *model, int width = 320, int height = 320, 
               const char *name="", int left=-1, int top=-1);
    virtual ~SummonView();

    virtual void ExecCommand(Command &command);

    inline void SetActive(bool b = true) { m_active = b; }
    void SetBgColor(Color &color);
    
    inline void SetWorldModel(SummonModel *model) { m_worldModel = model; }
    inline void SetScreenModel(SummonModel *model) { m_screenModel = model; }
    
    inline void Redisplay() {
        RedisplayCommand cmd;
        ExecCommand(cmd);
    }
    
    inline void NoteModelChange()
    {
        ClearTasks();
    }
    
    bool WithinView(const Vertex2f &pos1, const Vertex2f &pos2);

    void SetMousePos(int x, int y);


protected:

    // drawing methods
    virtual void Display();
    virtual void DrawWorld();
    virtual void DrawScreen();
    virtual void DrawCrosshair();
        
    void DrawElement(Element *elm, bool createTasks=true); 
    void DrawGraphic(Graphic *graphic);
    void DrawTextElement(TextElement *elm);
    
    void DrawText(void *font, string text, float x, float y);
    inline void SetColor(Color &color)
    { glColor4f(color.r, color.g, color.b, color.a); }
    
    inline void AddTask(DrawTask *task) { m_tasks.push_back(task); }
    inline void ClearTasks() {
        for (unsigned int i=0; i<m_tasks.size(); i++)
            delete m_tasks[i];
        m_tasks.clear();
    }
    inline DrawTask* GetLastTask() { return m_tasks.back(); }
    void ExecuteTasks();
    
    
    
    SummonModel *m_worldModel;
    SummonModel *m_screenModel;
    Color m_bgColor;
    bool m_active;
    vector<DrawTask*> m_tasks;
        
    // crosshair
    bool m_showCrosshair;
    Color m_crosshairColor;
    Vertex2i m_mousePos;
};

}

#endif
