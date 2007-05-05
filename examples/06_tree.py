#!/usr/bin/python -i
# SUMMON examples
# 06_tree.py - recursive drawing of a tree

# make summon commands available
from summon.core import *
import summon


# create a new window
win = summon.Window("06_tree")



# draw tree recursively
def drawTree(depth, width, height):  
    # draw two children by recursing
    if depth > 0:        
        grp = group(
            # draw horizontal line
            lines(-width/2.0, -height, width/2.0, -height),
    
            # left child
            translate(-width/2.0, -height   ,
                drawTree(depth-1, width/2.0, height)),

            # right child
            translate(width/2.0, -height,
                drawTree(depth-1, width/2.0, height)))
        
        return group(lines(0, 0, 0, -height), grp)
    else:
        return group(lines(0, 0, 0, -height))
    

# set background to white
win.set_bgcolor(1,1,1)


# draw a black tree
win.add_group(group(color(0,0,0), drawTree(6, 100, 10)))

# center the "camera" so that all shapes are in view
win.home()
