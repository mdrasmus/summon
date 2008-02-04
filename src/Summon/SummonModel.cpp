/***************************************************************************
* Summon
* Matt Rasmussen
* SummonModel.cpp
*
*
* This file is part of SUMMON.
* 
* SUMMON is free software: you can redistribute it and/or modify
* it under the terms of the Lesser GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
* 
* SUMMON is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
* 
* You should have received a copy of the Lesser GNU General Public License
* along with SUMMON.  If not, see <http://www.gnu.org/licenses/>.
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
    m_root(NULL),    
    m_hotspotClickSet(1000)
{
    SetRoot(new Group());
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
list<Command*> SummonModel::HotspotClick(Vertex2f pos, const Camera camera, 
                                         int kind)
{
    list<Command*> cmds;
    
    for (list<Hotspot*>::iterator i=m_hotspotClicks.begin(); 
         i!=m_hotspotClicks.end(); i++)
    {
        Hotspot *hotspot = (*i);
    
        if (hotspot->IsCollide(pos, camera, kind)) {
            CallProcCommand *cmd = (CallProcCommand*) hotspot->GetProc()->Create();
            
            if (hotspot->GivePos()) {
                Vertex2f pos2 = hotspot->GetLocalPos(pos, camera);
                Scm args;
                
                if (hotspot->GiveKind()) {
                    cmd->args = BuildScm("dff", kind, pos2.x, pos2.y);
                } else {
                    cmd->args = BuildScm("ff", pos2.x, pos2.y);
                }
            } else {
                cmd->args = Scm_EOL;
            }
            cmds.push_back(cmd);
        }
    }
    
    return cmds;
}


void SummonModel::SetRoot(Element *newroot)
{
    // if newroot is already root, do nothing
    if (newroot == m_root)
        return;
    
    // remove old root
    if (m_root) {
        m_root->DecRef();
        CleanupElement(m_root);
    }
    
    // set new root
    m_root = newroot;
    m_root->SetModel(this);
    m_root->IncRef();
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


Element *SummonModel::ReplaceElement(Element *oldelm, Scm code)
{
    Element *elm = GetElementFromObject(code);
    
    // verify that this is a group
    if (!elm) {
        Error("cannot add graphical element, it is invalid.");
        return NULL;
    }
    
    if (ReplaceElement(oldelm, elm))
        return elm;
    else
        return NULL;
}


bool SummonModel::ReplaceElement(Element *oldelm, Element *newelm)
{
    // TODO: fix

    if (newelm->GetParent() != NULL) {
        Error("element already has parent.");
        return NULL;
    }

    // handle case where root is replaced
    if (oldelm == m_root) {
        SetRoot(newelm);
        Update(newelm);
        return true;
    }

    Element *parent = oldelm->GetParent();
    
    if (parent == NULL) {
        Error("cannot replace element without parent");
        return false;
    }
    
    parent->ReplaceChild(oldelm, newelm);
    CleanupElement(oldelm);
    Update(newelm);
    
    return true;
}


bool SummonModel::RemoveElement(Element *elm)
{
    if (elm->GetModel() != this) {
        Error("element is not in model");
        return false;
    }

    Element *parent = elm->GetParent();
    
    // notify parent
    if (parent) {
        parent->RemoveChild(elm);
        Update(parent);
        CleanupElement(elm);
    } else {
        // if no parent then we must be the root
        assert(elm == m_root);
    
        // make sure model always has a root
        SetRoot(new Group());
        Update();
    }
    
    return true;
}


void SummonModel::CleanupElement(Element *elm)
{
    // remove any hotspots underneath this group
    RemoveHotspots(elm);
    elm->SetModel(NULL);    
    
    // delete only if it is not referenced by python
    if (!elm->IsReferenced())
        delete elm;
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

