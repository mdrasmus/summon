#!/usr/bin/python -i
# SUMMON examples
# 15_dup_window.py 
#
# illustrate use of multiple windows
#


# make summon commands available
from summon.core import *
import summon
import summon_core

# draw an arrow
def arrow():
    white = color(1,1,1)
    red  = color(1,0,0)
    blue = color(0,0,1)
    
    return group(
        quads(white, 0, 0,        # right fin
              red,   40, -40,
              red,   40, -80,
              white, 0, -40,
              
              white, 0,0,         # left fin
              blue,  -40, -40,
              blue,  -40, -80,
              white, 0, -40),
              
        lines(white, vertices(0,0, 0, 100))) # shaft
        

# execute a previous demo (creates a new window called 'win')
execfile("07_tree_toggle.py")


# create a second window that views the same model as the first window
win2 = win.duplicate()
win2.set_name("a duplicated window")
win2.set_size(400, 200)



# create a third window that views a different model
win3 = summon.Window("window 3")
win3.add_group(arrow())
win3.home()


print 
print "Note: use CTRL+d (while focused on a SUMMON window) to duplicate"
print "the window at any time."
print

