/***************************************************************************
* Summon
* Matt Rasmussen
* SummonView.h
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

#ifndef SUMMON_SUMMON_VIEW_H
#define SUMMON_SUMMON_VIEW_H

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
#include "Style.h"
#include "TextElement.h"
#include "Transform.h"
#include "ZoomClamp.h"


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
    

    
    void SetBoundary(const Vertex2f &pos1, const Vertex2f &pos2);
    bool WithinView(const Vertex2f &pos1, const Vertex2f &pos2);

    void SetMousePos(int x, int y);

    
    inline void UpdateCamera()
    {
        m_camera.trans = m_trans;
        m_camera.zoom = m_zoom;
        m_camera.focus = m_focus;
    }
    
    inline Camera GetCamera()
    {
        return Camera(m_trans, m_zoom, m_focus);
    }
    
    void ProcessRedisplay()
    {
        if (m_needRedisplay) {
            MakeCurrentWindow();
            glutPostRedisplay();
            m_needRedisplay = false;
        }
    }    

protected:

    // drawing methods
    virtual void Display();
    virtual void DrawWorld();
    virtual void DrawScreen();
    virtual void DrawCrosshair();

    inline void NoteModelChange()
    {
        ClearTasks();
    }
        
    void DrawElement(Element *elm, const Style &style, bool createTasks=true); 
    void DrawGraphic(Graphic *graphic);
    void DrawTextElement(TextElement *elm);
    bool DrawZoomClamp(ZoomClamp *zoomClamp);
    
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
    
    void CheckBoundary(bool useZoomx=true, bool useZoomy=true);
    
    void PostRedisplay()
    {
        m_needRedisplay = true;
    }   
    
    SummonModel *m_worldModel;
    SummonModel *m_screenModel;
    Color m_bgColor;
    bool m_active;
    vector<DrawTask*> m_tasks;
    
    Camera m_camera;
    
    // crosshair
    bool m_showCrosshair;
    Color m_crosshairColor;
    Vertex2i m_mousePos;
    
    // boundary
    Vertex2f m_boundary1;
    Vertex2f m_boundary2;
    
    bool m_needRedisplay;
    bool m_styleChange;
    Style m_defaultStyle;
    Style m_curStyle;
};

}

#endif
