#!/usr/bin/env python-i
# a very large hierarchical clustering of all human and dog genes
#

print """
NOTE: use Ctrl+right drag and Shift+right drag to zoom the 
      x- and y-axis independently"
"""

from summon import sumtree, util, treelib

# read tree in parent tree format
util.tic("read tree")
tree = treelib.Tree()
tree.read_parent_tree("dog-human-genes.ptree", "dog-human-genes.label")
util.toc()

# read colormap
colormap = treelib.read_tree_color_map("dog-human.colormap")


# Provide a customization of the node labels.
# To view node labels, press "L"
def nodelabel(node):
    if isinstance(node.name, str):
        if node.name.startswith("ENS"):
            return ""
    return node.name


# draw tree
util.tic("draw tree")
vis = sumtree.SumTree(tree, name="dog and human gene tree", xscale=0, 
                      showLabels=False,
                      vertical=True,
                      colormap=colormap,
                      nodelabel=nodelabel,
                      winsize=(700, 400))
vis.show()
util.toc()

