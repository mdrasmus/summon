from summon import *
import algorithms
import util
import sys



###########################################################################
# Drawing Code
#

def printNode(node):
    if node.isLeaf():
        print node.name
    else:
        for child in node.children:
            printNode(child)


def drawNode(conf, node, sx, sy):
    vis = []
    vis.append(lines(vertices(sx, sy, sx, sy-conf['height'])))

    # record node position in tree
    node.x = sx
    node.y = sy - conf['height']
    
    def func():
        print "----------------"
        printNode(node)
    
    vis.append(hotspot("click", 
                       sx - node.size/2.0, sy,
                       sx + node.size/2.0, sy-conf['height'],
                       func))
    
    # draw horizontal line
    if len(node.children) > 0:
        left = sx - node.size/2.0 + node.children[0].size/2.0
        right = sx + node.size/2.0 - node.children[-1].size/2.0

        vis.append(lines(vertices(left, sy - conf['height'], 
                                  right, sy - conf['height'])))
    return list2group(vis)


def drawTree(conf, node, sx=0, sy=0):
    vis = []

    # draw root of tree
    vis.append(drawNode(conf, node, sx, sy))

    # draw children
    if len(node.children) > 0:
        x = -node.size/2.0
        for child in node.children:
            x += child.size/2.0
            vis.append(drawTree(conf, child, 
                                sx+x, sy-conf['height']))
            x += child.size/2.0

    return list2group(vis)


###########################################################################
# Main Program Execution
#

sys.setrecursionlimit(100000)

# usage
if len(sys.argv) < 4:
    print "usage: vistree <tree file> <label file>"
    sys.exit(1)

# parse args
(treeFile, labelFile) = sys.argv[2:2+2]

# title
print "SUMTREE (SUMMON Tree Visualizer)"
print "Matt Rasmussen 2005"
print

# read
util.tic("reading input")
tree = algorithms.Tree()
tree.readParentTree(treeFile, labelFile)
tree.setSizes()
conf = {'height' : 1}
util.toc()

# draw
util.tic("drawing")
set_bgcolor(1,1,1)
add_group(group(
    color(0,0,0),
    drawTree(conf, tree.root)))
home()
util.toc()



#########################################################################
# Functions to manipulate the Tree
#

def getLeaves(node = tree.root):
    def walk(node, lst):
        if node.isLeaf():
            lst.append(node)
        else:
            for child in node.children:
                walk(child, lst)
    
    lst = []
    walk(node, lst)
    return lst

def writeLeaves(out = sys.stdout, node = tree.root):
    nodes = getLeaves()
    labels = map(lambda x: x.name, nodes)
    for l in labels:
        print >>out, l

def getLeavesOrder():
    nodes = getLeaves()
    names = map(lambda x: x.name, nodes)
    names = filter(lambda x: type(x) == str, names)
    
    labels = util.readStrings(labelFile)
    lookup = util.list2lookup(labels)
    perm = map(lambda x: lookup[x], names)
    return perm

def writeLeavesOrder(out = sys.stdout):
    perm = getLeavesOrder()
    for i in perm:
        print >>out, i
    
    
