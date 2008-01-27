/***************************************************************************
* Summon
* Matt Rasmussen
* StringCommand.h
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

#ifndef SUMMON_STRING_COMMAND_H
#define SUMMON_STRING_COMMAND_H

#include <set>
#include <string>
#include <vector>
#include "Command.h"



namespace Summon
{

using namespace std;

enum {
    COMMENT_COMMAND = 4000
};

class StringCommand : public Command
{
public:
    StringCommand() {}
    virtual ~StringCommand() {}

    virtual const char *GetOptionName() { return ""; }
    virtual const char *GetName() { return ""; }
    virtual const char *GetUsage() { return ""; }
    virtual const char *GetDescription() { return ""; }
    virtual const char *GetComment() { return ""; }
    virtual int Setup(int argc, const char **argv) { return 1; }
    
    virtual string ErrorHelp() {
        string msg = "usage: ";
        msg += GetName();
        msg += " ";
        msg += GetUsage();
        return msg;
    }
};



class CommentCommand : public StringCommand
{
public:
    CommentCommand(string str = "") : comment(str) {}

    virtual Command* Create() { return new CommentCommand(); }
    virtual int GetId() { return COMMENT_COMMAND; }
    virtual const char *GetComment() { return comment.c_str(); }
    
    string comment;
};



extern CommandAttr g_stringAttr;
vector<StringCommand*> GetAllStringCommands();
StringCommand *GetCommand(vector<StringCommand*> &cmds, 
                               int argc, char **argv, int *consume, 
                               bool option = false, bool showError = true);



extern vector<StringCommand*> g_usageList;
inline vector<StringCommand*> GetUsageList() { return g_usageList; }

#define RegisterStringCommand(cmd) \
    RegisterCommand(cmd) \
    AddAttr(g_stringAttr) \
    g_usageList.push_back(&cmd ## _c);

#define RegisterComment(var, str) \
    g_usageList.push_back(new CommentCommand(str));

}


#endif
