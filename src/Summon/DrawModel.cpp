/***************************************************************************
* Vistools
* Matt Rasmussen
* DrawModel.cpp
*
***************************************************************************/

#include "first.h"
#include <list>

#include "DrawModel.h"
#include "Hotspot.h"
#include "TextElement.h"


namespace Vistools
{

void DrawModel::ExecCommand(Command &command)
{
    switch (command.GetId()) {
        case ADD_GROUP_COMMAND: {
            BuildEnv env(true);
            int id = AddGroup(env, -1, ((AddGroupCommand*)(&command))->code);
            ((AddGroupCommand*)(&command))->SetReturn(Int2Scm(id));
            Redisplay();
            } break;
        
        case INSERT_GROUP_COMMAND: {
            BuildEnv env(true);
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
            } else {
                Error("unknown group %d", pid);
                ((InsertGroupCommand*)(&command))->SetReturn(Int2Scm(-1));
            }
            
            Redisplay();
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
            ReplaceGroupCommand *replace = (ReplaceGroupCommand*) &command;
            BuildEnv env(true);
            
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
                PopulateElement(env, parent, replace->code);
                replace->SetReturn(ScmCadr(ScmCar(replace->code)));
                
               
            } else {
                Error("unknown group %d", replace->groupid);
                replace->SetReturn(Int2Scm(-1));
            }
            
            Redisplay();
            } break;
        
        case CLEAR_GROUPS_COMMAND: {
            // remove root group, GroupTable will create new root
            RemoveHotspots(m_table.GetRootGroup());
            m_table.Clear();
            Redisplay();
            } break;
        
        case SHOW_GROUP_COMMAND: {
            ShowGroupCommand *show = (ShowGroupCommand*) &command;
            
            // toggle a group's visibility
            Group *group = m_table.GetGroup(show->groupid);
            if (group) {
                group->SetVisible(show->visible);
            }
        
            Redisplay();
            } break;
        
        case GET_GROUP_COMMAND: {
            GetGroupCommand *getGroup = (GetGroupCommand*) &command;
            
            Group *group = m_table.GetGroup(getGroup->groupid);
            if (group) {
                BuildEnv env(true);
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
list<Command*> DrawModel::HotspotClick(Vertex2f pos)
{
    list<Command*> cmds, tmpCmds;
    
    // find commands associated with this location
    tmpCmds = m_hotspotClicks.Find(pos);

    // must make a copy of commands to return    
    for (list<Command*>::iterator i=tmpCmds.begin(); i!=tmpCmds.end(); i++)
        cmds.push_back((*i)->Create());
    
    return cmds;
}


// Gets the environment of a group.
// env is the starting environment
// start is the starting element of the descent
BuildEnv DrawModel::GetEnv(BuildEnv &env, Element *start, Element *end)
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


int DrawModel::AddGroup(BuildEnv &env, int parent, Scm code)
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


Group *DrawModel::BuildGroup(BuildEnv &env, Scm code)
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



bool DrawModel::PopulateElement(BuildEnv &env, Element *parent, Scm code)
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


Element *DrawModel::BuildElement(BuildEnv &env, Scm code)
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
                Transform *trans = new Transform();
                Scm first  = ScmCadr(code);
                Scm second = ScmCaddr(code);

                if (!ScmFloatp(first) || !ScmFloatp(second)) {
                    delete elm;
                    return NULL;
                }

                trans->Add(header, Scm2Float(first), Scm2Float(second));
                
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
                Transform *trans = new Transform();
                Scm first  = ScmCadr(code);

                if (!ScmFloatp(first)) {
                    delete elm;
                    return NULL;
                }

                trans->Add(header, Scm2Float(first));

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



/*
    // create children if they exist
    for (Scm children = code;
         ScmConsp(children); 
         children = ScmCdr(children))
    {
        // get code for child
        Scm child = ScmCar(children);
        
        // if integer then it is an implied vertices primitive
        if (ScmFloatp(child)) {
            int len = 0;
            Scm ptr = children;
            
            // determine size of vertices primitive
            do {
                len++;
                ptr = ScmCdr(ptr);
            } while (ScmConsp(ptr) && ScmFloatp(ScmCar(ptr)));
            
            // parse vertices
            if (len % 2 == 0) {
                VerticesPrimitive *vertices = new VerticesPrimitive();

                vertices->len = len;
                vertices->data = new float [len];
                
                for (int i=0; i<len; i++) {
                    vertices->data[i] = Scm2Float(ScmCar(children));
                    children = ScmCdr(children);
                }
                
                graphic->AddPrimitive(vertices);
            } else {
                Error("Odd number of coordinates given for vertices");
                return false;
            }
            
            // determine if loop should continue
            if (!ScmConsp(children))
                break;
            else
                // repair the child pointer
                child = ScmCar(children);
        }
        
        // build primitive and add to graphic
        Primitive *prim = BuildPrimitive(env, child);
        if (prim) {
            graphic->AddPrimitive(prim);
        } else {
            Error("Bad primitive in graphic");
            return false;
        }

    }  
    return true;
}

Primitive *DrawModel::BuildPrimitive(BuildEnv &env, Scm code)
{
    Primitive *prim = NULL;

    // do nothing if not a list
    if (!ScmConsp(code))
        return NULL;
    
    // check that header is int
    if (!ScmIntp(ScmCar(code))) {
        return NULL;
    }
    
    // build element based on header
    switch (Scm2Int(ScmCar(code))) 
    {
        case VERTICES_CONSTRUCT: {
            VerticesPrimitive *vertices = new VerticesPrimitive();
            
            Scm lst = ScmCdr(code);
            vertices->len = ScmLength(lst);
            vertices->data = new float [vertices->len];
            
            for (int i=0; 
                 ScmConsp(lst) && i < vertices->len; 
                 i++, lst = ScmCdr(lst))
            {
                Scm node = ScmCar(lst);
                if (ScmFloatp(node)) {
                    vertices->data[i] = Scm2Float(node);
                } else {
                    Error("Bad vertex coordinate");
                    delete prim;
                    return NULL;
                }
            }
            
            prim = vertices;
            
            } break;
        
        case COLOR_CONSTRUCT: {
            ColorPrimitive *color = new ColorPrimitive();
            
            Scm lst = ScmCdr(code);
            
            for (int i=0; ScmConsp(lst) && i<4; i++, lst = ScmCdr(lst)) {
                Scm node = ScmCar(lst);
                if (ScmFloatp(node)) {
                    color->data[i] = Scm2Float(node);
                } else {
                    Error("Bad color value");
                    delete prim;
                    return NULL;
                }
            }
            
            prim = color;
            
            } break;
            
        default:
            Error("Unknown primitive %d", Scm2Int(ScmCar(code)));
            return NULL;
    }
    
    return prim;
}
*/


Element *DrawModel::BuildHotspot(BuildEnv &env, Scm code)
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
    hotspot->pos1 = pos1;
    hotspot->pos2 = pos2;
    
    // register hotspot
    m_hotspotClicks.Insert(proc, pos1, pos2);
    
    return hotspot;
}


Element *DrawModel::BuildText(BuildEnv &env, Scm code, int kind)
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


Scm DrawModel::GetGroup(BuildEnv &env, Element *elm)
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


void DrawModel::RemoveGroup(int id)
{
    Group *group = m_table.GetGroup(id);
    if (group) {
        RemoveHotspots(group);    
        m_table.RemoveGroup(id);
    }
}


void DrawModel::RemoveHotspots(Element *elm)
{
    if (elm->GetId() == HOTSPOT_CONSTRUCT) {
        m_hotspotClicks.Remove(((Hotspot*)elm)->GetProc());
    } else {
        // recurse
        for (Element::Iterator i=elm->Begin(); i!=elm->End(); i++) {
            RemoveHotspots(*i);
        }
    }
}


void DrawModel::FindBounding(Element *element, 
                         float *top, float *bottom, float *left, float *right,
                         TransformMatrix *matrix)
{
    // loop through children of this element
    for (Element::Iterator i=element->Begin(); i!=element->End(); i++) {
        Element *elm = (*i);
        
        if (IsGraphic(elm->GetId())) {
            Graphic *graphic = (Graphic*) elm;            
            
            // if child is graphic find the bounding box of its vertices
            //for (Graphic::PrimitiveIterator j = 
            //     ((Graphic*)elm)->PrimitivesBegin();
            //     j != ((Graphic*)elm)->PrimitivesEnd(); j++)
            
            for (int ptr = 0; graphic->More(ptr); 
                 ptr = graphic->NextPrimitive(ptr))
            {
                if (graphic->IsVertices(ptr)) {
                    int len = 2 * graphic->GetVerticesLen(ptr);
                    float *data =
                        graphic->GetVertex(graphic->VerticesStart(ptr));
                    
                    for (int k=0; k<len-1; k+=2) {
                        float x, y;
                        matrix->VecMult(data[k], data[k+1], &x, &y);
                        if (x < *left)   *left   = x;
                        if (x > *right)  *right  = x;
                        if (y < *bottom) *bottom = y;
                        if (y > *top)    *top    = y;
                    }
                }
            }
        } else if (IsTransform(elm->GetId())) {
            // if child is transform, apply transform and recurse
            // calculate new transform matrix
            TransformMatrix matrix2 = *matrix;
            MultMatrix(matrix->mat, ((Transform*) elm)->GetMatrix(), 
                       matrix2.mat);

            FindBounding(elm, top, bottom, left, right, &matrix2);
        } else {
            // recurse
            FindBounding(elm, top, bottom, left, right, matrix);
        }
    }
}

}

