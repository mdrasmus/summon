/***************************************************************************
* Summon
* Matt Rasmussen
* Glut2DController.h
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

#ifndef SUMMON_GLUT_2D_CONTROLLER_H
#define SUMMON_GLUT_2D_CONTROLLER_H

#include "Glut2DView.h"
#include "GlutController.h"
#include "glut2DCommands.h"
#include "Vertex.h"


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
