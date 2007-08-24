/***************************************************************************
* Summon
* Matt Rasmussen
* SummonModel.cpp
*
***************************************************************************/

#include "first.h"
#include <list>

#include "SummonModel.h"




namespace Summon
{

Group *Id2Group(int id)
{
    return (Group*) id;
}

int Group2Id(Group *group)
{
    return (int) group;
}


SummonModel::SummonModel(int id, int kind) :
    m_id(id),
    m_kind(kind),
    m_hotspotClickSet(1000)
{
    m_root = new Group();
    m_root->IncRef();
}    


void SummonModel::ExecCommand(Command &command)
{
    switch (command.GetId()) {
        case ADD_GROUP_COMMAND: {
            int id = Group2Id(AddGroup(NULL, 
                                       ((AddGroupCommand*)(&command))->code));
            ((AddGroupCommand*)(&command))->SetReturn(Int2Scm(id));
            Update(Id2Group(id));
            } break;
        
        case INSERT_GROUP_COMMAND: {
            int pid = ((InsertGroupCommand*)(&command))->groupid;
            
            // find parent group to insert into
            Group *group = Id2Group(pid);
            if (group != NULL)
            {
                // get the environment and parent of old group
                int id = Group2Id(AddGroup(Id2Group(pid), 
                                  ((InsertGroupCommand*)(&command))->code));
                
                ((InsertGroupCommand*)(&command))->SetReturn(Int2Scm(id));
                Update(Id2Group(id));
            } else {
                Error("unknown group %d", pid);
                ((InsertGroupCommand*)(&command))->SetReturn(Int2Scm(-1));
            }
            

            } break;
        
        case REMOVE_GROUP_COMMAND: {
            RemoveGroupCommand *remove = (RemoveGroupCommand*) &command;
            
            // loop through group ids
            for (unsigned int i=0; i<remove->groupids.size(); i++) {
                RemoveGroup(Id2Group(remove->groupids[i]));
                
                Element *elm = Id2Group(remove->groupids[i])->GetParent();
                if (elm)
                    Update(elm);
                else
                    Update();
            }
            
            } break;
        
        case REPLACE_GROUP_COMMAND: {
            ReplaceGroupCommand *replace = (ReplaceGroupCommand*) &command;
            
            // find group to replace
            Group *group = Id2Group(replace->groupid);
            if (group != NULL && 
                group != GetRoot())
            {
                // get the environment and parent of old group
                Element *parent = group->GetParent();
                
                // remove old group
                RemoveGroup(group);
                
                int id = Group2Id(AddGroup(parent, ScmCar(replace->code)));
                replace->SetReturn(Int2Scm(id));
                Update(Id2Group(id));
            } else {
                Error("unknown group %d", replace->groupid);
                replace->SetReturn(Int2Scm(-1));
            }
            } break;
        
        case CLEAR_GROUPS_COMMAND: {
            // remove root group, new root will be automatically created
            RemoveGroup(GetRoot());
            Update();
            } break;
        
        case SHOW_GROUP_COMMAND: {
            ShowGroupCommand *show = (ShowGroupCommand*) &command;
            
            // toggle a group's visibility
            Group *group = Id2Group(show->groupid);
            group->SetVisible(show->visible);
                
            Update(group);
            } break;
        
        /*
        case GET_GROUP_COMMAND: {
            GetGroupCommand *getGroup = (GetGroupCommand*) &command;
            
            Group *group = Id2Group(getGroup->groupid);
            if (group) {
                BuildEnv env(true, m_kind);
                getGroup->SetReturn(GetGroup(env, group));
            } else {
                Error("unknown group %d", getGroup->groupid);
            }
            } break;
        */

        case GET_ROOT_ID_COMMAND:
            ((GetRootIdCommand*) &command)->SetReturn(
                Int2Scm(Group2Id(GetRoot())));
            break;
        
        default:
            assert(0);
    }
}

// This function is called when a HotspotClick command is executed
// Checks to see if any hotspots have been activated
list<Command*> SummonModel::HotspotClick(Vertex2f pos)
{
    list<Command*> cmds;
    
    for (list<Hotspot*>::iterator i=m_hotspotClicks.begin(); 
         i!=m_hotspotClicks.end(); i++)
    {
        if (pos.x >= (*i)->envpos1.x && pos.x <= (*i)->envpos2.x &&
            pos.y >= (*i)->envpos1.y && pos.y <= (*i)->envpos2.y)
        {
            cmds.push_back((*i)->GetProc()->Create());
        }
    }
    
    return cmds;
}


// Gets the environment of a group.
// env is the starting environment
// start is the starting element of the descent
BuildEnv SummonModel::GetEnv(BuildEnv &env, Element *start, Element *end)
{
    list<Element*> elms;
    BuildEnv env2 = env;

    // the default value for start is the root
    if (start == NULL) {
        start = GetRoot();
    }
    
    // find path from end to start through parent pointers
    for (Element *i = end->GetParent(); i && i != start; i = i->GetParent()) {
        elms.push_front(i);
    }
    elms.push_front(start);

    
    // traceback down parent pointers to calc env
    for (list<Element*>::iterator i=elms.begin(); i!=elms.end(); i++) {
        switch ((*i)->GetId()) {
            case TRANSFORM_CONSTRUCT: {
                // modify environment for child elements
                float tmp[16];
                MultMatrix(env2.trans.mat, ((Transform*) (*i))->GetMatrix(), 
                           tmp);
                CopyMatrix(env2.trans.mat, tmp);
                break;
            }
        }
    }
    
    return env2;
}


Group *SummonModel::AddGroup(Element *parent, Scm code)
{
    Element *elm = GetElementFromObject(code.GetPy());
    
    // verify that this is a group
    if (!elm || elm->GetId() != GROUP_CONSTRUCT) {
        Error("Can only add groups");
        return NULL;
    }

    // set parent if default (root) is given
    if (parent == NULL)
        parent = GetRoot();
    
    Group *group = (Group*) elm;
    if (group) {
        parent->AddChild(group);
        return group;
    } else {
        return NULL;
    }
}



void SummonModel::Update()
{
    Element *element = GetRoot();
    BuildEnv env(true, m_kind);
    Update(element, &env);
    ModelChanged();
}


void SummonModel::Update(Element *element)
{
    BuildEnv env(true, m_kind);
    BuildEnv env2 = GetEnv(env, NULL, element);
    Update(element, &env2);
    ModelChanged();
}


void SummonModel::Update(Element *element, BuildEnv *env)
{
    BuildEnv *env2 = NULL;

    // perform element-specific updating
    switch (element->GetId()) {
        case HOTSPOT_CONSTRUCT:
            // apply current env transform to coordinates
            UpdateHotspot((Hotspot*) element, env);      
            break;
        
        case TEXT_CONSTRUCT:
            UpdateTextElement((TextElement*) element, env);
            break;
        
        case TRANSFORM_CONSTRUCT: {
            Transform *trans = (Transform*) element;
            env2 = new BuildEnv(false, env->modelKind);
            MultMatrix(env->trans.mat, trans->GetMatrix(), env2->trans.mat);
            env = env2;
            } break;
    }
    

    // recurse through children
    for (Element::Iterator i=element->Begin(); i!=element->End(); i++) {
        Update(*i, env);
    }
    
    // free the build environment if a new was created
    if (env2)
        delete env2;
}


void SummonModel::UpdateHotspot(Hotspot *hotspot, BuildEnv *env)
{
    Vertex2f pos1, pos2;
    env->trans.VecMult(hotspot->pos1.x, hotspot->pos1.y, &pos1.x, &pos1.y);
    env->trans.VecMult(hotspot->pos2.x, hotspot->pos2.y, &pos2.x, &pos2.y);
    
    if (pos1.x > pos2.x)
        swap(pos1.x, pos2.x);
    if (pos1.y > pos2.y)
        swap(pos1.y, pos2.y);
    
    hotspot->envpos1 = pos1;
    hotspot->envpos2 = pos2;
    
    // register hotspot
    if (!m_hotspotClickSet.HasKey(hotspot)) {
        m_hotspotClickSet.Insert(hotspot, true);
        m_hotspotClicks.push_back(hotspot);
    }
}


void SummonModel::UpdateTextElement(TextElement *textElm, BuildEnv *env)
{
    textElm->modelKind = env->modelKind;
    
    // find scaling
    Vertex2f orgin;
    env->trans.VecMult(0.0, 0.0, &orgin.x, &orgin.y);
    env->trans.VecMult(1.0, 1.0, &textElm->scale.x, &textElm->scale.y);
    textElm->scale = textElm->scale - orgin;
    

    // find environment position
    env->trans.VecMult(textElm->pos1.x, textElm->pos1.y, 
                       &textElm->envpos1.x, &textElm->envpos1.y);
    env->trans.VecMult(textElm->pos2.x, textElm->pos2.y, 
                       &textElm->envpos2.x, &textElm->envpos2.y);
    if (textElm->envpos1.x > textElm->envpos2.x)
        swap(textElm->envpos1.x, textElm->envpos2.x);
    if (textElm->envpos1.y > textElm->envpos2.y)
        swap(textElm->envpos1.y, textElm->envpos2.y);
}



void SummonModel::RemoveGroup(Group *group)
{
    Element *parent = group->GetParent();
    
    // notify parent
    if (parent)
        parent->RemoveChild(group);

    // remove any hotspots underneath this group
    RemoveHotspots(group);

    // make sure model always has a root
    if (group == m_root) {
        group->DecRef();
        m_root = new Group();
        m_root->IncRef();
    }
    
    // delete only if it is not referenced by python
    if (!group->IsReferenced())
        delete group;
}


        
void SummonModel::RemoveHotspots(Element *elm)
{
    if (elm->GetId() == HOTSPOT_CONSTRUCT) {
        m_hotspotClicks.remove((Hotspot*) elm);
        m_hotspotClickSet.Remove((Hotspot*) elm);
    } else {
        // recurse to child elements
        for (Element::Iterator i=elm->Begin(); i!=elm->End(); i++)
            RemoveHotspots(*i);
    }
}


void SummonModel::FindBounding(Vertex2f *pos1, Vertex2f *pos2)
{

    float FLOAT_MIN = -1e307;
    float FLOAT_MAX = 1e307;

    // find smallest bouding box
    float top    = FLOAT_MIN, 
          bottom = FLOAT_MAX, 
          left   = FLOAT_MAX, 
          right  = FLOAT_MIN;

    TransformMatrix matrix;
    MakeIdentityMatrix(matrix.mat);
    
    Group *group = GetRoot();
    group->FindBounding(&top, &bottom, &left, &right, &matrix);
    
    pos1->x = left;
    pos1->y = bottom;
    pos2->x = right;
    pos2->y = top;
}


} // namespace Summon

