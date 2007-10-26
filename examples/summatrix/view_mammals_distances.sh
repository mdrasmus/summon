#!/bin/sh
# view a matrix in label format

echo
echo NOTE: try quering matrix values using the following syntax:
echo   "mat['human']['Cow']"
echo

summatrix \
    -l mammals_distances.lmat \
    -c rainbow \
    -o mammals_distances.labels \
    --bgcolor 1,1,1 \
    --style quads \
    --showlabels panels \
    -c mammals_distances.colormap \
    $*
