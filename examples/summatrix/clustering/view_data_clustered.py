#!/usr/bin/env python-i
# use summatrix to visualize a clustered dense matrix 
#

# load summon matrix library
import summon
from summon import matrix, util



# read in matrix (stored in Index Format 'imat')
mat = matrix.Matrix()
matrix.open_matrix("data.mat", mat, format="dmat")

# set colormap
mat.colormap = summon.PosNegColorMap()

# read row and column labels
mat.rowlabels = util.read_strings("data.rlabels")
mat.collabels = util.read_strings("data.clabels")

# read permutation vectors for rows and columns of matrix
mat.rperm = util.read_ints("data.rperm")
mat.cperm = util.read_ints("data.cperm")

# Read partition vectors for rows and columns of matrix.
# Rows (columns) with the same partition ID belong to the same partition/cluster
mat.rpart = util.read_strings("data.rpart")
mat.cpart = util.read_strings("data.cpart")


# OPTIONAL
# If partition vectors are given, you omit the permutation vectors and
# setup a default permutation that sorts the rows (columns) such that rows
# (columns) of the same partition are consecutive.
#matrix.set_perm_from_part(mat)


# create and show matrix viewer
viewer = matrix.MatrixViewer(mat, title="unclustered data",
                             showLabels=True,
                             showLabelWindows=True,
                             bgcolor=(1,1,1),
                             style="quads",
                             winsize=(400, 600))
viewer.show()

