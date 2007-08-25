#!/usr/bin/env python-i
# SUMMON examples
# 07_tree_toggle.py - recursive drawing of a tree with node toggles

# make summon commands available
from summon.core import *
import summon


# create a new window
win = summon.Window("07_tree_toggle")


button_size = 1

class Node:
    def __init__(self, grp):
        self.shown = True   # boolean whether this node is visible
        self.group = grp    # reference to graphical element,
                            # used to change its visibility


# draw tree recursively
def drawTree(depth, width, height):  
    # draw two children by recursing
    if depth > 0:        
        grp = group(
            # draw horizontal line
            lines(-width/2.0, -height, width/2.0, -height),
    
            # left child
            translate(-width/2.0, -height,
                drawTree(depth-1, width/2.0, height)),

            # right child
            translate(width/2.0, -height,
                drawTree(depth-1, width/2.0, height)))
        
        node = Node(grp)
        
        # callback function for when a tree node is clicked
        def onClickNode():
            node.shown = not node.shown
            win.show_group(node.group, node.shown)
        
        return group(
            color(0,0,0),
            lines(0, 0, 0, -height), grp,
            
            # draw toggle button
            color(1, 0, 0),
            translate(0, -height, 
                quads(-button_size, -button_size,
                      -button_size, button_size,
                      button_size, button_size,
                      button_size, -button_size),
                hotspot("click", 
                    -button_size, -button_size,
                    button_size, button_size,
                    onClickNode)))
    else:
        return lines(0, 0, 0, -height)
    

# set background to white
win.set_bgcolor(1,1,1)


# draw a black tree
win.add_group(group(color(0,0,0), drawTree(6, 100, 10)))

# center the "camera" so that all shapes are in view
win.home()


print "Note: Use middle click or Ctrl+click to toggle the visibility of "
print "      each node in the tree."
      
