/***************************************************************************
* Summon
* Matt Rasmussen
* GlutController.h
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
   int m_button;  // TODO: not used?
   int m_state;   // TODO: not used?
   int m_mod;     // TODO: not used?
   bool m_drag;
};

}

#endif
