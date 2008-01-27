/***************************************************************************
* Summon
* Matt Rasmussen
* glut2DCommands.h
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

#ifndef SUMMON_GLUT_2D_COMMANDS_H
#define SUMMON_GLUT_2D_COMMANDS_H

#include "StringCommand.h"
#include "Factory.h"
#include "glutInputs.h"
#include "Vertex.h"

namespace Summon
{

void glutCommandsInit();

enum {
    GLUT_COMMANDS_BEGIN = 1000,
    REDISPLAY_COMMAND,
    TRANSLATE_COMMAND,
    ZOOM_COMMAND,
    ZOOM_X_COMMAND,
    ZOOM_Y_COMMAND,
    FOCUS_COMMAND,
    GLUT_COMMANDS_END
};


class RedisplayCommand : public Command
{
public:
    virtual Command* Create() { return new RedisplayCommand(); }
    virtual CommandId GetId() { return REDISPLAY_COMMAND; }
};



class TranslateCommand : public StringCommand
{
public:
    virtual Command* Create() { return new TranslateCommand(); }
    virtual CommandId GetId() { return TRANSLATE_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_MOTION_INPUT) {
            trans = ((MouseMotionInput*)(&input))->vel;
            trans.y *= -1;
        }
    }

    virtual const char *GetOptionName() { return "-trans"; }
    virtual const char *GetName() { return "trans"; }
    virtual const char *GetUsage() { return "x, y"; }
    virtual const char *GetDescription() 
    { return "translate the view by (x,y)"; }
    
    virtual int Setup(int argc, const char **argv)
    {
        if (argc >= 3) {
            trans.x = atof(argv[1]);
            trans.y = atof(argv[2]);
            return 3;
        }
        return -1;
    }
    
    Vertex2f trans;
};


class ZoomCommand : public StringCommand
{
public:
    ZoomCommand() : zoom(1.0, 1.0) {}

    virtual Command* Create() { return new ZoomCommand(); }
    virtual CommandId GetId() { return ZOOM_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_MOTION_INPUT) {
            float num = (float) ((MouseMotionInput*)(&input))->vel.y;
            
            // clamp zoom
            if (num > MAX_ZOOM)  num = MAX_ZOOM;
            if (num < -MAX_ZOOM) num = -MAX_ZOOM;
            
            zoom.x = 1 + num / (MAX_ZOOM + 1);
            zoom.y = 1 + num / (MAX_ZOOM + 1);
        }
    }
    
    virtual const char *GetOptionName() { return "-zoom"; }
    virtual const char *GetName() { return "zoom"; }
    virtual const char *GetUsage() { return "factorX, factorY"; }
    virtual const char *GetDescription() { return "zoom view by a factor"; }
    virtual int Setup(int argc, const char **argv)
    {
        if (argc >= 3) {
            zoom.x = atof(argv[1]);
            zoom.y = atof(argv[2]);            
            return 3;
        }
        return -1;
    }
    
    enum { MAX_ZOOM = 20 };
    
    Vertex2f zoom;
};


class ZoomXCommand : public StringCommand
{
public:
    ZoomXCommand() : zoom(1.0, 1.0) {}

    virtual Command* Create() { return new ZoomXCommand(); }
    virtual CommandId GetId() { return ZOOM_X_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_MOTION_INPUT) {
            float num = (float) ((MouseMotionInput*)(&input))->vel.y;
            
            // clamp zoom
            if (num > MAX_ZOOM)  num = MAX_ZOOM;
            if (num < -MAX_ZOOM) num = -MAX_ZOOM;
            
            zoom.x = 1 + num / (MAX_ZOOM + 1);
            zoom.y = 1.0;
        }
    }
    
    virtual const char *GetOptionName() { return ""; }
    virtual const char *GetName() { return "zoomx"; }
    virtual const char *GetUsage() { return "factor"; }
    virtual const char *GetDescription() 
    { return "zoom x-axis by a factor"; }
    
    virtual int Setup(int argc, const char **argv)
    {
        if (argc >= 2) {
            zoom.x = atof(argv[1]);
            zoom.y =  1.0;
            return 2;
        }
        return -1;
    }
    
    enum { MAX_ZOOM = 20 };
    
    Vertex2f zoom;    
};

class ZoomYCommand : public StringCommand
{
public:
    ZoomYCommand() : zoom(1.0, 1.0) {}

    virtual Command* Create() { return new ZoomYCommand(); }
    virtual CommandId GetId() { return ZOOM_Y_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_MOTION_INPUT) {
            float num = (float) ((MouseMotionInput*)(&input))->vel.y;
            
            // clamp zoom
            if (num > MAX_ZOOM)  num = MAX_ZOOM;
            if (num < -MAX_ZOOM) num = -MAX_ZOOM;

            zoom.x = 1.0;            
            zoom.y = 1 + num / (MAX_ZOOM + 1);
        }
    }
    
    virtual const char *GetOptionName() { return ""; }
    virtual const char *GetName() { return "zoomy"; }
    virtual const char *GetUsage() { return "factor"; }
    virtual const char *GetDescription() 
    { return "zoom y-axis by a factor"; }

    
    virtual int Setup(int argc, const char **argv)
    {
        if (argc >= 2) {
            zoom.x =  1.0;       
            zoom.y = atof(argv[1]);
            return 2;
        }
        return -1;
    }
    
    enum { MAX_ZOOM = 20 };
    
    Vertex2f zoom;    
};


class FocusCommand : public StringCommand
{
public:
    FocusCommand() {}

    virtual Command* Create() { return new FocusCommand(); }
    virtual CommandId GetId() { return FOCUS_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_CLICK_INPUT) {
            focus = ((MouseClickInput*)&input)->pos;
        }
    }
    
    virtual const char *GetOptionName() { return "-focus"; }
    virtual const char *GetName() { return "focus"; }
    virtual const char *GetUsage() { return "x, y"; }
    virtual const char *GetDescription() { return "focus the view on (x,y)"; }
    virtual int Setup(int argc, const char **argv)
    {
        if (argc >= 3) {
            int x = atoi(argv[1]);
            int y = atoi(argv[2]);
            focus = Vertex2i(x, y);
            return 3;
        }
        return -1;
    }
    
    Vertex2i focus;
};

}


#endif
