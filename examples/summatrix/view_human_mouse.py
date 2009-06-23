#!/usr/bin/env python-i
# example of using summatrix from within python

# load summon matrix library
from summon import matrix

# read in matrix (stored in Index Format 'imat')
mat = matrix.Matrix()
matrix.open_matrix("human_mouse.imat", mat, format="imat")

# create and show matrix viewer
viewer = matrix.MatrixViewer(mat, title="human vs mouse")
viewer.show()

