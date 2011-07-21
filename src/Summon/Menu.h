/***************************************************************************
* Summon
* Matt Rasmussen
* SummonView.h
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

#ifndef SUMMON_MENU_H
#define SUMMON_MENU_H



// C++ headers
#include <vector>

// opengl headers
#include <GL/glut.h>


namespace Summon
{

using namespace std;


class Menu 
{
public:
    Menu(int menuid = 0)
        : menuid(menuid)
    {
    }

    ~Menu()
    {
        destroy();
    }

    int create(void (*func)(int))
    {
        menuid = glutCreateMenu(func);
        return menuid;
    }

    void destroy()
    {
        if (menuid != 0)
            glutDestroyMenu(menuid);
        menuid = 0;
    }

    void addEntry(const char *text, int value)
    {
        glutSetMenu(menuid);
        glutAddMenuEntry(text, value);
        items.push_back(value);
    }

    void addSubMenu(const char *text, int submenuid)
    {
        glutSetMenu(menuid);
        glutAddSubMenu(text, submenuid);
        items.push_back(-1);
    }
                
    void removeItem(int index)
    {
        glutSetMenu(menuid);
        glutRemoveMenuItem(index);
        items.erase(items.begin() + index - 1);
        // NOTE: items automatically slide down just like glut's behavior
    }

    void setEntry(int index, const char *text, int value)
    {
        glutSetMenu(menuid);
        glutChangeToMenuEntry(index, text, value);
        items[index] = value;
    }

    void setSubMenu(int index, const char *text, int submenuid)
    {
        glutSetMenu(menuid);
        glutChangeToSubMenu(index, text, submenuid);
        items[index] = -1;
    }


    int menuid;
    vector<int> items; // callback ids for each item
};


} // namespace Summon

#endif // SUMMON_MENU_H
