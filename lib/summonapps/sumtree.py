from summon import *
import algorithms
import util
import sys

class Param:
    def __init__(self):
        self.distfactor = None

param = Param()


###########################################################################
# Drawing Code
#

def setupTree(node):
    if param.distfactor:
        if "dist" in dir(node):
            node.height = node.dist * param.distfactor
        else:
            node.height = param.distfactor
    else:
        node.height = 1
    
    for child in node.children:
        setupTree(child)

def printNode(node):
    if node.isLeaf():
        print node.name
    else:
        for child in node.children:
            printNode(child)


def colorMix(colors):
    sumcolor = [0, 0, 0]
    for c in colors:
        sumcolor[0] += c[0]
        sumcolor[1] += c[1]
        sumcolor[2] += c[2]                
    for i in range(3):
        sumcolor[i] /= float(len(colors))
    return sumcolor

def setColors(node):
    if node.isLeaf():
        if node.name.startswith("ENSG"): node.color = [1,0,0]
        elif node.name.startswith("ENSCAFG"): node.color = [1,1,0]
        elif node.name.startswith("ENSMUSG"): node.color = [0,0,1]
        elif node.name.startswith("ENSRNOG"): node.color = [0,1,0]
        else:
            node.color = [0,0,0]
    else:
        colors = []
        for child in node.children:
            setColors(child)
            colors.append(child.color)
        node.color = colorMix(colors)
        
def colorKey():
    print "color key"
    print "---------"
    print "human  red"
    print "dog yellow"
    print "mouse blue"
    print "rat green"
    

def drawNode(conf, node, sx, sy, height):
    vis = []
    vis.append(lines(apply(color,node.color), 
                     vertices(sx, sy, sx, sy - height)))

    # record node position in tree
    node.x = sx
    node.y = sy - node.height
    
    def func():
        print "----------------"
        printNode(node)
    
    vis.append(hotspot("click", 
                       sx - node.size/2.0, sy,
                       sx + node.size/2.0, sy - height,
                       func))
    
    # draw horizontal line
    if len(node.children) > 0:
        left = sx - node.size/2.0 + node.children[0].size/2.0
        right = sx + node.size/2.0 - node.children[-1].size/2.0

        vis.append(lines(apply(color,node.color),
                         vertices(left, sy - height, 
                                  right, sy - height)))
    return list2group(vis)


def drawTree(conf, node, sx=0, sy=0):
    vis = []

    # draw root of tree
    vis.append(drawNode(conf, node, sx, sy, node.height))

    # draw children
    if len(node.children) > 0:
        x = -node.size/2.0
        for child in node.children:
            x += child.size/2.0
            vis.append(drawTree(conf, child, 
                                sx+x, sy - node.height))
            x += child.size/2.0

    return list2group(vis)


###########################################################################
# Main Program Execution
#

sys.setrecursionlimit(100000)

# usage
if len(sys.argv) < 4:
    print "usage: vistree [-ptree <tree file> <label file>] [-newick <tree file>]"
    print "               [-usedist <dist factor>]"
    sys.exit(1)

# parse args
i = 2
while i < len(sys.argv):
    if sys.argv[i] == "-ptree":
        (mode, treeFile, labelFile) = sys.argv[2:2+3]
        i += 3
    elif sys.argv[i] == "-newick":
        mode, treeFile = sys.argv[2:2+2]
        i += 2
    elif sys.argv[i] == "-usedist":
        param.distfactor = float(sys.argv[i+1])
        i += 2
    else:
        raise "unknown argument '%s'" % sys.argv[2]

# title
print "SUMTREE (SUMMON Tree Visualizer)"
print "Matt Rasmussen 2005"
print

# read
util.tic("reading input")
tree = algorithms.Tree()
if mode == "-ptree":
    tree.readParentTree(treeFile, labelFile)
elif mode == "-newick":
    tree.readNewick(treeFile)
tree.setSizes()
setupTree(tree.root)
setColors(tree.root)

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

print tree.nodes.values()[-1].name

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
    
    
