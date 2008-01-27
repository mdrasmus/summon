/***************************************************************************
* Summon
* Matt Rasmussen
* Model.h
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

#ifndef SUMMON_MODEL_H
#define SUMMON_MODEL_H

#include <list>
#include "Command.h"
#include "View.h"


namespace Summon
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
