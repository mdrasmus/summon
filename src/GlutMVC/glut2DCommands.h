/***************************************************************************
* Vistools
* Matt Rasmussen
* glut2DCommands.h
*
***************************************************************************/

#ifndef GLUT_2D_COMMANDS_H
#define GLUT_2D_COMMANDS_H

#include "StringCommand.h"
#include "Factory.h"
#include "glutInputs.h"
#include "types.h"

namespace Vistools
{

void glutCommandsInit();

enum {
    GLUT_COMMANDS_BEGIN = 1000,
    REDISPLAY_COMMAND,
    TRANSLATE_COMMAND,
    ZOOM_COMMAND,
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
    virtual const char *GetUsage() { return "<x> <y>"; }
    virtual const char *GetDescription() 
    { return "translate the view by (x,y)"; }
    
    virtual int Setup(int argc, const char **argv)
    {
        if (argc >= 3) {
            float x = atof(argv[1]);
            float y = atof(argv[2]);
            trans = Vertex2f(x, y);
            return 3;
        }
        return -1;
    }
    
    Vertex2f trans;
};


class ZoomCommand : public StringCommand
{
public:
    ZoomCommand() : zoom(1.0) {}

    virtual Command* Create() { return new ZoomCommand(); }
    virtual CommandId GetId() { return ZOOM_COMMAND; }
    
    virtual void Setup(Input &input) 
    {
        if (input.GetId() == MOUSE_MOTION_INPUT) {
            float num = (float) ((MouseMotionInput*)(&input))->vel.y;
            
            // clamp zoom
            if (num > MAX_ZOOM)  num = MAX_ZOOM;
            if (num < -MAX_ZOOM) num = -MAX_ZOOM;
            
            zoom = 1 + num / (MAX_ZOOM + 1);
        }
    }
    
    virtual const char *GetOptionName() { return "-zoom"; }
    virtual const char *GetName() { return "zoom"; }
    virtual const char *GetUsage() { return "<factor>"; }
    virtual const char *GetDescription() { return "zoom view by a factor"; }
    virtual int Setup(int argc, const char **argv)
    {
        if (argc >= 2) {
            zoom = atof(argv[1]);
            return 2;
        }
        return -1;
    }
    
    enum { MAX_ZOOM = 20 };
    
    float zoom;
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
    virtual const char *GetUsage() { return "<x> <y>"; }
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
