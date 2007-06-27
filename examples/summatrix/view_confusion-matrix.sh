#!/bin/sh
# use summatrix to visualize a dense matrix
#

summatrix \
    -d confusion-matrix.mat \
    --rclab confusion-matrix.label \
    --bgcolor 1,1,1 \
    --style quads $*
