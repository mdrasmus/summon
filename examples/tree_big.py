# SUMMON examples
# tree_big.py - recursive drawing of a BIG tree

# make summon commands available
from summon import *
import random


# clear the screen of all drawing
clear_groups()

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
    

# draw a tree recursively
def drawTree(tree, height):
    # start a list of drawing elements that we will add to
    vis = []

    # let width be determined by node
    width = tree.width

    # draw vertical line
    vis.append(lines(0, 0, 0, -height))
    
    
    
    # draw two children by recursing
    if len(tree.children) > 0:
        left  = -width/2.0 + tree.children[0].width/2.0
        right = width/2.0 - tree.children[1].width/2.0
    
        # draw horizontal line
        vis.append(lines(left, -height, right, -height))
        
        # left child
        vis.append(
            translate(left, -height   ,
                drawTree(tree.children[0], height)))


        # right child
        vis.append(
            translate(right, -height,
                drawTree(tree.children[1], height)))
    
    # convert draw list, vis, into a group and return
    return list2group(vis)




# set background to white
set_bgcolor(1,1,1)

# make a random tree
tree = MakeRandomTree(30, 1, .96)

# draw the tree
add_group(group(color(0,0,0), drawTree(tree, 1)))

# center the "camera" so that all shapes are in view
home()
