/***************************************************************************
* Vistools
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


namespace Vistools
{

class DrawController : public Glut2DController
{
public:
    DrawController(CommandExecutor *global, DrawView *view, DrawModel *model);
    virtual ~DrawController();
   
    void ExecCommand(Command &command);
    void HotspotClick(Vertex2i pos);
   
    inline void SetModel(DrawModel *model) { m_model = model; }
   
protected:
    CommandExecutor *m_global;
    DrawView *m_view;
    DrawModel *m_model;
};

}

#endif
