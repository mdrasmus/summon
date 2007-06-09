/***************************************************************************
* Summon
* Matt Rasmussen
* Model.cpp
*
***************************************************************************/

#include "first.h"
#include "Model.h"

namespace Summon
{


Model::Model()
{}
   
Model::~Model()
{}

   
void Model::AddView(View *view)
{
   viewers.push_back(view);
}

void Model::RemoveView(View *view)
{
   viewers.remove(view);
}

void Model::UpdateViews(Command &command)
{
   for (ViewIter iter = viewers.begin(); iter != viewers.end(); iter++) {
      (*iter)->ExecCommand(command);
   }
}


}
