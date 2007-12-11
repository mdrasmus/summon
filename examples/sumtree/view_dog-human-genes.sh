#!/bin/sh
# a very large hierarchical clustering of all human and dog genes
#

echo 
echo NOTE: use Ctrl+right drag and Shift+right drag to zoom the x- and y-axis independently
echo


sumtree \
    -p dog-human-genes.ptree \
    -l dog-human-genes.label \
    --vertical \
    --noshowlabels \
    --winsize 700x400 \
    -t 0 \
    -c ensembl.colormap $*
