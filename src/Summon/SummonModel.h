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
    
    virtual void ExecCommand(Command &command);

    // model queries
    list<Command*> HotspotClick(Vertex2f pos);
    
    BuildEnv GetEnv(BuildEnv &env, Element *start, Element *end);
    
    // book keeping
    void Update();
    void Update(Element *element);
    void Update(Element *element, BuildEnv *env);
    void UpdateHotspot(Hotspot *hotspot, BuildEnv *env);
    void UpdateTextElement(TextElement *textElm, BuildEnv *env);
    void RemoveHotspots(Element *elm);
    
    // model manipulation
    Group *AddGroup(BuildEnv &env, Element *parent, Scm code);
    void RemoveGroup(Group *group);
    
    Scm GetGroup(BuildEnv &env, Element *elm);
    
    void FindBounding(Vertex2f *pos1, Vertex2f *pos2);
    
    inline Group *GetRoot()
    { return m_root; }
    
    inline int GetId() { return m_id; }
    
    void SetKind(int kind) { m_kind = kind; }
    
    inline void Redisplay()
    {
        RedisplayCommand redisplay;
        UpdateViews(redisplay);
    }
    
    inline void ModelChanged()
    {
        ModelChangedCommand cmd;
        UpdateViews(cmd);
    }

    
protected:
    int m_id;
    int m_kind;
    Group *m_root;
    list<Hotspot*> m_hotspotClicks;
    HashTable<Hotspot*, bool, HashPointer> m_hotspotClickSet;
};

}

#endif
