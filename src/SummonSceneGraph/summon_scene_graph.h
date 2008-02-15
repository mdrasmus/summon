/***************************************************************************
* SUMMON
* Matt Rasmussen
* summon_scene_graph.h
*
* This file creates main python interface to the SUMMON module.  The SUMMON 
* module itself, it represented by a singleton class SummonModule.
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

#ifndef SUMMON_SUMMON_SCENE_GRAPH_H
#define SUMMON_SUMMON_SCENE_GRAPH_H

#include "first.h"


//=============================================================================
// python visible prototypes
extern "C" {

// functions directly visible in python
PyObject *MakeElement(PyObject *self, PyObject *args);
PyObject *IncRefElement(PyObject *self, PyObject *args);
PyObject *DeleteElement(PyObject *self, PyObject *args);
PyObject *GetElementChildren(PyObject *self, PyObject *args);
PyObject *GetElementContents(PyObject *self, PyObject *args);
PyObject *GetElementParent(PyObject *self, PyObject *args);


}

#endif
