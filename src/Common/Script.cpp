/***************************************************************************
* Vistools
* Matt Rasmussen
* Script.cpp
*
* Wrappers for Scheme library (guile)
*
***************************************************************************/

#if (VISTOOLS_SCRIPT == VISTOOLS_SCHEME)
#   include "Scheme/Script.cpp"
#endif
#if (VISTOOLS_SCRIPT == VISTOOLS_PYTHON)
#   include "Python/Script.cpp"
#endif
