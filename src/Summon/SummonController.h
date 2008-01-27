/***************************************************************************
* Summon
* Matt Rasmussen
* SummonController.h
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

#ifndef SUMMON_SUMMON_CONTROLLER_H
#define SUMMON_SUMMON_CONTROLLER_H

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
