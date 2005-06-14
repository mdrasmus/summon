import sys

print sys.path

import summonlib.shapes

sumtree = summonlib.tree

###########################################################################
# Main Program Execution
#

# usage
if len(sys.argv) < 4:
    print "usage: sumtree [-ptree <tree file> <label file>] [-newick <tree file>]"
    print "               [-usedist <dist factor>]"
    sys.exit(1)

# parse args
param = sumtree.Param(sys.argv[2:])

# title
print "SUMTREE (SUMMON Tree Visualizer)"
print "Matt Rasmussen 2005"
print

# read
tree = sumtree.readTree(param)

# draw
sumtree.display(param, tree)


