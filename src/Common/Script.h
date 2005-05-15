/***************************************************************************
* Vistools
* Matt Rasmussen
* Script.h
*
* Interface to internal scripting manipulation
*
***************************************************************************/


#if (VISTOOLS_SCRIPT == VISTOOLS_SCHEME)
#   include "Scheme/Script.h"
#endif
#if (VISTOOLS_SCRIPT == VISTOOLS_PYTHON)
#   include "Python/Script.h"
#endif
