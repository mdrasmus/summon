# SUMMON examples
# tree.py - recursive drawing of a tree

# make summon commands available
from summon import *


# clear the screen of all drawing
clear_groups()


# draw tree recursively
def drawTree(depth, width, height):
    # start a list of drawing elements that we will add to
    vis = []

    # draw vertical line
    vis.append(lines(vertices(0, 0, 0, -height)))
    
    # draw two children by recursing
    if depth > 0:
        # draw horizontal line
        vis.append(lines(vertices(-width/2.0, -height, width/2.0, -height)))
    
        # left child
        vis.append(
            translate(-width/2.0, -height   ,
                drawTree(depth-1, width/2.0, height)))


        # right child
        vis.append(
            translate(width/2.0, -height,
                drawTree(depth-1, width/2.0, height)))
    
    # convert draw list, vis, into a group and return
    return list2group(vis)

# set background to white
set_bgcolor(1,1,1)


# draw a black tree
add_group(group(color(0,0,0), drawTree(6, 100, 10)))

# center the "camera" so that all shapes are in view
home()
