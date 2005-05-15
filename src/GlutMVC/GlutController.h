/***************************************************************************
* Vistools
* Matt Rasmussen
* GlutController.h
*
***************************************************************************/

#ifndef GLUT_CONTROLLER_H
#define GLUT_CONTROLLER_H

#include "Controller.h"
#include "GlutView.h"
#include "types.h"

namespace Vistools
{

class GlutController : public Controller {
public:
   GlutController(int window);
   virtual ~GlutController();

   void AddKeyBinding(char key, CommandId cmd);
   
   // glut callbacks
   static void GlutKey(unsigned char key, int x, int y);
   static void GlutSpecialKey(int key, int x, int y);
   static void GlutMotion(int x, int y);
   static void GlutMouseClick(int button, int state, int x, int y);

protected:
   // callbacks
   virtual void Key(unsigned char key, int x, int y);
   virtual void SpecialKey(int key, int x, int y);
   virtual void Motion(int x, int y);
   virtual void MouseClick(int button, int state, int x, int y);

   Vertex2i m_lastMouse;
   int m_button;
   int m_state;
   int m_mod;
};

}

#endif
