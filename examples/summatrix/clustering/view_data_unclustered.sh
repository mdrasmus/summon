#!/bin/sh
# use summatrix to visualize a dense matrix
#

summatrix \
    -d data.mat \
    --rlab data.rlabels \
    --clab data.clabels \
    -c posneg \
    --bgcolor 1,1,1 \
    --style quads \
    --winsize 400x600 $*
