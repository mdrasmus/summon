#ifndef COMMON_H
#define COMMON_H

#include <vector>

#define MAX_LINE 40000

namespace Summon {

enum {
    LOG1,
    LOG2,
    LOG3
};


void Log(int level, const char *fmt, ...);
void PushStage(const char *stage);
void PopStage();
bool Chomp(char *str);
std::vector<char*> Split(char *str, const char *delim, bool multiDelim = true);
bool InChars(char c, const char *chars);
char *Strtok(char *str, const char *delim, char **last, bool multiDelim = true);

}

#endif
