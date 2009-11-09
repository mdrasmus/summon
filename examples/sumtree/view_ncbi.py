#!/usr/bin/env python-i
# a very large hierarchical clustering of all human and dog genes
#

print """
NOTE: use Ctrl+right drag and Shift+right drag to zoom the 
      x- and y-axis independently

WARNING: this is a very large tree (~260,000 leaves).  Allow time for loading.

"""

import zipfile

# import necessary libraries
from summon import sumtree, util, treelib

# read tree in parent tree format
util.tic("read tree")
tree = treelib.parse_newick(zipfile.ZipFile("ncbi-taxonomy.zip").read(
    "ncbi-taxonomy.tre"))
util.toc()

# draw tree
util.tic("draw tree")
vis = sumtree.SumTree(tree, name="NCBI taxonomy tree", xscale=0, 
                      winsize=(400, 700))
vis.show()
util.toc()

vis.flag(["Homo_sapiens"], (1, 0, 0))
vis.flag(["Mus_musculus"], (0, 0, 1))
