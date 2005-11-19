/***************************************************************************
* Vistools
* Matt Rasmussen
* DrawView.h
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
#include "DrawModel.h"
#include "drawCommands.h"
#include "GroupTable.h"
#include "Graphic.h"
#include "TextElement.h"
#include "Transform.h"


namespace Vistools
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
        m_element(elm)
    {}
    
    DrawTask(int drawlist) :
        m_id(TASK_DRAWLIST),
        m_drawlist(drawlist)
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
        if (m_id == TASK_DRAWLIST)
            glEndList();
    }
    
    int m_id;
    Element *m_element;
    int m_drawlist;
};

class DrawView;
class DrawViewListener 
{
public:
    virtual void Redraw(DrawView *view) {}
};


class DrawView : public Glut2DView 
{
public:
    DrawView(DrawModel *model, int width = 320, int height = 320);
    virtual ~DrawView();

    virtual void ExecCommand(Command &command);

    inline void SetActive(bool b = true) { m_active = b; }
    void SetBgColor(Color &color);
    void Home();
    
    inline void SetWorldModel(DrawModel *model) { m_worldModel = model; }
    inline void SetScreenModel(DrawModel *model) { m_screenModel = model; }
    inline void SetListener(DrawViewListener *listener)
    { m_listener = listener; }

    inline void Redisplay() {
        RedisplayCommand cmd;
        ExecCommand(cmd);
    }
    
    inline void NoteModelChange()
    {
        ClearTasks();
    }
    
    bool WithinView(const Vertex2f &pos1, const Vertex2f &pos2);


protected:

    // drawing methods
    virtual void DrawWorld();
    virtual void DrawScreen();
    
    void DrawElement(GroupTable *table, Element *elm); 
    void DrawGraphic(Graphic *graphic);
    /*
    void DrawPrimitives(Graphic::PrimitiveIterator begin, 
                        Graphic::PrimitiveIterator end);
    */
    void DrawTextElement(TextElement *elm);
    
    void DrawText(void *font, string text, float x, float y);
    inline void SetColor(Color &color)
    { glColor4f(color.r, color.g, color.b, color.a); }
    
    inline void AddTask(DrawTask *task) { m_tasks.push_back(task); }
    inline void ClearTasks() {
        for (int i=0; i<m_tasks.size(); i++)
            delete m_tasks[i];
        m_tasks.clear();
    }
    inline DrawTask* GetLastTask() { return m_tasks.back(); }
    void ExecuteTasks();
    inline bool IsExecutingTasks() { return m_executingTasks; }
    
    DrawModel *m_worldModel;
    DrawModel *m_screenModel;
    Color m_bgColor;
    bool m_active;
    vector<DrawTask*> m_tasks;
    bool m_executingTasks;
    DrawViewListener *m_listener;
};

}

#endif
