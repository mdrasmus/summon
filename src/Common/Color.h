/***************************************************************************
* Vistools
* Matt Rasmussen
* Color.h
*
***************************************************************************/

#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <string>
#include "common.h"

namespace Vistools
{

using namespace std;

class Color {
public:
    Color(float red = 1, float green = 1, float blue = 1, float alpha = 1) :
        r(red), g(green), b(blue), a(alpha)
    {}
    
    Color(string str) {
        Read(str);
    }
    
    inline void Set(float red = 1, float green = 1, float blue = 1, float alpha = 1)
    {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }
    
    bool Read(string str) {
        if (str == "red")
            Set(1,0,0);
        else if (str == "orange")
            Set(1,.5,0);
        else if (str == "yellow")
            Set(1,1,0);
        else if (str == "green")
            Set(0,1,0);
        else if (str == "blue")
            Set(0,0,1);
        else if (str == "violet")
            Set(1,0,1);
        else if (str == "white")
            Set(1,1,1);
        else if (str == "black")
            Set(0,0,0);
        else if (str.substr(0, 2) == "0x") {
            string rstr = str.substr(2,2);
            string gstr = str.substr(4,2);
            string bstr = str.substr(6,2);
            string astr = str.substr(8,2);
            int red=0, green=0, blue=0, alpha=1;
            float d = 255;            
            
            if (rstr.length() > 0) sscanf(rstr.c_str(), "%x", &red);
            if (gstr.length() > 0) sscanf(gstr.c_str(), "%x", &green);
            if (bstr.length() > 0) sscanf(bstr.c_str(), "%x", &blue);
            if (astr.length() > 0) sscanf(astr.c_str(), "%x", &alpha);

            
            Set(red/d, green/d, blue/d, alpha/d);
        } else {
            Error("Unknown color '%s'", str.c_str());
            return false;
        }
        return true;
    }

    float r;
    float g;
    float b;
    float a;
};



}

#endif
