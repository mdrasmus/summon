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




struct BuildEnv
{
    BuildEnv(bool identity = false, int kind=MODEL_WORLD) :
        modelKind(kind)
    {
        if (identity) {
            trans.SetIdentity();
        }
    } 
    
    
    TransformMatrix trans;
    int modelKind;
};


class SummonModel : public Model
{
public:
    SummonModel(int id, int kind=MODEL_WORLD);

    inline int GetId() { return m_id; }
    void SetKind(int kind) { m_kind = kind; }    
    
    // model manipulation
    virtual void ExecCommand(Command &command);
    Element *AddElement(Element *parent, Scm code);
    void RemoveElement(Element *elm);

    // model queries
    list<Command*> HotspotClick(Vertex2f pos);    
    void FindBounding(Element *elm, Vertex2f *pos1, Vertex2f *pos2);
    inline Element *GetRoot()
    { return m_root; }
    
    
protected:
    BuildEnv GetEnv(BuildEnv &env, Element *start, Element *end);
    
    // book keeping
    void Update();
    void Update(Element *element);
    void Update(Element *element, BuildEnv *env);
    void UpdateHotspot(Hotspot *hotspot, BuildEnv *env);
    void UpdateTextElement(TextElement *textElm, BuildEnv *env);
    void RemoveHotspots(Element *elm);

    inline void ModelChanged()
    {
        ModelChangedCommand cmd;
        UpdateViews(cmd);
    }


    int m_id;
    int m_kind;
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
