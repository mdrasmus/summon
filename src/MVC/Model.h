/***************************************************************************
* Vistools
* Matt Rasmussen
* Model.h
*
***************************************************************************/

#ifndef MODEL_H
#define MODEL_H

#include <list>
#include "Command.h"
#include "View.h"


namespace Vistools
{

class Model {
public:
   Model();
   virtual ~Model();
   
   void AddView(View *view);
   void RemoveView(View *view);
   void UpdateViews(Command &command);

   typedef std::list<View*> ViewList;
   typedef ViewList::iterator ViewIter;
private:
   ViewList viewers;
};

}

#endif
