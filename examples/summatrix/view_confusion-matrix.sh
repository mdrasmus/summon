#!/bin/sh


summatrix \
    -d confusion-matrix.mat \
    --rclab confusion-matrix.label \
    --bgcolor 1,1,1 \
    --style quads $*
