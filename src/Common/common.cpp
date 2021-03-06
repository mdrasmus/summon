/***************************************************************************
* Summon
* Matt Rasmussen
* common.cpp
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
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include "common.h"



namespace Summon {

using namespace std;

vector<string> g_logStages;
vector<clock_t> g_logTimes;


// define timing function
#if defined(SUMMON_MSW) || defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
    // windows
#   include <windows.h>
    
    long MsecTime()
    {
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        long tmpres = ft.dwLowDateTime / 10;
    }


#else
    // unix
#   include <sys/times.h>
    
    long MsecTime()
    {
	    struct timeval time;
	    gettimeofday(&time, NULL);
	    return time.tv_sec * 1000 + time.tv_usec / 1000;
    }
#endif




bool Chomp(char *str)
{
   int len = strlen(str);
   if (str[len-1] == '\n') {
      str[len-1] = '\0';
      return true;
   } else
      return false;
}

std::vector<char*> Split(char *str, const char *delim, bool multiDelim)
{
   std::vector<char*> tokens;
   char *last = NULL;
   char *token;
   
   while ((token = Strtok(str, delim, &last, multiDelim))) {
      tokens.push_back(token);
   }
   
   return tokens;
}

bool InChars(char c, const char *chars)
{
   if (!chars)
      return false;
   for (;*chars; chars++)
      if (c == *chars) return true;
   return false;
}

char *Strtok(char *str, const char *delim, char **last, bool multiDelim) 
{
   char *ptr, *token;   
   
   if (str == NULL)
      return NULL;
   
   if (*last == NULL)
      ptr = str;
   else
      ptr = *last;
   
   // Search past delims
   if (multiDelim)
      while (*ptr && InChars(*ptr, delim)) ptr++;
   
   if (!*ptr) return NULL;
   
   token = ptr;
   
   // Search past non-delims      
   while (*ptr && !InChars(*ptr, delim)) ptr++;
   
   // Are we at the end of the line?
   if (!*ptr)
      *last = ptr;         // Make sure we return NULL next time
   else
      *last = ptr + 1;     // Make sure we start after this char
   *ptr = '\0';
   
   return token;
}



void Log(int level, const char *fmt, ...)
{
   va_list ap;   
   va_start(ap, fmt);
   
   for (unsigned int i=0; i<g_logStages.size(); i++)
      fprintf(stderr, "  ");
   
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, "\n");
   
   va_end(ap);
}

void PushStage(const char *stage)
{
   for (unsigned int i=0; i<g_logStages.size(); i++)
      fprintf(stderr, "  ");
   
   fprintf(stderr, "STAGE BEGIN: %s\n", stage);
   g_logStages.push_back(string(stage));
   
   // timing
   //struct tms buf;
   //times(&buf);
   //g_logTimes.push_back(buf.tms_utime);
}

void PopStage()
{
   for (unsigned int i=0; i<g_logStages.size()-1; i++)
      fprintf(stderr, "  ");
   
   //struct tms buf;
   //times(&buf);
   //float elapse = float(buf.tms_utime - g_logTimes.back()) / sysconf(_SC_CLK_TCK);
   //   
   //fprintf(stderr, "STAGE END:   %s [%.3f s]\n", 
   //       g_logStages.back().c_str(), elapse);
   g_logStages.pop_back();
   g_logTimes.pop_back();
}


}
