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



SummonModel::SummonModel(int id, int kind) :
    m_id(id),
    m_kind(kind),
    m_hotspotClickSet(1000)
{
    m_root = new Group();
    m_root->SetModel(this);
    m_root->IncRef();
}    


void SummonModel::ExecCommand(Command &command)
{
    switch (command.GetId()) {
        case ADD_GROUP_COMMAND: {
            if (!AddElement(NULL, ((AddGroupCommand*)(&command))->code))
                ((ScriptCommand*)(&command))->SetReturn(Scm_NULL);
            } break;
        
        case INSERT_GROUP_COMMAND: {
            int pid = ((InsertGroupCommand*)(&command))->groupid;
            
            // find parent group to insert into
            if (!AddElement(Id2Element(pid), 
                            ((InsertGroupCommand*)(&command))->code))
                ((ScriptCommand*)(&command))->SetReturn(Scm_NULL);
            
            } break;
        
        case REMOVE_GROUP_COMMAND: {
            RemoveGroupCommand *remove = (RemoveGroupCommand*) &command;
            
            // loop through group ids
            for (unsigned int i=0; i<remove->groupids.size(); i++) {
                if (!RemoveElement(Id2Element(remove->groupids[i]))) {
                    remove->SetReturn(Scm_NULL);
                }
            }
            
            } break;
        
        case REPLACE_GROUP_COMMAND: {
            ReplaceGroupCommand *replace = (ReplaceGroupCommand*) &command;
            
            // find group to replace
            Element *elm = Id2Element(replace->groupid);
            Element *elm2 = ReplaceElement(elm, ScmCar(replace->code));
            if (!elm2)
                replace->SetReturn(Scm_NULL);
            } break;
        
        case CLEAR_GROUPS_COMMAND: {
            // remove root group, new root will be automatically created
            RemoveElement(GetRoot());
            } break;
        
        case SHOW_GROUP_COMMAND: {
            ShowGroupCommand *show = (ShowGroupCommand*) &command;
            
            // toggle a group's visibility
            Element *elm = Id2Element(show->groupid);
            elm->SetVisible(show->visible); 
            Update(elm);
            } break;

        case GET_ROOT_ID_COMMAND:
            ((GetRootIdCommand*) &command)->SetReturn(
                Int2Scm(Element2Id(GetRoot())));
            break;
        
        case GET_BOUNDING_COMMAND: {
            ShowGroupCommand *show = (ShowGroupCommand*) &command;
            
            // get the bounding box of an element
            Element *elm = Id2Element(show->groupid);
            Vertex2f pos1, pos2;
            FindBounding(elm, &pos1, &pos2);
            
            show->SetReturn(ScmCons(Float2Scm(pos1.x),
                              ScmCons(Float2Scm(pos1.y),
                                ScmCons(Float2Scm(pos2.x),
                                  ScmCons(Float2Scm(pos2.y), Scm_EOL)))));
        
            } break;
        default:
            // unknown command given
            assert(0);
    }
}

// This function is called when a HotspotClick command is executed
// Checks to see if any hotspots have been activated
list<Command*> SummonModel::HotspotClick(Vertex2f pos, Vertex2f zoom)
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


Element *SummonModel::AddElement(Element *parent, Scm code)
{
    Element *elm = GetElementFromObject(code.GetPy());
    
    // verify that this is a group
    if (!elm) {
        Error("cannot add graphical element, it is invalid.");
        return NULL;
    }

    if (elm->GetParent() != NULL) {
        Error("element already has parent.");
        return NULL;
    }

    // set parent if default (root) is given
    if (parent == NULL)
        parent = GetRoot();
    
    parent->AddChild(elm);
    Update(elm);
    return elm;
}


bool SummonModel::ReplaceElement(Element *oldelm, Element *newelm)
{
    // TODO: fix
    Element *parent = oldelm->GetParent();
    
    if (parent == NULL) {
        Error("cannot replace root element");
        return false;
    }
    
    // remove old element
    if (!RemoveElement(oldelm))
        return false;
    parent->AddChild(newelm);
    Update(newelm);
    
    return true;
}


Element *SummonModel::ReplaceElement(Element *oldelm, Scm code)
{
    // TODO: fix
    Element *parent = oldelm->GetParent();
    
    if (parent == NULL) {
        Error("cannot replace root element");
        return NULL;
    }
    
    // remove old element
    if (!RemoveElement(oldelm))
        return NULL;
    Element *elm = AddElement(parent, code);
    
    return elm;
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
    
    element->SetModel(this);
    
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



bool SummonModel::RemoveElement(Element *elm)
{
    if (elm->GetModel() != this) {
        Error("element is not in model");
        return false;
    }

    Element *parent = elm->GetParent();
    
    // notify parent
    if (parent)
        parent->RemoveChild(elm);

    // remove any hotspots underneath this group
    RemoveHotspots(elm);
    elm->SetModel(NULL);

    // make sure model always has a root
    if (elm == m_root) {
        elm->DecRef();
        m_root = new Group();
        m_root->SetModel(this);
        m_root->IncRef();
        Update();
    } else {
        assert(parent);
        Update(parent);
    }
    
    // delete only if it is not referenced by python
    if (!elm->IsReferenced())
        delete elm;
    
    return true;
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


void SummonModel::FindBounding(Element *elm, Vertex2f *pos1, Vertex2f *pos2)
{

    float FLOAT_MIN = -1e307;
    float FLOAT_MAX = 1e307;

    // find smallest bouding box
    float top    = FLOAT_MIN, 
          bottom = FLOAT_MAX, 
          left   = FLOAT_MAX, 
          right  = FLOAT_MIN;

    TransformMatrix matrix;
    
    if (elm) {
        BuildEnv env(true, m_kind);    
        BuildEnv env2 = GetEnv(env, NULL, elm);
        CopyMatrix(matrix.mat, env.trans.mat);
    } else {
        elm = GetRoot();
        MakeIdentityMatrix(matrix.mat);
    }
        
    
    elm->FindBounding(&top, &bottom, &left, &right, &matrix);
    
    pos1->x = left;
    pos1->y = bottom;
    pos2->x = right;
    pos2->y = top;
}


} // namespace Summon

