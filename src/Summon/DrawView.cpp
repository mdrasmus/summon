/***************************************************************************
* Vistools
* Matt Rasmussen
* MatrixView.cpp
*
***************************************************************************/

#include "first.h"
#include <algorithm>
#include <sstream>
#include <time.h>
#include "common.h"
#include "drawCommands.h"
#include "DrawView.h"



namespace Vistools
{

DrawView::DrawView(DrawModel *model, int width, int height) :
    Glut2DView(width, height),
    m_worldModel(model),
    m_screenModel(NULL),
    m_active(false),
    m_bgColor(0,0,0,1),
    m_executingTasks(false)
{
    SetVisible(0, 0, width, height);
    
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
}


DrawView::~DrawView()
{
}


void DrawView::ExecCommand(Command &command)
{
    MakeCurrentWindow();

    // don't execute anything without a model
    if (!m_worldModel) {
        return;
    }

    switch (command.GetId()) {
        case SET_BGCOLOR_COMMAND:
            SetBgColor(((SetBgColorCommand*)&command)->color);
            Redisplay();
            break;

        case GET_BGCOLOR_COMMAND:
            ((GetBgColorCommand*) &command)->SetReturn(
                ScmCons(Float2Scm(m_bgColor.r),
                 ScmCons(Float2Scm(m_bgColor.g),
                  ScmCons(Float2Scm(m_bgColor.b), Scm_EOL))));
            break;
        
        case SET_VISIBLE_COMMAND: {
            SetVisibleCommand *vis = (SetVisibleCommand*) &command;
            SetVisible(vis->data[0], vis->data[1],
                       vis->data[2], vis->data[3]);
            Redisplay();
            } break;
        
        case GET_VISIBLE_COMMAND: {
            Vertex2i size = GetWindowSize();
            Vertex2f pos1 = ScreenToWorld(0,0);
            Vertex2f pos2 = ScreenToWorld(size.x, size.y);
            
            ((GetVisibleCommand*) &command)->SetReturn(
                ScmCons(Float2Scm(pos1.x),
                 ScmCons(Float2Scm(pos1.y),
                  ScmCons(Float2Scm(pos2.x),
                   ScmCons(Float2Scm(pos2.y), Scm_EOL)))));
            } break;
        
        case SET_WINDOW_SIZE_COMMAND: {
            SetWindowSizeCommand *size = (SetWindowSizeCommand*) &command;
            Resize(size->width, size->height);
            } break;
        
        case GET_WINDOW_SIZE_COMMAND: {
            Vertex2i size = GetWindowSize();
            ((SetWindowSizeCommand*) &command)->SetReturn(
                ScmCons(Int2Scm(size.x),
                 ScmCons(Int2Scm(size.y), Scm_EOL)));
            } break;
        
        case HOME_COMMAND:
            Home();
            Redisplay();
            break;
        
        case SET_ANTIALIAS_COMMAND:
            if (((SetAntialiasCommand*)&command)->enabled) {
                glEnable(GL_POLYGON_SMOOTH);
                glEnable(GL_LINE_SMOOTH);
                glEnable(GL_POINT_SMOOTH);
            } else {
                glDisable(GL_POLYGON_SMOOTH);
                glDisable(GL_LINE_SMOOTH);
                glDisable(GL_POINT_SMOOTH);
            }
            Redisplay();

            break;
        
        case REDISPLAY_COMMAND:
            NoteModelChange();
            Glut2DView::ExecCommand(command);
            break;
        
        default:
            Glut2DView::ExecCommand(command);
    }
}


void DrawView::Home()
{
    if (!m_worldModel)
        return;

    float FLOAT_MIN = -1e307;
    float FLOAT_MAX = 1e307;

    // find smallest bouding box
    int rootid = m_worldModel->GetGroupTable()->GetRoot();
    float top    = FLOAT_MIN, 
          bottom = FLOAT_MAX, 
          left   = FLOAT_MAX, 
          right  = FLOAT_MIN;
    
    TransformMatrix matrix;
    MakeIdentityMatrix(matrix.mat);
    
    // find smallest bounding box
    m_worldModel->FindBounding(m_worldModel->GetGroupTable()->GetGroup(rootid), 
                          &top, &bottom, &left, &right, &matrix);
    
    // set make smallest bounding box visible
    SetVisible(left, bottom, right, top);
}


void DrawView::ExecuteTasks()
{
    m_executingTasks = true;

    for (int i=0; i<m_tasks.size(); i++) {
        switch (m_tasks[i]->GetId()) {
            case TASK_DRAWLIST:
                glCallList(m_tasks[i]->m_drawlist);
                break;
            case TASK_ELEMENT: {
                GroupTable *table = m_worldModel->GetGroupTable();
                DrawElement(table, m_tasks[i]->m_element);
                } break;
            default:
                assert(0);
        }
    }
    
    m_executingTasks = false;
}


void DrawView::DrawWorld()
{
    // don't execute anything without a model
    if (!m_worldModel) {
        return;
    }
    
    glPointSize(GetZoom());
    
    if (m_tasks.size() == 0) {
        // if no tasks, create new drawlist task
        AddTask(new DrawTask(glGenLists(1)));
            
        GroupTable *table = m_worldModel->GetGroupTable();
        DrawElement(table, table->GetGroup(table->GetRoot()));
        
        GetLastTask()->Close();
    } else {
        ExecuteTasks();
    }
}


void DrawView::DrawScreen()
{
    // don't execute anything without a model
    if (!m_screenModel) {
        return;
    }

    glPointSize(1.0);
    GroupTable *table = m_screenModel->GetGroupTable();
    DrawElement(table, table->GetGroup(table->GetRoot()));

}


void DrawView::DrawElement(GroupTable *table, Element *element)
{
    // ignore non-visible elements
    if (!element->IsVisible())
        return;

    // prepprocess dynamic elements
    if (element->IsDynamic() && !IsExecutingTasks()) {
        // close any drawlists that are currently being compiled
        GetLastTask()->Close();
        
        // add a task to draw this dynamic element
        AddTask(new DrawTask(element));
    }
    
    
    // preprocess element
    switch (element->GetId()) {
        // graphics
        case POINTS_CONSTRUCT:
        case LINES_CONSTRUCT:
        case LINE_STRIP_CONSTRUCT:        
        case TRIANGLES_CONSTRUCT:
        case TRIANGLE_STRIP_CONSTRUCT:
        case TRIANGLE_FAN_CONSTRUCT:
        case QUADS_CONSTRUCT:
        case QUAD_STRIP_CONSTRUCT:
        case POLYGON_CONSTRUCT:
            switch (element->GetId()) {
                case POINTS_CONSTRUCT:         glBegin(GL_POINTS); break;
                case LINES_CONSTRUCT:          glBegin(GL_LINES); break;
                case LINE_STRIP_CONSTRUCT:     glBegin(GL_LINE_STRIP); break;   
                case TRIANGLES_CONSTRUCT:      glBegin(GL_TRIANGLES); break;
                case TRIANGLE_STRIP_CONSTRUCT: glBegin(GL_TRIANGLE_STRIP); break;
                case TRIANGLE_FAN_CONSTRUCT:   glBegin(GL_TRIANGLE_FAN); break;
                case QUADS_CONSTRUCT:          glBegin(GL_QUADS); break;
                case QUAD_STRIP_CONSTRUCT:     glBegin(GL_QUAD_STRIP); break;
                case POLYGON_CONSTRUCT:        glBegin(GL_POLYGON); break;
            }

            DrawPrimitives(((Graphic*)element)->PrimitivesBegin(),
                       ((Graphic*)element)->PrimitivesEnd());
            glEnd();
            break;
        
        case TEXT_CONSTRUCT:
            DrawTextElement((TextElement*) element);
            break;
        
        case TRANSFORM_CONSTRUCT:
            glPushMatrix();

            {
                Transform *trans = (Transform*) element;
                for (Transform::Iterator i = trans->TransformsBegin();
                     i != trans->TransformsEnd(); i++)
                {
                    switch ((*i).kind) {
                        case TRANSLATE_CONSTRUCT:
                            glTranslatef((*i).first, (*i).second, 0);
                            break;
                        case ROTATE_CONSTRUCT:
                            glRotatef((*i).first, 0, 0, 1);
                            break;
                        case SCALE_CONSTRUCT:
                            glScalef((*i).first, (*i).second, 0);
                            break;
                        case FLIP_CONSTRUCT:
                            glRotatef(180, (*i).first, (*i).second, 0);
                            break;
                    }
                }
            }
            break;

        // do nothing
        case HOTSPOT_CONSTRUCT:
        case GROUP_CONSTRUCT:
            break;

        default:
            Error("Unknown element %d", element->GetId());
            assert(0);
    }
    
    
    // exec element's children
    for (Element::Iterator i=element->Begin(); i!=element->End(); i++) {
        DrawElement(table, *i);
    }
    
    
    // postprocess element
    switch (element->GetId()) {
        case TRANSFORM_CONSTRUCT:
            glPopMatrix();
            break;
    }
    
    
    // postprocess dynamic elements
    if (element->IsDynamic() && !IsExecutingTasks()) {
        // start a new drawlist task for later elements
        AddTask(new DrawTask(glGenLists(1)));
    }
}


void DrawView::DrawPrimitives(Graphic::PrimitiveIterator begin, 
                              Graphic::PrimitiveIterator end)
{
    for (Graphic::PrimitiveIterator i=begin; i!=end; i++) {
        switch ((*i)->GetId()) {
            case VERTICES_CONSTRUCT: {
                VerticesPrimitive *vertices = (VerticesPrimitive*) (*i);
                
                for (int i=0; i<vertices->len - 1; i+=2) {
                    glVertex2fv(&vertices->data[i]);
                }
                
                } break;
            
            case COLOR_CONSTRUCT: {
                ColorPrimitive *color = (ColorPrimitive*) (*i);
                glColor4fv(color->data);
                } break;
            
            default:
                Error("Unkkown primitive %d", (*i)->GetId());
                assert(0);
        }
    }
}


void DrawView::DrawTextElement(TextElement *elm)
{
    const unsigned char *text = (const unsigned char*) elm->text.c_str();

    if (elm->kind == TextElement::KIND_BITMAP) {
        void *font = GLUT_BITMAP_8_BY_13;

        Vertex2f pos1 = elm->pos1;
        Vertex2f pos2 = elm->pos2;

        // find text on-screen size
        float textWidth  = glutBitmapLength(font, text) / GetZoom();
        float textHeight = 13.0 / GetZoom();
        float boxWidth   = pos2.x - pos1.x;
        float boxHeight  = pos2.y - pos1.y;

        // draw nothing if text does not fit in bounding box
        if ((textWidth > boxWidth) || 
            (textHeight > boxHeight))
            return;

        // find drawing point based on justification
        Vertex2f pos = pos1;

        if (elm->justified & TextElement::LEFT)
            pos.x = pos1.x;
        else if (elm->justified & TextElement::CENTER)
            pos.x = pos1.x + (boxWidth - textWidth) / 2;
        else if (elm->justified & TextElement::RIGHT)
            pos.x = pos2.x - textWidth;

        if (elm->justified & TextElement::TOP)
            pos.y = pos2.y - textHeight;
        else if (elm->justified & TextElement::MIDDLE)
            pos.y = pos1.y + (boxHeight - textHeight) / 2;
        else if (elm->justified & TextElement::BOTTOM)
            pos.y = pos1.y;

        DrawText(font, elm->text, pos.x, pos.y);
        
    } else if (elm->kind == TextElement::KIND_SCALE) {
        void *font = GLUT_STROKE_ROMAN;
        float fontSize = 119.05;

        Vertex2f pos1 = elm->pos1;
        Vertex2f pos2 = elm->pos2;
        
        float textWidth  = glutStrokeLength(font, text);
        float textHeight = fontSize;
        float boxWidth   = pos2.x - pos1.x;
        float boxHeight  = pos2.y - pos1.y;

        float xscale = boxWidth / textWidth;
        float yscale = boxHeight / textHeight;
        float scale = min(xscale, yscale);

        textWidth *= scale;
        textHeight *= scale;

        // find drawing point based on justification
        Vertex2f pos = pos1;

        if (elm->justified & TextElement::LEFT)
            pos.x = pos1.x;
        else if (elm->justified & TextElement::CENTER)
            pos.x = pos1.x + (boxWidth - textWidth) / 2;
        else if (elm->justified & TextElement::RIGHT)
            pos.x = pos2.x - textWidth;

        if (elm->justified & TextElement::TOP)
            pos.y = pos2.y - textHeight;
        else if (elm->justified & TextElement::MIDDLE)
            pos.y = pos1.y + (boxHeight - textHeight) / 2;
        else if (elm->justified & TextElement::BOTTOM)
            pos.y = pos1.y;
        
        const unsigned char *chr = text;
        glPushMatrix();
        glTranslatef(pos.x, pos.y, 0);
        glScalef(textHeight/fontSize, textHeight/fontSize, textHeight/fontSize);
        for (; *chr; chr++)
            glutStrokeCharacter(font, *chr);
        glPopMatrix();
    }
}


void DrawView::DrawText(void *font, string text, float x, float y)
{
    glRasterPos2f(x, y);
    for(unsigned int i=0; i<text.size(); i++ )
        glutBitmapCharacter(font , (text.c_str())[i] );
}


void DrawView::SetBgColor(Color &color)
{
    m_bgColor = color;
    glClearColor(color.r, color.g, color.b, color.a);
}


}
