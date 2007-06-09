/***************************************************************************
* Summon
* Matt Rasmussen
* DrawModel.h
*
***************************************************************************/

#ifndef DRAW_MODEL_H
#define DRAW_MODEL_H

#include "Script.h"
#include "Model.h"
#include "drawCommands.h"
#include "Graphic.h"
#include "GroupTable.h"
#include "Hotspot.h"
#include "Transform.h"

namespace Summon
{




struct BuildEnv
{
    BuildEnv(bool identity = false, int kind=MODEL_WORLD) :
        modelKind(kind) {
        if (identity) {
            trans.SetIdentity();
        }
    }
    
    TransformMatrix trans;
    int modelKind;
};


class DrawModel : public Model
{
public:
    DrawModel(int id, int kind=MODEL_WORLD) :
        m_id(id),
        m_kind(kind)
    {
    }
    
    virtual void ExecCommand(Command &command);

    // model queries
    list<Command*> HotspotClick(Vertex2f pos);
    
    BuildEnv GetEnv(BuildEnv &env, Element *start, Element *end);
    
    // model construction
    int AddGroup(BuildEnv &env, int parent, Scm code);
    Group *BuildGroup(BuildEnv &env, Scm code);
    Element *BuildElement(BuildEnv &env, Scm code);
    bool PopulateElement(BuildEnv &env, Element *elm, Scm code);
    //bool PopulateGraphic(BuildEnv &env, Graphic *graphic, Scm code);
    //Primitive *BuildPrimitive(BuildEnv &env, Scm code);
    Element *BuildHotspot(BuildEnv &env, Scm code);
    Element *BuildText(BuildEnv &env, Scm code, int kind);
    
    void RemoveGroup(int id);
    Scm GetGroup(BuildEnv &env, Element *elm);
    
    void RemoveHotspots(Element *elm);
    
    void SetKind(int kind) { m_kind = kind; }
    
    void FindBounding(Vertex2f *pos1, Vertex2f *pos2);
    
    inline GroupTable *GetGroupTable()
    { return &m_table; }
    
    inline int GetId() { return m_id; }
    
    inline void Redisplay()
    {
        RedisplayCommand redisplay;
        UpdateViews(redisplay);
    }

    
protected:
    int m_id;
    int m_kind;
    GroupTable m_table;
    list<Hotspot*> m_hotspotClicks;
};

}

#endif
