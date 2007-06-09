/***************************************************************************
* Summon
* Matt Rasmussen
* SummonModel.cpp
*
***************************************************************************/

#include "first.h"
#include <list>

#include "SummonModel.h"
#include "TextElement.h"


namespace Summon
{

void SummonModel::ExecCommand(Command &command)
{
    switch (command.GetId()) {
        case ADD_GROUP_COMMAND: {
            //ModelChangeCommand change;        
            BuildEnv env(true, m_kind);
            int id = AddGroup(env, -1, ((AddGroupCommand*)(&command))->code);
            ((AddGroupCommand*)(&command))->SetReturn(Int2Scm(id));
            
            //change.changedGroups.push_back(m_table.GetGroup(id));
            
            Redisplay();
            } break;
        
        case INSERT_GROUP_COMMAND: {
            //ModelChangeCommand change;
            BuildEnv env(true, m_kind);
            int pid = ((InsertGroupCommand*)(&command))->groupid;
            
            // find parent group to insert into
            Group *group = m_table.GetGroup(pid);
            if (group != NULL)
            {
                // get the environment and parent of old group
                env = GetEnv(env, NULL, group);
                
                int id = AddGroup(env, pid, 
                                  ((InsertGroupCommand*)(&command))->code);
                
                ((InsertGroupCommand*)(&command))->SetReturn(Int2Scm(id));
                //change.changedGroups.push_back(group);
                
                Redisplay();                
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
            
            Redisplay();
            } break;
        
        case REPLACE_GROUP_COMMAND: {
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
                Redisplay();                
            } else {
                Error("unknown group %d", replace->groupid);
                replace->SetReturn(Int2Scm(-1));
            }
            
            } break;
        
        case CLEAR_GROUPS_COMMAND: {
            // remove root group, GroupTable will create new root
            RemoveHotspots(m_table.GetRootGroup());
            m_table.Clear();
            Redisplay();
            } break;
        
        case SHOW_GROUP_COMMAND: {
            //ModelChangeCommand change;
            ShowGroupCommand *show = (ShowGroupCommand*) &command;
            
            // toggle a group's visibility
            Group *group = m_table.GetGroup(show->groupid);
            if (group) {
                group->SetVisible(show->visible);
                //change.changedGroups.push_back(group);
            }
        
            Redisplay();
            } break;
        
        case GET_GROUP_COMMAND: {
            GetGroupCommand *getGroup = (GetGroupCommand*) &command;
            
            Group *group = m_table.GetGroup(getGroup->groupid);
            if (group) {
                BuildEnv env(true, m_kind);
                getGroup->SetReturn(GetGroup(env, group));
            } else {
                Error("unknown group %d", getGroup->groupid);
            }
            } break;

        case GET_ROOT_ID_COMMAND:
            ((GetRootIdCommand*) &command)->SetReturn(
                Int2Scm(m_table.GetRoot()));
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
        if (pos.x >= (*i)->pos1.x && pos.x <= (*i)->pos2.x &&
            pos.y >= (*i)->pos1.y && pos.y <= (*i)->pos2.y)
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
        start = m_table.GetRootGroup();
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
    // do nothing if not a list
    if (!ScmConsp(code))
        return -1;
    
    // set parent if default (root) is given
    if (parent == -1) {
        parent = m_table.GetRoot();
    }
    
    // get parent group object
    Group *pgroup = m_table.GetGroup(parent);
    if (!pgroup) {
        Error("Unknown parent group %d", pgroup);
        return -1;
    }
    
    
    // process all groups
    //for (Scm groups = code; ScmConsp(groups); groups = ScmCdr(groups)) {
        // verify that this is a group
        Scm header = ScmCar(code);
        if (!(ScmIntp(header) && Scm2Int(header) == GROUP_CONSTRUCT)) {
            Error("Can only add groups");
            return -1;
        }
        
        // build group and add to parent
        Group *group = BuildGroup(env, ScmCdr(code));
        if (group) {
            pgroup->AddChild(group);
            return group->GetGroupId();
        } else {
            return -1;
        }
    //}
}


Group *SummonModel::BuildGroup(BuildEnv &env, Scm code)
{
    Group *group = NULL;

    // ensure group id is an int
    Scm groupid = ScmCar(code);
    if (!(ScmIntp(groupid))) {
        Error("Group id is not an integer");
        return NULL;
    }

    // create group
    group = new Group(Scm2Int(groupid));
    
    // populate the group with subelements
    if (!PopulateElement(env, group, ScmCdr(code))) {
        Error("Bad group");
        delete group;
        return NULL;
    }
    
    // add to table
    m_table.AddGroup(group);
    
    return group;
}



bool SummonModel::PopulateElement(BuildEnv &env, Element *parent, Scm code)
{
    // process children
    for (Scm children = code; 
         ScmConsp(children); 
         children = ScmCdr(children))
    {
        Scm child = ScmCar(children);
        
        // build child element
        Element *elm = BuildElement(env, child);
        if (!elm) {
            Error("Bad element");
            return false;
        }
        
        // add child element to parent
        parent->AddChild(elm);
    }
    
    return true;
}


Element *SummonModel::BuildElement(BuildEnv &env, Scm code)
{
    Element *elm = NULL;

    // do nothing if not a list
    if (!ScmConsp(code))
        return NULL;
    
    // check that header is int
    if (!ScmIntp(ScmCar(code))) {
        return NULL;
    }
    
    // build element based on header
    int header = Scm2Int(ScmCar(code));
    
    if (IsGraphic(header)) {
        // process element if it is a graphic
        elm = new Graphic(header);
            
        if (!((Graphic*)elm)->Build(ScmCdr(code))) {
            delete elm;
            return NULL;
        }
    } else {
        // element must be something other than a graphic
        switch (header) 
        {
            case GROUP_CONSTRUCT: 
                elm = BuildGroup(env, ScmCdr(code));
                break;
            
            case DYNAMIC_GROUP_CONSTRUCT: 
                elm = new DynamicGroup(ScmCadr(code));
                break;
            
            case HOTSPOT_CONSTRUCT:
                elm = BuildHotspot(env, ScmCdr(code));
                break;
            
            case TEXT_CONSTRUCT: 
                elm = BuildText(env, ScmCdr(code), TextElement::KIND_BITMAP);
                break;
            
            case TEXT_SCALE_CONSTRUCT: 
                elm = BuildText(env, ScmCdr(code), TextElement::KIND_SCALE);
                break;
            
            case TEXT_CLIP_CONSTRUCT: 
                elm = BuildText(env, ScmCdr(code), TextElement::KIND_CLIP);
                break;
            
            case COLOR_CONSTRUCT: {
                // allow a color construct outside of graphics
                Graphic *graphic = new Graphic(POINTS_CONSTRUCT);
                if (graphic->Build(ScmCons(code, Scm_EOL))) {
                    elm = graphic;
                } else {
                    delete graphic;
                    return NULL;
                }

                } break;

            case TRANSLATE_CONSTRUCT:
            case SCALE_CONSTRUCT:
            case FLIP_CONSTRUCT: {
                Scm first  = ScmCadr(code);
                Scm second = ScmCaddr(code);

                if (!ScmFloatp(first) || !ScmFloatp(second)) {
                    delete elm;
                    return NULL;
                }
                
                Transform *trans = new Transform(header, 
                                           Scm2Float(first), Scm2Float(second));
                
                // modify environment for children elements
                BuildEnv env2 = env;
                MultMatrix(env.trans.mat, trans->GetMatrix(), env2.trans.mat);

                elm = trans;
                if (!PopulateElement(env2, elm, ScmCdddr(code))) {
                    delete elm;
                    return NULL;
                }

                } break;

            case ROTATE_CONSTRUCT: {
                Scm first  = ScmCadr(code);

                if (!ScmFloatp(first)) {
                    delete elm;
                    return NULL;
                }
                
                Transform *trans = new Transform(header, Scm2Float(first));

                // modify environment for children elements
                BuildEnv env2 = env;
                MultMatrix(env.trans.mat, trans->GetMatrix(), env2.trans.mat);

                elm = trans;            
                if (!PopulateElement(env2, elm, ScmCddr(code))) {
                    delete elm;
                    return NULL;
                }

                } break;

            default:
                Error("Unknown element '%d'", Scm2Int(ScmCar(code)));
        }
    }
    
    
    return elm;
}



Element *SummonModel::BuildHotspot(BuildEnv &env, Scm code)
{   
    int kindid;
    string kind;
    float x1, y1, x2, y2;
    Scm procCode;
    
    // parse the scm code for a hotspot
    if (!ParseScm("Bad format for Hotspot", code,
                  "sffffp", &kind, &x1, &y1, &x2, &y2, &procCode))
        return NULL;

    // parse hotspot kind
    if (kind == "click") {
        kindid = Hotspot::CLICK;
    } else {
        Error("Unknown hotspot type '%s'", kind.c_str());
        return NULL;
    }
    
    // parse procedure
    CallProcCommand *proc = new CallProcCommand(procCode);
    if (!proc->defined) {
        delete proc;
        return NULL;
    }    
    
    // construct Hotspot
    Hotspot *hotspot = new Hotspot();
    hotspot->SetProc(proc);
    hotspot->kind = kindid;
    
    // apply current env transform to coordinates
    Vertex2f pos1, pos2;
    env.trans.VecMult(x1, y1, &pos1.x, &pos1.y);
    env.trans.VecMult(x2, y2, &pos2.x, &pos2.y);
    
    if (pos1.x > pos2.x)
        swap(pos1.x, pos2.x);
    if (pos1.y > pos2.y)
        swap(pos1.y, pos2.y);
    
    hotspot->pos1 = pos1;
    hotspot->pos2 = pos2;
    
    // register hotspot
    m_hotspotClicks.push_back(hotspot);
    
    return hotspot;
}


Element *SummonModel::BuildText(BuildEnv &env, Scm code, int kind)
{
    string text;
    float x1, y1, x2, y2, minHeight, maxHeight;
    if (!ParseScm("Bad format for text construct", code,
                  "sffff", &text, &x1, &y1, &x2, &y2))
        return NULL;
    code = ScmCddr(ScmCdddr(code));
    
    // clip text takes two more arguments
    if (kind == TextElement::KIND_CLIP) {
        if (!ParseScm("Bad format for text construct", code,
                  "ff", &minHeight, &maxHeight))
            return NULL;
        code = ScmCddr(code);
    }
    
    
    
    // parse justification
    int justified = 0; 
    for (; ScmConsp(code) && ScmStringp(ScmCar(code)); code = ScmCdr(code)) {
        string str = Scm2String(ScmCar(code));
        
        if (str == "left")          justified |= TextElement::LEFT;
        else if (str == "center")   justified |= TextElement::CENTER;
        else if (str == "right")    justified |= TextElement::RIGHT;
        else if (str == "top")      justified |= TextElement::TOP;
        else if (str == "middle")   justified |= TextElement::MIDDLE;
        else if (str == "bottom")   justified |= TextElement::BOTTOM;
        else if (str == "vertical") justified |= TextElement::VERTICAL;
        else {
            Error("unknown justification '%s'", str.c_str());
            return NULL;
        }
    }
    
    // apply default justification
    if (justified == 0) {
        justified = TextElement::LEFT | TextElement::MIDDLE;
    }

    // construct TextElement
    TextElement *textElm = new TextElement();
    textElm->kind = kind;
    textElm->text = text;
    textElm->justified = justified;
    textElm->minHeight = minHeight;
    textElm->maxHeight = maxHeight;
    textElm->modelKind = env.modelKind;
    
    // find scaling
    Vertex2f orgin;
    env.trans.VecMult(0.0, 0.0, &orgin.x, &orgin.y);
    env.trans.VecMult(1.0, 1.0, &textElm->scale.x, &textElm->scale.y);
    textElm->scale = textElm->scale - orgin;
    Vertex2f pos1(x1, y1), pos2(x2, y2);
    textElm->SetRect(pos1, pos2);

    // find environment position
    env.trans.VecMult(x1, y1, &textElm->envpos1.x, &textElm->envpos1.y);
    env.trans.VecMult(x2, y2, &textElm->envpos2.x, &textElm->envpos2.y);
    if (textElm->envpos1.x > textElm->envpos2.x)
        swap(textElm->envpos1.x, textElm->envpos2.x);
    if (textElm->envpos1.y > textElm->envpos2.y)
        swap(textElm->envpos1.y, textElm->envpos2.y);

    
    return textElm;
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
    Group *group = m_table.GetGroup(id);
    if (group) {
        RemoveHotspots(group);    
        m_table.RemoveGroup(id);
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
    
    Group *group = GetGroupTable()->GetRootGroup();
    group->FindBounding(&top, &bottom, &left, &right, &matrix);
    
    pos1->x = left;
    pos1->y = bottom;
    pos2->x = right;
    pos2->y = top;
}

}

