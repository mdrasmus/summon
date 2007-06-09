


#include "first.h"
//#include <sys/times.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include "common.h"

namespace Summon {

using namespace std;

vector<string> g_logStages;
vector<clock_t> g_logTimes;


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

void Error(const char *fmt, ...)
{
   va_list ap;   
   va_start(ap, fmt);
   
   fprintf(stderr, "error: ");
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, "\n");
   
   va_end(ap);
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
