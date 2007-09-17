/***************************************************************************
* Summon
* Matt Rasmussen
* SummonController.h
*
***************************************************************************/

#ifndef DRAW_CONTROLLER_H
#define DRAW_CONTROLLER_H

#include "Script.h"
#include "Glut2DController.h"
#include "summonCommands.h"
#include "SummonModel.h"
#include "SummonView.h"


namespace Summon
{

class SummonController : public Glut2DController, public GlutViewListener
{
public:
    SummonController(CommandExecutor *global, SummonView *view, 
                   SummonModel *world=NULL,
                   SummonModel *screen=NULL);
    virtual ~SummonController();
   
    void ExecCommand(Command &command);
    void HotspotClick(Vertex2i pos);
   
    inline void SetWorldModel(SummonModel *model) { m_world = model; }
    inline void SetScreenModel(SummonModel *model) { m_screen = model; }    
   
    virtual void PassiveMotion(int x, int y);

    virtual void ViewRedraw(GlutView *view) {}
    virtual void ViewResize(GlutView *view);
    virtual void ViewMove(GlutView *view);
    virtual void ViewClose(GlutView *view) {}

   
protected:

    CommandExecutor *m_global;
    SummonView *m_view;
    SummonModel *m_world;
    SummonModel *m_screen;    
    Scm m_resizeProc;
    Scm m_moveProc;
};

}

#endif
