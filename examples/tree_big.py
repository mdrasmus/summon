#!/usr/bin/env summon
# SUMMON examples
# tree_big.py - recursive drawing of a BIG tree

# make summon commands available
from summon import *
import random


# clear the screen of all drawing
clear_groups()

# size of node toggle buttons
button_size = .25

# class for node in tree
class Node:
    def __init__(self, gid):
        self.shown = True
        self.groupid = gid


# a class for a tree
class Tree:
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


# draw tree recursively
def drawTree(tree, height):  
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
        return group(color(0,0,0), lines(0, 0, 0, -height))    




# set background to white
set_bgcolor(1,1,1)

# make a random tree
tree = MakeRandomTree(30, 1, .96)

# draw the tree
add_group(group(color(0,0,0), drawTree(tree, 1)))

# center the "camera" so that all shapes are in view
home()
