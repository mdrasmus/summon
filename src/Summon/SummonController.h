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

class SummonController : public Glut2DController, SummonViewListener
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
   
    virtual void Motion(int x, int y);
   
protected:

    virtual void ViewResize(SummonView *view);

    CommandExecutor *m_global;
    SummonView *m_view;
    SummonModel *m_world;
    SummonModel *m_screen;    
    Scm m_resizeProc;
};

}

#endif
