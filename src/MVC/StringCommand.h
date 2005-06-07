/***************************************************************************
* Vistools
* Matt Rasmussen
* StringCommand.h
*
***************************************************************************/

#ifndef STRING_COMMAND_H
#define STRING_COMMAND_H

#include <set>
#include <string>
#include <vector>
#include "Command.h"


namespace Vistools
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
