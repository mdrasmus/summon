#!/bin/sh
# use summatrix to visualize a dense matrix
#   rperm permutes the rows such that similar rows are placed next to each other
#   cperm permutes the columns
#   rpart gives a clusterid to each row, when two neighboring rows have 
#       different clusterids, a dividing line is drawn between them.
#   cpart works for columns like rpart for rows
#   showlabes=panels displays the labels in separate panel windows
#

summatrix \
    -d data.mat \
    --rlab data.rlabels \
    --clab data.clabels \
    -c posneg \
    --bgcolor 1,1,1 \
    --style quads \
    --cperm data.cperm \
    --rpart data.mat.clustering.6 \
    --cpart data.cpart \
    --rptree data.row.ptree \
    --winsize 400x600 \
    --showlabels panels $*
