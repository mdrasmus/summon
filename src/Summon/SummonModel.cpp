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



SummonModel::SummonModel(int id, ModelKind kind) :
    m_id(id),
    m_kind(kind),
    m_hotspotClickSet(1000)
{
    m_root = new Group();
    m_root->SetModel(this);
    m_root->IncRef();
    Update();
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
            
            show->SetReturn(BuildScm("ffff", pos1.x, pos1.y, pos2.x, pos2.y));
            } break;
        default:
            // unknown command given
            assert(0);
    }
}

// This function is called when a HotspotClick command is executed
// Checks to see if any hotspots have been activated
list<Command*> SummonModel::HotspotClick(Vertex2f pos, const Camera camera)
{
    list<Command*> cmds;
    
    for (list<Hotspot*>::iterator i=m_hotspotClicks.begin(); 
         i!=m_hotspotClicks.end(); i++)
    {
        Hotspot *hotspot = (*i);
    
        if (hotspot->IsCollide(pos, camera)) {
            // TODO: add function for passing click pos as argument to 
            // proc command
            CallProcCommand *cmd = (CallProcCommand*) hotspot->GetProc()->Create();
            
            if (hotspot->GivePos()) {
                Vertex2f pos2 = hotspot->GetLocalPos(pos, camera);
                Scm mousePos = BuildScm("ff", pos2.x, pos2.y);
                cmd->args = mousePos;
            } else {
                cmd->args = Scm_EOL;
            }
            cmds.push_back(cmd);
        }
    }
    
    return cmds;
}



Element *SummonModel::AddElement(Element *parent, Scm code)
{
    Element *elm = GetElementFromObject(code);
    
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



void SummonModel::Update(Element *element)
{
    if (element == NULL)
        element = GetRoot();

    UpdateRecurse(element);
    ModelChanged();
}


void SummonModel::UpdateRecurse(Element *element)
{    
    element->SetModel(this);
    element->Update();
    
    // perform element-specific updating
    switch (element->GetId()) {
        case HOTSPOT_CONSTRUCT:
            // apply current env transform to coordinates
            UpdateHotspot((Hotspot*) element);      
            break;
        
        case TEXT_CONSTRUCT:
            ((TextElement*) element)->modelKind = m_kind;
            break;        
    }
    
    // recurse through children
    for (Element::Iterator i=element->Begin(); i!=element->End(); i++) {
        UpdateRecurse(*i);
    }
}


void SummonModel::UpdateHotspot(Hotspot *hotspot)
{    
    // register hotspot
    if (!m_hotspotClickSet.HasKey(hotspot)) {
        m_hotspotClickSet.Insert(hotspot, true);
        m_hotspotClicks.push_back(hotspot);
    }
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


// TODO: this needs a camera argument, but maybe not.
void SummonModel::FindBounding(Element *elm, Vertex2f *pos1, Vertex2f *pos2)
{
    const Camera camera;
    const float FLOAT_MIN = -1e307;
    const float FLOAT_MAX = 1e307;

    // find smallest bouding box
    float top    = FLOAT_MIN, 
          bottom = FLOAT_MAX, 
          left   = FLOAT_MAX, 
          right  = FLOAT_MIN;

    TransformMatrix matrix;
    
    if (elm) {
        const TransformMatrix *tmp = elm->GetTransform(&matrix, camera);
        CopyMatrix(matrix.mat, tmp->mat);
    } else {
        elm = GetRoot();
        MakeIdentityMatrix(matrix.mat);
    }
        
    // TODO: should probably send camera?  or would that be more expensive?
    elm->FindBounding(&top, &bottom, &left, &right, &matrix);
    
    pos1->x = left;
    pos1->y = bottom;
    pos2->x = right;
    pos2->y = top;
}


} // namespace Summon

