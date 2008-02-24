/***************************************************************************
* Summon
* Matt Rasmussen
* SummonModel.h
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

#ifndef SUMMON_SUMMON_MODEL_H
#define SUMMON_SUMMON_MODEL_H

#include "Script.h"
#include "Model.h"
#include "summonCommands.h"
#include "Graphic.h"
#include "HashTable.h"
#include "Hotspot.h"
#include "Transform.h"
#include "TextElement.h"
#include "Group.h"


namespace Summon
{




class SummonModel : public Model
{
public:
    SummonModel(int id, ModelKind kind=MODEL_WORLD);

    inline int GetId() { return m_id; }
    void SetKind(ModelKind kind) { m_kind = kind; }    
    
    // model manipulation
    virtual void ExecCommand(Command &command);
    Element *AddElement(Element *parent, Scm code);
    bool RemoveElement(Element *elm);
    bool ReplaceElement(Element *oldelm, Element *newelm);
    Element *ReplaceElement(Element *oldelm, Scm code);
    
    // book keeping
    void Update(Element *element=NULL);

    // model queries
    list<Command*> HotspotClick(Vertex2f pos, const Camera camera, int kind);
    void FindBounding(Element *elm, Vertex2f *pos1, Vertex2f *pos2, 
                      const Camera &camera=defaultCamera);
    inline Element *GetRoot()
    { return m_root; }
    
protected:    
    // book keeping
    void UpdateRecurse(Element *element);
    void UpdateHotspot(Hotspot *hotspot);
    void RemoveHotspots(Element *elm);
    void CleanupElement(Element *elm);

    inline void ModelChanged()
    {
        ModelChangedCommand cmd;
        UpdateViews(cmd);
    }

    void SetRoot(Element *newroot);

    int m_id;
    ModelKind m_kind;
    Element *m_root;
    list<Hotspot*> m_hotspotClicks;
    typedef list<Hotspot*>::iterator HotspotListIter;
    HashTable<Hotspot*, HotspotListIter, HashPointer> m_hotspotClickSet;
};

inline SummonModel *GetModelOfElement(Element *elm)
{
    return (SummonModel*) elm->GetModel();
}

}

#endif
