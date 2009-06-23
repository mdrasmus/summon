#!/usr/bin/env python-i
# example of using summatrix from within python
# view a matrix in label format

print
print "NOTE: try quering matrix values using the following syntax:"
print "  mat['human']['Cow']"
print


# load summon matrix library
import summon
from summon import matrix

# read in matrix (stored in Labeled Format 'limat')
mat = matrix.Matrix()
mat.order = "mammals_distances.labels"  # set the labels needed for matrix
matrix.open_matrix("mammals_distances.lmat", mat, format="lmat")

# read color map for matrix
mat.colormap = summon.readColorMap("mammals_distances.colormap")

# create and show matrix viewer
viewer = matrix.MatrixViewer(mat, title="Mammals distances",
                             bgcolor=(1,1,1),  # use white background (r,g,b)
                             style="quads",    # use squares for matrix entries
                             showLabels=True,  # show labels
                             showLabelWindows=True) # use a separate window
                                                    # for labels
viewer.show()

