#!/usr/bin/env python-i
# example of using summatrix from within python

# load summon matrix library
from summon import matrix

# read in matrix (stored in Compressed Row Format 'rmat')
mat = matrix.Matrix()
matrix.open_matrix("dog_human.mat", mat, format="rmat")

# create and show matrix viewer
viewer = matrix.MatrixViewer(mat, title="dog vs human")
viewer.show()

