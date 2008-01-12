/***************************************************************************
* Summon
* Matt Rasmussen
* glutInputs.h
*
***************************************************************************/

#ifndef SUMMON_GLUT_INPUTS_H
#define SUMMON_GLUT_INPUTS_H

#include <string>
#include <GL/glut.h>
#include "Input.h"
#include "Vertex.h"

namespace Summon
{

enum {
    GLUT_INPUTS_BEGIN = 1,
    KEYBOARD_INPUT, 
    SPECIAL_KEY_INPUT,
    MOUSE_MOTION_INPUT,
    MOUSE_CLICK_INPUT,
    GLUT_INPUTS_END
};

void glutInputsInit();
    
class KeyboardInput : public Input
{
public:
    KeyboardInput() : key(0), mod(0) {}
    
    inline virtual Input* Create() { return new KeyboardInput(); }
    inline virtual InputId GetId() { return KEYBOARD_INPUT; }
    inline virtual int GetHash() { return int(key) | (mod << 8); }
    virtual const char *GetString() { 
        char str[2];
        str[0] = key;
        str[1] = '\0';
        
        string str2 = string("key ") + string(str);
        
        if (mod & GLUT_ACTIVE_SHIFT)
            str2 += "+shift ";
        if (mod & GLUT_ACTIVE_CTRL)
            str2 += "+ctrl ";
        if (mod & GLUT_ACTIVE_ALT)
            str2 += "+alt ";
        
        return str2.c_str();
    }
    
    unsigned char key;
    int mod;    
    Vertex2i pos;
};


class SpecialKeyInput : public Input
{
public:
    SpecialKeyInput() : key(0), mod(0) {}
    
    inline virtual Input* Create() { return new SpecialKeyInput(); }
    inline virtual InputId GetId() { return SPECIAL_KEY_INPUT; }
    inline virtual int GetHash() { return int(key) | (mod << 16); }
    
    int key;
    int mod;
    Vertex2i pos;
};


class MouseMotionInput : public Input
{
public:
    MouseMotionInput() : mod(0), button(0), state(0) {}
    
    inline virtual Input* Create() { return new MouseMotionInput(); }
    inline virtual InputId GetId() { return MOUSE_MOTION_INPUT; }
    inline virtual int GetHash() { 
        int hash = 0;
        
        switch (button) {
            case GLUT_LEFT_BUTTON:   hash += 0; break;
            case GLUT_MIDDLE_BUTTON: hash += 1; break;
            case GLUT_RIGHT_BUTTON:  hash += 2; break;
        }
        
        hash *= 2;
        
        switch (state) {
            case GLUT_UP:   hash += 0; break;
            case GLUT_DOWN: hash += 1; break;
        }
        
        hash = hash | (mod << 8);
        
        return hash;
    }
    
    virtual const char *GetString() { 
        string str = "drag ";
        
        switch (button) {
            case GLUT_LEFT_BUTTON:
                str += "left button";
                break;
            case GLUT_MIDDLE_BUTTON:
                str += "middle button";
                break;
            case GLUT_RIGHT_BUTTON:
                str += "right button";
                break;
        }
        
                
        if (mod & GLUT_ACTIVE_SHIFT)
            str += "+shift ";
        if (mod & GLUT_ACTIVE_CTRL)
            str += "+ctrl ";
        if (mod & GLUT_ACTIVE_ALT)
            str += "+alt ";
        
        return str.c_str();
    }

    int mod;
    int button;
    int state;
    Vertex2i pos;
    Vertex2i vel;
};


class MouseClickInput : public Input
{
public:
    MouseClickInput() : mod(0), button(0), state(0), drag(false) {}
    
    virtual Input* Create() { return new MouseClickInput(); }
    inline virtual InputId GetId() { return MOUSE_CLICK_INPUT; }
    inline virtual int GetHash() { 
        int hash = 0;
        
        switch (button) {
            case GLUT_LEFT_BUTTON:   hash |= 0x0; break;
            case GLUT_MIDDLE_BUTTON: hash |= 0x1; break;
            case GLUT_RIGHT_BUTTON:  hash |= 0x2; break;
        }
        
        switch (state) {
            case GLUT_UP:   hash |= 0x0; break;
            case GLUT_DOWN: hash |= 0x4; break;
        }
        
        if (drag)
            hash |= 0x8;
        else
            hash |= 0x0;
        
        hash = hash | (mod << 5);
        
        return hash;
    }
    
    virtual const char *GetString() { 
        string str = "click ";
        
        switch (button) {
            case GLUT_LEFT_BUTTON:
                str += "left button";
                break;
            case GLUT_MIDDLE_BUTTON:
                str += "middle button";
                break;
            case GLUT_RIGHT_BUTTON:
                str += "right button";
                break;
        }
        
        if (mod & GLUT_ACTIVE_SHIFT)
            str += "+shift ";
        if (mod & GLUT_ACTIVE_CTRL)
            str += "+ctrl ";
        if (mod & GLUT_ACTIVE_ALT)
            str += "+alt ";        
        
        return str.c_str();
    }
    
    int mod;
    int button;
    int state;
    bool drag;
    Vertex2i pos;
};


}

#endif

