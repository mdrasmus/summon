#!/bin/sh
# a phylogenetic tree of mammal species
#


sumtree \
    -n mammals.tree \
    -t 40 \
    --winsize 600x400 \
    $*
