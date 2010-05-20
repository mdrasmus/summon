#!/usr/bin/env python-i

import summon
from summon.core import *

#
# test for whether a non referenced element is GC when one of its children
# has a pointer
#


win = summon.Window("remove")
h = win.add_group(group())

g = group(lines(0, 0, 100, 100))

print g.get_parent()

for i in range(100):
    h.append(group(g))
    
    print g.get_parent()
    h.clear()

    if g.get_parent():
        g.remove_self()

h.append(g)


#=============================================================================
# test replace

h.clear()
g = group(group(lines(0,0,100,100)))
h.append(g)

h.replace(g, group(lines(100, 0, 0, 100)))
del g

