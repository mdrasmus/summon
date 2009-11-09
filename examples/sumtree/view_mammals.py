#!/usr/bin/env python-i
# a phylogenetic tree of mammal species
#


print """
NOTE: use Ctrl+right drag and Shift+right drag to zoom the 
      x- and y-axis independently
"""

# import necessary libraries
from summon import sumtree, treelib

# read tree in newick format
tree = treelib.read_tree("mammals.tree")

# draw tree
vis = sumtree.SumTree(tree, name="dog and human gene tree", xscale=40, 
                      winsize=(600, 400))
vis.show()

