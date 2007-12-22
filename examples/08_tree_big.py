#!/usr/bin/env python-i
# SUMMON examples
# 08_tree_big.py - recursive drawing of a BIG tree


import random

# make summon commands available
from summon.core import *
import summon

# create new window
win = summon.Window("08_tree_big")



# size of node toggle buttons
button_size = .25


class Node:
    """class for node in tree"""
    def __init__(self, grp):
        self.shown = True
        self.group = grp



class Tree:
    """a class for a tree"""
    children = []
    width = 1

    def __init__(self):
        self.children = []
        self.width = 1          # default width of a leaf
        


def MakeRandomTree(depth, chance, mult):
    tree = Tree()
    
    # recurse if depth less than curoff and with some probability
    if depth > 0 and chance > random.random():
        # add two children
        tree.children.append(MakeRandomTree(depth-1, chance*mult, mult))
        tree.children.append(MakeRandomTree(depth-1, chance*mult, mult))
        
        # make this node's width sum of children
        tree.width = tree.children[0].width + tree.children[1].width
    else:
        # define leaf
        tree.width = 1

    return tree        



def drawTree(tree, height):  
    """draw tree recursively"""
    
    width = tree.width
    
    # draw two children by recursing
    if len(tree.children) > 0:
        left  = -width/2.0 + tree.children[0].width/2.0
        right = width/2.0 - tree.children[1].width/2.0
           
        grp = group(
            # draw horizontal line
            lines(left, -height, right, -height),
    
            # left child
            translate(left, -height,
                drawTree(tree.children[0], height)),

            # right child
            translate(right, -height,
                drawTree(tree.children[1], height)))
        
        node = Node(grp)
        def func():
            node.shown = not node.shown
            win.show_group(node.group, node.shown)
        
        return group(
            color(0,0,0),
            lines(0, 0, 0, -height), grp,
            
            # draw toggle button
            color(1, 0, 0),
            translate(0, -height, 
                zoom_clamp(
                    quads(-button_size, -button_size,
                          -button_size, button_size,
                          button_size, button_size,
                          button_size, -button_size),
                    hotspot("click", 
                        -button_size, -button_size,
                        button_size, button_size,
                        func),
                    minx=5, miny=5, maxx=40, maxy=40, link=True, clip=True)))
    else:
        return group(color(0,0,0), lines(0, 0, 0, -height))    




# set background to white
win.set_bgcolor(1, 1, 1)

# make a random tree
tree = MakeRandomTree(15, 1, .96)

# draw the tree
win.add_group(group(color(0, 0, 0), drawTree(tree, 1)))

# center the "camera" so that all shapes are in view
win.home()



print
print "NOTE: use shift+right drag and ctrl+right drag to zoom"
print "x and y axis independently"
print
