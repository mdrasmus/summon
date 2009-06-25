#!/usr/bin/env python-i
# use summatrix to visualize a dense matrix
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


# create and show matrix viewer
viewer = matrix.MatrixViewer(mat, title="unclustered data",
                             showLabels=True,
                             bgcolor=(1,1,1),
                             style="quads",
                             winsize=(400, 600))
viewer.show()

