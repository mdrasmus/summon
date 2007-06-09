/***************************************************************************
* Summon
* Matt Rasmussen
* DrawController.h
*
***************************************************************************/

#ifndef DRAW_CONTROLLER_H
#define DRAW_CONTROLLER_H

#include "Script.h"
#include "Glut2DController.h"
#include "drawCommands.h"
#include "DrawModel.h"
#include "DrawView.h"


namespace Summon
{

class DrawController : public Glut2DController, DrawViewListener
{
public:
    DrawController(CommandExecutor *global, DrawView *view, 
                   DrawModel *world=NULL,
                   DrawModel *screen=NULL);
    virtual ~DrawController();
   
    void ExecCommand(Command &command);
    void HotspotClick(Vertex2i pos);
   
    inline void SetWorldModel(DrawModel *model) { m_world = model; }
    inline void SetScreenModel(DrawModel *model) { m_screen = model; }    
   
    virtual void Motion(int x, int y);
   
protected:

    virtual void ViewResize(DrawView *view);

    CommandExecutor *m_global;
    DrawView *m_view;
    DrawModel *m_world;
    DrawModel *m_screen;    
    Scm m_resizeProc;
};

}

#endif
