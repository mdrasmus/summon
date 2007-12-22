/***************************************************************************
* Summon
* Matt Rasmussen
* SummonModel.h
*
***************************************************************************/

#ifndef DRAW_MODEL_H
#define DRAW_MODEL_H

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
    list<Command*> HotspotClick(Vertex2f pos, const Camera camera);    
    void FindBounding(Element *elm, Vertex2f *pos1, Vertex2f *pos2);
    inline Element *GetRoot()
    { return m_root; }
    
protected:    
    // book keeping
    void UpdateRecurse(Element *element);
    void UpdateHotspot(Hotspot *hotspot);
    void RemoveHotspots(Element *elm);

    inline void ModelChanged()
    {
        ModelChangedCommand cmd;
        UpdateViews(cmd);
    }


    int m_id;
    ModelKind m_kind;
    Element *m_root;
    list<Hotspot*> m_hotspotClicks;
    HashTable<Hotspot*, bool, HashPointer> m_hotspotClickSet;
};

inline SummonModel *GetModelOfElement(Element *elm)
{
    return (SummonModel*) elm->GetModel();
}

}

#endif
