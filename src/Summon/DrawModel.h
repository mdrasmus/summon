/***************************************************************************
* Vistools
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
#include "RectTree.h"
#include "Transform.h"

namespace Vistools
{


struct BuildEnv
{
    BuildEnv(bool identity = false) {
        if (identity) {
            trans.SetIdentity();
        }
    }
    
    TransformMatrix trans;
};


class DrawModel : public Model
{
public:
    DrawModel(int id) :
        m_id(id)
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
 
    void FindBounding(Element *elm, 
                      float *top, float *bottom, float *left, float *right,
                      TransformMatrix *matrix);
    
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
    GroupTable m_table;
    RectTree<Command*> m_hotspotClicks;
};

}

#endif
