/**************************************************************************
* Summon
* Matt Rasmussen
* common.h
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


#ifndef SUMMON_COMMON_H
#define SUMMON_COMMON_H

#include <vector>


#define MAX_LINE 40000

const float FLOAT_MIN = -1e307;
const float FLOAT_MAX = 1e307;

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
