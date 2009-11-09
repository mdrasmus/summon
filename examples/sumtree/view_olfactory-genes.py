#!/usr/bin/env python-i
# large phylogeny of mammalian olfactory genes
#


print """
NOTE: use Ctrl+right drag and Shift+right drag to zoom the 
      x- and y-axis independently
"""

# import necessary libraries
from summon import sumtree, util, treelib

# read tree in newick format
tree = treelib.read_tree("olfactory-genes.tree")

# read colormap
colormap = treelib.read_tree_color_map("ensembl.colormap")

# draw tree
vis = sumtree.SumTree(tree, name="dog and human gene tree", xscale=0, 
                      colormap=colormap)
vis.show()


