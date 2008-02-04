/***************************************************************************
* Summon
* Matt Rasmussen
* GlutController.h
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

#ifndef SUMMON_GLUT_CONTROLLER_H
#define SUMMON_GLUT_CONTROLLER_H

#include "Controller.h"
#include "GlutView.h"
#include "Vertex.h"

namespace Summon
{

class GlutController : public Controller {
public:
    GlutController(int window);
    virtual ~GlutController();

    void AddKeyBinding(char key, CommandId cmd);
    Vertex2i GetMousePos();
   
    void SetClickTime(long millisecs) {
        m_clickTime = millisecs;
    }
   
    // glut callbacks
    static void GlutKey(unsigned char key, int x, int y);
    static void GlutSpecialKey(int key, int x, int y);
    static void GlutMotion(int x, int y);
    static void GlutPassiveMotion(int x, int y);
    static void GlutMouseClick(int button, int state, int x, int y);

protected:
    // callbacks
    virtual void Key(unsigned char key, int x, int y);
    virtual void SpecialKey(int key, int x, int y);
    virtual void Motion(int x, int y);
    virtual void PassiveMotion(int x, int y);
    virtual void MouseClick(int button, int state, int x, int y);

    Vertex2i m_lastMouse;
    int m_button;
    int m_state;
    int m_mod;
    long m_clickStart;
    long m_clickTime;
    bool m_drag;
};

}

#endif
