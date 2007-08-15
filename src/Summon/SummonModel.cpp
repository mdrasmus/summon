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


void SummonModel::ExecCommand(Command &command)
{
    switch (command.GetId()) {
        case ADD_GROUP_COMMAND: {
            //ModelChangeCommand change;        
            BuildEnv env(true, m_kind);
            int id = AddGroup(env, -1, ((AddGroupCommand*)(&command))->code);
            ((AddGroupCommand*)(&command))->SetReturn(Int2Scm(id));
            
            //change.changedGroups.push_back(m_table.GetGroup(id));
            ModelChanged();
            Update();
            //Redisplay();
            } break;
        
        case INSERT_GROUP_COMMAND: {
            //ModelChangeCommand change;
            BuildEnv env(true, m_kind);
            int pid = ((InsertGroupCommand*)(&command))->groupid;
            
            // find parent group to insert into
            Group *group = Id2Group(pid); //m_table.GetGroup(pid);
            if (group != NULL)
            {
                // get the environment and parent of old group
                env = GetEnv(env, NULL, group);
                
                int id = AddGroup(env, pid, 
                                  ((InsertGroupCommand*)(&command))->code);
                
                ((InsertGroupCommand*)(&command))->SetReturn(Int2Scm(id));
                //change.changedGroups.push_back(group);
                
                ModelChanged();
                Update();
                //Redisplay();                
            } else {
                Error("unknown group %d", pid);
                ((InsertGroupCommand*)(&command))->SetReturn(Int2Scm(-1));
            }
            

            } break;
        
        case REMOVE_GROUP_COMMAND: {
            RemoveGroupCommand *remove = (RemoveGroupCommand*) &command;
            
            // loop through group ids
            for (unsigned int i=0; i<remove->groupids.size(); i++) {
                RemoveGroup(remove->groupids[i]);
            }
            
            ModelChanged();
            Update();
            //Redisplay();
            } break;
        
        case REPLACE_GROUP_COMMAND: {
            /*
            //ModelChangeCommand change;
            ReplaceGroupCommand *replace = (ReplaceGroupCommand*) &command;
            BuildEnv env(true, m_kind);
            
            // find group to replace
            Group *group = m_table.GetGroup(replace->groupid);
            if (group != NULL && 
                group != m_table.GetRootGroup())
            {
                // get the environment and parent of old group
                env = GetEnv(env, NULL, group);
                Element *parent = group->GetParent();
                
                // remove old group
                RemoveGroup(replace->groupid);
                
                // add new group
                if (PopulateElement(env, parent, replace->code)) {
                    replace->SetReturn(ScmCadr(ScmCar(replace->code)));
                    //change.changedGroups.push_back();
                } else {
                    replace->SetReturn(Int2Scm(-1));
                }
                ModelChanged();
                //Redisplay();                
            } else {
                Error("unknown group %d", replace->groupid);
                replace->SetReturn(Int2Scm(-1));
            }
            */
            } break;
        
        case CLEAR_GROUPS_COMMAND: {
            // remove root group, GroupTable will create new root
            RemoveHotspots(GetRoot());
            RemoveGroup(GetRoot()->GetGroupId());
            //m_table.Clear();
            ModelChanged();
            Update();
            //Redisplay();
            } break;
        
        case SHOW_GROUP_COMMAND: {
            //ModelChangeCommand change;
            ShowGroupCommand *show = (ShowGroupCommand*) &command;
            
            // toggle a group's visibility
            Group *group = Id2Group(show->groupid); //m_table.GetGroup(show->groupid);
            if (group) {
                group->SetVisible(show->visible);
                //change.changedGroups.push_back(group);
            }
            
            ModelChanged();
            Update();
            //Redisplay();
            } break;
        
        case GET_GROUP_COMMAND: {
            GetGroupCommand *getGroup = (GetGroupCommand*) &command;
            
            Group *group = Id2Group(getGroup->groupid); //m_table.GetGroup(getGroup->groupid);
            if (group) {
                BuildEnv env(true, m_kind);
                getGroup->SetReturn(GetGroup(env, group));
            } else {
                Error("unknown group %d", getGroup->groupid);
            }
            } break;

        case GET_ROOT_ID_COMMAND:
            ((GetRootIdCommand*) &command)->SetReturn(
                Int2Scm(GetRoot()->GetGroupId()));
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
        start = GetRoot(); //m_table.GetRootGroup();
    }
    
    // find path from end to start through parent pointers
    for (Element *i = end; i != start; i = i->GetParent()) {
        elms.push_front(i);
    }
    elms.push_front(end);

    
    // traceback down parent pointers to calc env
    for (list<Element*>::iterator i=elms.begin(); i!=elms.end(); i++) {
        switch ((*i)->GetId()) {
            case TRANSFORM_CONSTRUCT: {
                // modify environment for children elements
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


int SummonModel::AddGroup(BuildEnv &env, int parent, Scm code)
{
    Element *elm = GetElementFromObject(code.GetPy());
    
    // verify that this is a group
    if (!elm || elm->GetId() != GROUP_CONSTRUCT) {
        Error("Can only add groups");
        return -1;
    }

    // set parent if default (root) is given
    if (parent == -1) {
        parent = GetRoot()->GetGroupId(); //m_table.GetRoot();
    }
    
    // get parent group object
    Group *pgroup = Id2Group(parent); //m_table.GetGroup(parent);
    if (!pgroup) {
        Error("Unknown parent group %d", pgroup);
        return -1;
    }
    
    
    Group *group = (Group*) elm;
    //m_table.AddGroup(group);
    if (group) {
        pgroup->AddChild(group);
        return group->GetGroupId();
    } else {
        return -1;
    }
}



void SummonModel::Update()
{
    Element *element = GetRoot();
    BuildEnv env(true, m_kind);
    Update(element, &env);
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
        printf("HOTSPOT register %f %f %f %f | %f %f %f %f\n", 
                    hotspot->pos1.x, hotspot->pos1.y, 
                    hotspot->pos2.x, hotspot->pos2.y,
                    pos1.x, pos1.y, pos2.x, pos2.y);
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


Scm SummonModel::GetGroup(BuildEnv &env, Element *elm)
{
    Scm children = Scm_EOL;
    

    if (elm->GetId() == GROUP_CONSTRUCT ||
        elm->GetId() == TRANSFORM_CONSTRUCT ||
        elm->GetId() == DYNAMIC_GROUP_CONSTRUCT)
    {
        // build children, iterate backwards
        Element::Iterator iter = elm->End();
        iter--;
        for (; iter != elm->End(); iter--) {
            if (IsTransform((*iter)->GetId())) {
                Transform *trans = (Transform*) (*iter);
                BuildEnv env2 = env;
                MultMatrix(env.trans.mat, trans->GetMatrix(), env2.trans.mat);
                Scm elms = GetGroup(env2, *iter);
                
                // transform returns multiple objects
                children = ScmAppend(elms, children);
            } else {
                if ((*iter)->IsVisible()) {
                    children = ScmCons(GetGroup(env, *iter), children);
                }
            }
        }
        
        if (elm->GetId() == GROUP_CONSTRUCT) {
            return ScmCons(Int2Scm(GROUP_CONSTRUCT), 
                           ScmCons(Int2Scm(((Group*)elm)->GetGroupId()),
                                   children));                  
        } else {
            // transform returns multiple objects
            return children;
        }
    
    
    } else if (IsGraphic(elm->GetId())) {
        Graphic *graphic = (Graphic*) elm;
        Scm children = Scm_EOL;
        
        // build primitives
        for (int ptr = 0; graphic->More(ptr); ptr = graphic->NextPrimitive(ptr))
        {
            Scm child = Scm_EOL;

            if (graphic->IsVertices(ptr)) {
                float *data = graphic->GetVertex(graphic->VerticesStart(ptr));

                for (int i = 2 * graphic->GetVerticesLen(ptr) - 1; i > 0; i-=2)
                {
                    float x, y;
                    env.trans.VecMult(data[i-1], data[i], &x, &y);
                    child = ScmCons(Float2Scm(x), 
                                    ScmCons(Float2Scm(y), child));
                }
                child = ScmCons(Int2Scm(VERTICES_CONSTRUCT), child);

            } else if (graphic->IsColor(ptr)) {
                char *color = graphic->GetColor(ptr);
                for (int i = 3; i >= 0; i--) {
                    child = ScmCons(Float2Scm(((unsigned char) color[i]) / 255.0), child);
                }
                child = ScmCons(Int2Scm(COLOR_CONSTRUCT), child);

            } else {
                // unknown primitive
                Error("unknown primitive");
                assert(0);
            }

            children = ScmAppend(children, ScmCons(child, Scm_EOL));
        }
    
        return ScmCons(Int2Scm(elm->GetId()), children);
    } else if (elm->GetId() == TEXT_CONSTRUCT) {
        TextElement *text = (TextElement*) elm;
        
        Scm justified = Scm_EOL;
        
        if (text->justified & TextElement::LEFT)
            justified = ScmCons(String2Scm("left"), justified);
        if (text->justified & TextElement::CENTER)
            justified = ScmCons(String2Scm("center"), justified);
        if (text->justified & TextElement::RIGHT)
            justified = ScmCons(String2Scm("right"), justified);
        if (text->justified & TextElement::TOP)
            justified = ScmCons(String2Scm("top"), justified);
        if (text->justified & TextElement::MIDDLE)
            justified = ScmCons(String2Scm("middle"), justified);
        if (text->justified & TextElement::BOTTOM)
            justified = ScmCons(String2Scm("bottom"), justified);
        if (text->justified & TextElement::VERTICAL)
            justified = ScmCons(String2Scm("vertical"), justified);

        float x1, y1, x2, y2;
        env.trans.VecMult(text->pos1.x, text->pos1.y, &x1, &y1);
        env.trans.VecMult(text->pos2.x, text->pos2.y, &x2, &y2);
        
        int id = 0;
        
        switch (text->kind) {
            case TextElement::KIND_BITMAP:
                id = TEXT_CONSTRUCT;
                break;
            case TextElement::KIND_SCALE:
                id = TEXT_SCALE_CONSTRUCT;
                break;
            case TextElement::KIND_CLIP:
                id = TEXT_CLIP_CONSTRUCT;
                
                // add extra height fields                
                justified = 
                    ScmCons(Float2Scm(text->minHeight),
                        ScmCons(Float2Scm(text->maxHeight), justified));
                break;
            default:
                assert(0);
        }
        
        return ScmCons(Int2Scm(id),
                ScmCons(String2Scm(text->text.c_str()),
                 ScmCons(Float2Scm(x1),
                  ScmCons(Float2Scm(y1),
                   ScmCons(Float2Scm(x2),
                    ScmCons(Float2Scm(y2), justified))))));
        
        
    } else if (elm->GetId() == HOTSPOT_CONSTRUCT) {
        Hotspot *hotspot = (Hotspot*) elm;
        Scm kind;
        
        if (hotspot->kind == Hotspot::CLICK) {
            kind = String2Scm("click");
        } else {
            assert(0);
        }
        
        return ScmCons(Int2Scm(elm->GetId()),
                ScmCons(kind,
                 ScmCons(Float2Scm(hotspot->pos1.x),
                  ScmCons(Float2Scm(hotspot->pos1.y),
                   ScmCons(Float2Scm(hotspot->pos2.x),
                    ScmCons(Float2Scm(hotspot->pos2.y),
                     ScmCons(hotspot->GetProc()->GetScmProc(), Scm_EOL)))))));
        
    } else {
        assert(0);
    }
    
    return Scm_EOL;
}


void SummonModel::RemoveGroup(int id)
{
    Group *group = Id2Group(id); //m_table.GetGroup(id);
    if (group) {
        RemoveHotspots(group);
        //m_table.RemoveGroup(id);
        
        Element *parent = group->GetParent();
        
        // notify parent
        if (parent) {
            parent->RemoveChild(group);
        }
        
        delete group;
        
        // make sure model always has a root
        if (group == m_root) {
            m_root = new Group();
        }        
    }
}


        




void SummonModel::RemoveHotspots(Element *elm)
{
    if (elm->GetId() == HOTSPOT_CONSTRUCT) {
        m_hotspotClicks.remove((Hotspot*) elm);
    } else {
        // recurse
        for (Element::Iterator i=elm->Begin(); i!=elm->End(); i++) {
            RemoveHotspots(*i);
        }
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

}

