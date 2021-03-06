/***************************************************************************
* Summon
* Matt Rasmussen
* SummonView.cpp
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

// c/c++ includes
#include <algorithm>
#include <sstream>
#include <time.h>

// summon includes
#include "common.h"
#include "summonCommands.h"
#include "SummonView.h"



namespace Summon
{

SummonView::SummonView(SummonModel *model, int width, int height, 
                       const char *name, int left, int top) :
    Glut2DView(width, height, name, left, top),
    m_worldModel(model),
    m_screenModel(NULL),
    m_bgColor(0,0,0,0),
    m_active(false),
    m_showCrosshair(false),
    m_crosshairColor(1,1,1,1),
    m_mousePos(0,0),
    m_boundary1(FLOAT_MIN, FLOAT_MIN),
    m_boundary2(FLOAT_MAX, FLOAT_MAX),
    m_needRedisplay(false),
    m_styleChange(false),
    m_defaultStyle(Color(1, 1, 1))
    
{
    SetVisible(0, 0, width, height);


    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
    
#   ifdef GL_MULTISAMPLE
        // try to use multisample, freeglut usually doesn't support it
        glEnable(GL_MULTISAMPLE);
#   endif
#   ifdef GL_MULTISAMPLE_ARB
        glEnable(GL_MULTISAMPLE_ARB);
#   endif
    //glEnable(GL_POINT_SMOOTH);  
    glDisable(GL_POINT_SMOOTH);  
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
    //glEnable(GL_POLYGON_SMOOTH);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    
     
}


SummonView::~SummonView()
{
}


void SummonView::ExecCommand(Command &command)
{
    // TODO: this might be too restrictive
    // don't execute anything without a model
    if (!m_worldModel) {
        return;
    }


    switch (command.GetId()) {
        case SET_WINDOW_NAME_COMMAND: {
            string name = ((SetWindowNameCommand*)&command)->name;
            MakeCurrentWindow();
            SetName((char*) name.c_str());
            } break;
        
        
        case GET_WINDOW_NAME_COMMAND: {
            ((ScriptCommand*) &command)->SetReturn(String2Scm(GetName().c_str()));
            } break;
        
        case SET_WINDOW_POSITION_COMMAND: {
            int x = ((SetWindowPositionCommand*)&command)->x;
            int y = ((SetWindowPositionCommand*)&command)->y;
            
            MakeCurrentWindow();
            SetPosition(x, y);
            Redisplay();
            } break;
            
        case GET_WINDOW_POSITION_COMMAND: {
            Vertex2i pos = GetPosition();
            ((ScriptCommand*) &command)->SetReturn(BuildScm("dd", pos.x, pos.y));
            } break;

        
        case SET_WINDOW_SIZE_COMMAND: {
            MakeCurrentWindow();
            SetWindowSizeCommand *size = (SetWindowSizeCommand*) &command;
            Resize(size->width, size->height);
            } break;
        
        case GET_WINDOW_SIZE_COMMAND: {
            Vertex2i size = GetWindowSize();
            ((ScriptCommand*) &command)->SetReturn(BuildScm("dd", size.x, size.y));
            } break;
        
        case SET_WINDOW_BOUNDARY_COMMAND: {
            SetWindowBoundaryCommand *cmd = (SetWindowBoundaryCommand*) &command;
            SetBoundary(cmd->pos1, cmd->pos2);
            PostRedisplay();
            } break;
        
        case GET_WINDOW_BOUNDARY_COMMAND: {
            ((ScriptCommand*) &command)->SetReturn(
                BuildScm("ffff", m_boundary1.x, m_boundary1.y, 
                         m_boundary2.x, m_boundary2.y));
            } break;
            
        
        case RAISE_WINDOW_COMMAND: {
            // raise or lower the window
            RaiseWindowCommand *cmd = (RaiseWindowCommand*) &command;
            Raise(cmd->raise);
            } break;
        
        case SET_TRANS_COMMAND: {
            SetTransCommand *trans = (SetTransCommand*) &command;
            TranslateTo(trans->x, trans->y);
            CheckBoundary();
            PostRedisplay();
            } break;
        
        case GET_TRANS_COMMAND: {
            ((ScriptCommand*) &command)->SetReturn(
                BuildScm("ff", m_trans.x, m_trans.y));
            } break;
            
        case SET_ZOOM_COMMAND: {
            SetZoomCommand *zoom = (SetZoomCommand*) &command;
            ZoomTo(zoom->x, zoom->y);
            CheckBoundary();
            PostRedisplay();
            } break;
            
        case GET_ZOOM_COMMAND: {
            ((ScriptCommand*) &command)->SetReturn(
                BuildScm("ff", m_zoom.x, m_zoom.y));
            } break;
    
        case SET_FOCUS_COMMAND: {
            SetFocusCommand *focus = (SetFocusCommand*) &command;
            SetFocus(focus->x, focus->y);
            } break;
            
        case GET_FOCUS_COMMAND: {
            ((ScriptCommand*) &command)->SetReturn(
                BuildScm("ff", m_focus.x, m_focus.y));
            } break;
    
        case SET_BGCOLOR_COMMAND:
            SetBgColor(((SetBgColorCommand*)&command)->color);
            PostRedisplay();
            break;

        case GET_BGCOLOR_COMMAND:
            ((GetBgColorCommand*) &command)->SetReturn(
                BuildScm("fff", m_bgColor.r, m_bgColor.g, m_bgColor.b));
            break;
        
        case SET_VISIBLE_COMMAND: {       
            SetVisibleCommand *vis = (SetVisibleCommand*) &command;
            SetVisible(vis->data[0], vis->data[1],
                       vis->data[2], vis->data[3]);
            CheckBoundary();
            PostRedisplay();
            } break;
        
        case GET_VISIBLE_COMMAND: {
            Vertex2i size = GetWindowSize();
            Vertex2d pos1 = ScreenToWorld(0,0);
            Vertex2d pos2 = ScreenToWorld(size.x, size.y);
            
            ((GetVisibleCommand*) &command)->SetReturn(
                BuildScm("ffff", pos1.x, pos1.y, pos2.x, pos2.y));
            } break;
        
        
        case SET_ANTIALIAS_COMMAND:
            MakeCurrentWindow();
            if (((SetAntialiasCommand*)&command)->enabled) {
                glEnable(GL_POLYGON_SMOOTH);
                glEnable(GL_LINE_SMOOTH);
                glEnable(GL_POINT_SMOOTH);
            } else {
                glDisable(GL_POLYGON_SMOOTH);
                glEnable(GL_LINE_SMOOTH);
                glDisable(GL_POINT_SMOOTH);
            }
            Redisplay();

            break;
        
        case SHOW_CROSSHAIR_COMMAND:
            m_showCrosshair = ((ShowCrosshairCommand*)&command)->enabled;
            break;
        
        case SET_CROSSHAIR_COLOR_COMMAND:
            m_crosshairColor = ((SetCrosshairColorCommand*)&command)->color;
            break;
        
        case MODEL_CHANGED_COMMAND:
            PostRedisplay();
            break;
        
        //=============================================================
        // camera commands
        case TRANSLATE_SCRIPT_COMMAND: {
            MakeCurrentWindow();
            TranslateBy(((TranslateScriptCommand*)(&command))->trans.x, 
                        ((TranslateScriptCommand*)(&command))->trans.y);
            CheckBoundary();
            Redisplay();
            } break;
    
        case ZOOM_SCRIPT_COMMAND: {
            MakeCurrentWindow();
            ZoomBy(((ZoomScriptCommand*) &command)->zoom.x,
                   ((ZoomScriptCommand*) &command)->zoom.y);
            CheckBoundary();
            Redisplay();
            } break;
        
        case ZOOM_X_SCRIPT_COMMAND: {
            MakeCurrentWindow();        
            ZoomBy(((ZoomScriptCommand*) &command)->zoom.x, 1.0);
            CheckBoundary(true, false);
            Redisplay();
            } break;
        
        case ZOOM_Y_SCRIPT_COMMAND: {
            MakeCurrentWindow();
            ZoomBy(1.0, ((ZoomScriptCommand*) &command)->zoom.y);
            CheckBoundary(false, true);
            Redisplay();
            } break;
            
        case FOCUS_SCRIPT_COMMAND: {
            FocusScriptCommand* cmd = (FocusScriptCommand*) &command;
            Vertex2d focus = WindowToWorld(cmd->focus.x, cmd->focus.y);
            SetFocus(focus.x, focus.y);
            } break;
        
        case TRANSLATE_COMMAND:
        case ZOOM_COMMAND:        
            MakeCurrentWindow();
            Glut2DView::ExecCommand(command);
            CheckBoundary();
            break;
        
        case ZOOM_X_COMMAND:
            MakeCurrentWindow();
            Glut2DView::ExecCommand(command);
            CheckBoundary(true, false);
            break;

        case ZOOM_Y_COMMAND:
            MakeCurrentWindow();
            Glut2DView::ExecCommand(command);
            CheckBoundary(false, true);
            break;

        
        //=========================================================
        // menus
        case ATTACH_MENU_COMMAND: {
            AttachMenuCommand *cmd = (AttachMenuCommand*) &command;
            int button = -1;
            
            switch (cmd->button) {
                case 0: button = GLUT_LEFT_BUTTON; break;
                case 1: button = GLUT_MIDDLE_BUTTON; break;
                case 2: button = GLUT_RIGHT_BUTTON; break;
                default:
                    Error("unknown menu button '%d'", cmd->button);
            }
            
            MakeCurrentWindow();
            if (button != -1)
                AttachMenu(cmd->menuid, button);
            
            } break;
        
        
        case DETACH_MENU_COMMAND: {
            DetachMenuCommand *cmd = (DetachMenuCommand*) &command;
            int button = -1;
            
            switch (cmd->button) {
                case 0: button = GLUT_LEFT_BUTTON; break;
                case 1: button = GLUT_MIDDLE_BUTTON; break;
                case 2: button = GLUT_RIGHT_BUTTON; break;
                default:
                    Error("unknown menu button '%d'", cmd->button);
            }
            
            MakeCurrentWindow();
            if (button != -1)
                DetachMenu(cmd->menuid, button);
            
            } break;
        
        
        default:
            MakeCurrentWindow();
            Glut2DView::ExecCommand(command);
    }
}




// TODO: everytime the model is changed all tasks are discarded.  Instead only
// those tasks affect by the change should ideally be altered.
void SummonView::ExecuteTasks()
{
    for (unsigned int i=0; i<m_tasks.size(); i++) {
        switch (m_tasks[i]->GetId()) {
            case TASK_DRAWLIST:
                glCallList(m_tasks[i]->m_drawlist);
                break;
            case TASK_ELEMENT:
                // assumes only world elements are on task list
                DrawElement(m_tasks[i]->m_element, m_defaultStyle, false);
                break;
            default:
                assert(0);
        }
    }
}


void SummonView::Display()
{
    UpdateCamera();

    glClearColor(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a);

    // clear screen and initialize coordinate system
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw world in its own coordinate system
    glPushMatrix();
    TransformWorld();
    DrawWorld();
    glPopMatrix();   

    // draw graphics with the screen coordinate system
    DrawScreen();
    
    // draw crosshair
    if (m_showCrosshair)
        DrawCrosshair();
    
    // display new frame
    SwapBuffers();
}


void SummonView::DrawWorld()
{
    // don't execute anything without a model
    if (!m_worldModel) {
        return;
    }
    
    // set point size to max of zooms
    Vertex2d zoom = GetZoom();
    float pointsize;
    if (zoom.y > zoom.x)
        pointsize = zoom.y;
    else
        pointsize = zoom.x;
    //if (pointsize < 1.5) 
    //    pointsize = 1.5;
    glPointSize(pointsize);
    glColor4f(m_defaultStyle.color.r, 
              m_defaultStyle.color.g,
              m_defaultStyle.color.b,
              m_defaultStyle.color.a);
    
    m_styleChange = false;
    m_curStyle = m_defaultStyle;
    
    // NOTE: I am not using draw lists effectively.
    // It's fast to construct with out them.
    if (m_tasks.size() == 0) {
        // if no tasks, create new drawlist task
        //AddTask(new DrawTask(glGenLists(1)));
        
        DrawElement(m_worldModel->GetRoot(), m_defaultStyle, false); // true
        
        //GetLastTask()->Close();
        
        //printf("tasks %d\n", m_tasks.size());
    } else {
        ExecuteTasks();
    }
    
    //if (m_listener)
    //    m_listener->ViewRedraw(this);
}


void SummonView::DrawScreen()
{
    // don't execute anything without a model
    if (!m_screenModel) {
        return;
    }

    glPointSize(1.0);
    DrawElement(m_screenModel->GetRoot(), m_defaultStyle, false);

}


void SummonView::SetMousePos(int x, int y) {
    m_mousePos.x = x;
    m_mousePos.y = y;
    
    if (m_showCrosshair)
        Redisplay();
}

void SummonView::DrawCrosshair()
{
    // get dimensions
    Vertex2i pos = WindowToScreen(m_mousePos.x, m_mousePos.y);
    Vertex2i winsize = GetWindowSize();
    
    // draw crosshair
    glDisable(GL_LINE_SMOOTH);
    
    glBegin(GL_LINES);
    glColor4f(m_crosshairColor.r, 
              m_crosshairColor.g, 
              m_crosshairColor.b, 
              m_crosshairColor.a);
    glVertex3i(0, pos.y, 0);
    glVertex3i(winsize.x, pos.y, 0);
    glVertex3i(pos.x, 0, 0);
    glVertex3i(pos.x, winsize.y, 0);
    glEnd();
    
    glEnable(GL_LINE_SMOOTH);
}


void SummonView::DrawElement(Element *element, const Style &lastStyle, 
                             bool createTasks)
{
    //bool stylePushed = false;
    //Style newStyle;
    bool drawChildren = true;
    
    // handle styles
    m_styleChange = false;

    // ignore non-visible elements
    if (!element->IsVisible())
        return;

    // prepprocess dynamic elements
    if (createTasks) {
        if (element->IsDynamic()) {
            // close any drawlists that are currently being compiled
            if (GetLastTask()->IsOpen())
                GetLastTask()->Close();
            
            // add a task to draw this dynamic element
            AddTask(new DrawTask(element));
        } else {
            // if the last task is closed open a new draw list
            // NOTE: dynamic tasks will close themselves
            if (!GetLastTask()->IsOpen())
                AddTask(new DrawTask(glGenLists(1)));
        }
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
            
            DrawGraphic((Graphic*) element);
            
            glEnd();
            break;
        
        case COLOR_CONSTRUCT:
            DrawGraphic((Graphic*) element);
            break;
        
        case TEXT_CONSTRUCT:
            DrawTextElement((TextElement*) element);
            break;
        
        case TRANSFORM_CONSTRUCT:
            glPushMatrix();

            {
                Transform *trans = (Transform*) element;
                switch (trans->GetKind()) {
                    case TRANSLATE_CONSTRUCT:
                        glTranslatef(trans->GetParam1(), trans->GetParam2(), 0);
                        break;
                    case ROTATE_CONSTRUCT:
                        glRotatef(trans->GetParam1(), 0, 0, 1);
                        break;
                    case SCALE_CONSTRUCT:
                        glScalef(trans->GetParam1(), trans->GetParam2(), 0);
                        break;
                    case FLIP_CONSTRUCT: {
                        double angle = acos(trans->GetParam1()) / M_PI * 180;
                        glRotated(-angle, 0, 0, 1);
                        glScaled(1.0, -1.0, 0.0);
                        glRotated(angle, 0, 0, 1);
                        }
                        break;
                }
            }
            break;
        
        case ZOOM_CLAMP_CONSTRUCT:
            glPushMatrix();
            drawChildren = DrawZoomClamp((ZoomClamp*) element);
            
            break;

        // do nothing
        case HOTSPOT_CONSTRUCT:
        case GROUP_CONSTRUCT:
        case CUSTOM_GROUP_CONSTRUCT:
            break;

        default:
            Error("Unknown element %d", element->GetId());
            assert(0);
    }

    
    // exec element's children
    if (drawChildren) {
        for (Element *elm=element->GetChild(); elm; elm=elm->GetNext()) {
            DrawElement(elm, lastStyle, createTasks);
        }
    }
    
    // postprocess dynamic elements
    if (createTasks) {
        if (!element->IsDynamic() && !GetLastTask()->IsOpen()) {
            // start a new drawlist task for later elements
            AddTask(new DrawTask(glGenLists(1)));
        }
    }
    
    // postprocess element
    switch (element->GetId()) {
        case TRANSFORM_CONSTRUCT:
        case ZOOM_CLAMP_CONSTRUCT:
            glPopMatrix();
            break;
    }
    
    // postprocess dynamic elements
    if (createTasks) {
        if (element->IsDynamic()) {
            GetLastTask()->Close();
        }
    }

}


bool SummonView::DrawZoomClamp(ZoomClamp *zoomClamp)
{
    // if trans == NULL then zoom_clamp is clipped
    if (!zoomClamp->IsClipped(m_camera)) {
        // perform zoom clamp transform
        TransformMatrix matrix;  // working temp space        
        const TransformMatrix *trans = zoomClamp->GetTransform(&matrix, m_camera);
    
        // reset transform to simply camera transform
        glLoadIdentity();
        TransformWorld();
        
        // convert matrix to column-major
        // TODO: switch my matrices to column-major
        const float *m = trans->mat;
        float tmp[16] = {m[0], m[4], m[8],  m[12],
                         m[1], m[5], m[9],  m[13],
                         m[2], m[6], m[10], m[14],
                         m[3], m[7], m[11], m[15]};

        glMultMatrixf(tmp);        
        return true;
    } else
        return false;
}


void SummonView::DrawGraphic(Graphic *graphic)
{
    for (int ptr = 0; graphic->More(ptr); ptr = graphic->NextPrimitive(ptr)) {
        switch (graphic->GetTag(ptr)) {
            case Graphic::PRIM_VERTICES: {
                int end = graphic->VerticesEnd(ptr);
                for (int ptr2=graphic->VerticesStart(ptr); 
                     ptr2<end; 
                     ptr2 = graphic->NextVertex(ptr2))
                {
                    glVertex2fv(graphic->GetVertex(ptr2));
                }
                } break;
                
            case Graphic::PRIM_COLOR:
                glColor4ubv((GLubyte*) graphic->GetColor(ptr));
                //m_styleChange = true;
                //m_curStyle.color.Set4(graphic->GetColor(ptr));
                break;
        }
    }
}




bool SummonView::WithinView(const Vertex2d &pos1, const Vertex2d &pos2)
{
    Vertex2i size = GetWindowSize();
    Vertex2i spos1 = WorldToScreen(pos1.x, pos1.y);
    Vertex2i spos2 = WorldToScreen(pos2.x, pos2.y);
    
    // find overlap
    return (spos2.x >= 0 && spos1.x <= size.x && 
            spos2.y >= 0 && spos1.y <= size.y);
}



void SummonView::DrawTextElement(TextElement *elm)
{
    const unsigned char *text = (const unsigned char*) elm->text.c_str();
    
    // get text bounding box
    Vertex2f pos1 = elm->pos1;
    Vertex2f pos2 = elm->pos2;
    
    // find scaling
    Vertex2f scale;
    TransformMatrix tmp;
    const TransformMatrix *matrix = elm->GetTransform(&tmp, m_camera);
    matrix->GetScaling(&scale.x, &scale.y);
    
    
    // find environment position
    Vertex2f envpos1, envpos2;
    
    matrix->VecMult(pos1.x, pos1.y, &envpos1.x, &envpos1.y);
    matrix->VecMult(pos2.x, pos2.y, &envpos2.x, &envpos2.y);
    if (envpos1.x > envpos2.x)
        swap(envpos1.x, envpos2.x);
    if (envpos1.y > envpos2.y)
        swap(envpos1.y, envpos2.y);    
    
    
    // do not draw text that is not visible
    // get bounding visible view
    
    if (elm->modelKind == MODEL_WORLD && 
        !WithinView(envpos1, envpos2)) 
    {
        return;
    }
    
    if (elm->kind == TextElement::KIND_BITMAP) {
        // Bitmap text
        
        void *font = GLUT_BITMAP_8_BY_13;
        Vertex2f zoom = GetZoom();
        if (elm->modelKind == MODEL_SCREEN) {
            zoom.x = 1.0;
            zoom.y = 1.0;
        }

        // find text on-screen size
        float textWidth  = glutBitmapLength(font, text) / zoom.x / scale.x;
        float textHeight = 13.0 / zoom.y / scale.y;
        float boxWidth   = pos2.x - pos1.x;
        float boxHeight  = pos2.y - pos1.y;

        // draw nothing if text does not fit in bounding box
        if ((textWidth > boxWidth) || 
            (textHeight > boxHeight))
            return;

        Vertex2f pos = JustifyBox(elm->justified, pos1, pos2, 
                                  textWidth, textHeight, boxWidth, boxHeight);
        DrawText(font, elm->text, pos.x, pos.y);
        
    } else if (elm->kind == TextElement::KIND_SCALE) {
        // Scale text
    
        // TODO: make text size a property of TextElement
        // and move this code to TextElement such that it executes only once.
        static const void *font = GLUT_STROKE_MONO_ROMAN;
        static const double fontSize = getTextHeight(font);
        double textWidth  = getTextWidth(font, text);
        double textHeight = fontSize;
        
        double boxWidth   = pos2.x - pos1.x;
        double boxHeight  = pos2.y - pos1.y;

        double xscale = boxWidth / textWidth;
        double yscale = boxHeight / textHeight;
        double tscale = min(xscale, yscale);

        textWidth *= tscale;
        textHeight *= tscale;

        Vertex2d pos = JustifyBox(elm->justified, pos1, pos2, 
                                  textWidth, textHeight, boxWidth, boxHeight);
        
        const unsigned char *chr = text;
        glPushMatrix();
        glTranslated(pos.x, pos.y, 0);

        // NOTE: freeglut 2.6.0 uses points in its text. 
        // Therefore, we must set point size 1 and restore it afterwards.
        float pointsize;
        glGetFloatv(GL_POINT_SIZE, &pointsize);
        glPointSize(1.0);
        glScaled(textHeight/fontSize, textHeight/fontSize, textHeight/fontSize);
        for (; *chr; chr++)
            glutStrokeCharacter((void*) font, *chr);

        // restore GL state
        glPopMatrix();
        glPointSize(pointsize);
    }
}


void SummonView::DrawText(void *font, string text, float x, float y)
{
    const char *str = text.c_str();
    glRasterPos2f(x, y);
    for(unsigned int i=0; i<text.size(); i++ )
        glutBitmapCharacter(font , str[i]);
}


void SummonView::SetBgColor(Color &color)
{
    m_bgColor = color;
    m_bgColor.a = 0.0;
}


void SummonView::CopyPixels(void *pixels)
{
    MakeCurrentWindow();
    Vertex2i winsize = GetWindowSize();
    glReadPixels(0, 0, winsize.x, winsize.y, 
        GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) pixels);
}


void SummonView::SetBoundary(const Vertex2d &pos1, const Vertex2d &pos2)
{
    m_boundary1 = pos1;
    m_boundary2 = pos2;
    
    if (m_boundary1.x > m_boundary2.x)
        swap(m_boundary1.x, m_boundary2.x);
    if (m_boundary1.y > m_boundary2.y)
        swap(m_boundary1.y, m_boundary2.y);
    
    CheckBoundary();
}


// adjust translate and zoom as necessary to maintain boundary
void SummonView::CheckBoundary(bool useZoomx, bool useZoomy)
{
    // find visible
    Vertex2i winsize = GetWindowSize();
    Vertex2d vis1 = ScreenToWorld(0, 0);
    Vertex2d vis2 = ScreenToWorld(winsize.x, winsize.y);
    
    // adjust zoom
    const Vertex2d boundarySize = m_boundary2 - m_boundary1;    
    Vertex2d visSize = vis2 - vis1;
    Vertex2d oldvisSize = visSize;
    float zoomx = 1.0, zoomy = 1.0;
    
    // check width
    if (visSize.x > boundarySize.x) {
        const float ratio = visSize.x / boundarySize.x;
        if (useZoomx) {
            visSize.x = boundarySize.x;
            zoomx *= ratio;
        }
                
        if (useZoomy) {
            visSize.y /= ratio;
            zoomy *= ratio;
        }
    }
    
    // check height
    if (visSize.y > boundarySize.y) {
        const float ratio = visSize.y / boundarySize.y;
        
        if (useZoomx) {
            visSize.x /= ratio;
            zoomx *= ratio;
        }
         
        if (useZoomy) {   
            visSize.y = boundarySize.y;
            zoomy *= ratio;
        }
    }
    
    ZoomBy(zoomx, zoomy);
    
    // recalc visible region
    vis1 = ScreenToWorld(0, 0);
    vis2 = ScreenToWorld(winsize.x, winsize.y);
    
    // adjust translate
    Vertex2d trans(0, 0);
    
    if (vis1.x < m_boundary1.x) {
        trans.x = m_boundary1.x - vis1.x;
    } else if (vis2.x > m_boundary2.x) {
        trans.x = m_boundary2.x - vis2.x;
    }
    
    if (vis1.y < m_boundary1.y) {
        trans.y = m_boundary1.y - vis1.y;
    } else if (vis2.y > m_boundary2.y) {
        trans.y = m_boundary2.y - vis2.y;
    }

    TranslateBy(trans.x*m_zoom.x, trans.y*m_zoom.y);
}

} // namespace Summon
