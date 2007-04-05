#!/usr/bin/env summon
# SUMMON examples
# tree_toggle.py - recursive drawing of a tree with node toggles

# make summon commands available
from summon import *


# clear the screen of all drawing
clear_groups()

button_size = 1

class Node:
    def __init__(self, gid):
        self.shown = True
        self.groupid = gid

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
        
        node = Node(get_group_id(grp))   
        def func():
            node.shown = not node.shown
            show_group(node.groupid, node.shown)
        
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
                    func)))
    else:
        return group(lines(0, 0, 0, -height))
    

# set background to white
set_bgcolor(1,1,1)


# draw a black tree
add_group(group(color(0,0,0), drawTree(6, 100, 10)))

# center the "camera" so that all shapes are in view
home()