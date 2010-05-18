#!/usr/bin/env python-i
# use summatrix to visualize a clustered dense matrix with trees
#

# load summon matrix library
import summon
from summon import matrix, util, treelib



# read in matrix (stored in Index Format 'imat')
mat = matrix.Matrix()
matrix.open_matrix("data.mat", mat, format="dmat")

# set colormap
mat.colormap = summon.PosNegColorMap()

# read row and column labels
mat.rowlabels = util.read_strings("data.rlabels")
mat.collabels = util.read_strings("data.clabels")

# read partition vectors for rows and columns of matrix
mat.rpart = util.read_strings("data.rpart")
mat.cpart = util.read_strings("data.cpart")

# read trees
# NOTE: trees will imply a permutation on the rows and columns
rtree = treelib.Tree()
rtree.read_parent_tree("data.row.ptree", labels=mat.rowlabels)
ctree = treelib.Tree()
ctree.read_parent_tree("data.col.ptree", labels=mat.collabels)



# create and show matrix viewer
viewer = matrix.MatrixViewer(mat, title="tree clustered data",
                             rtree=rtree,
                             ctree=ctree,
                             show_labels=True,
                             show_label_windows=True,
                             bgcolor=(1,1,1),
                             style="quads",
                             winsize=(400, 600))
viewer.show()

