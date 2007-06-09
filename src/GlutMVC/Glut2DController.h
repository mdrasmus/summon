/***************************************************************************
* Summon
* Matt Rasmussen
* Glut2DController.h
*
***************************************************************************/

#ifndef GLUT_2D_CONTROLLER_H
#define GLUT_2D_CONTROLLER_H

#include "Glut2DView.h"
#include "GlutController.h"
#include "glut2DCommands.h"
#include "types.h"


namespace Summon
{

class Glut2DController : public GlutController {
public:
   Glut2DController(Glut2DView* view);
   virtual ~Glut2DController();

   virtual void ExecCommand(Command &command);

private:   
   Glut2DView *m_view;
};

}

#endif
