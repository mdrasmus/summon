import algorithms, util
import sys
from summon import *

sys.setrecursionlimit(1000)

class Param:
    def __init__(self, argv = [], parsers = []):
        self.distfactor = None
        self.mode = ""
        self.treeFile = ""
        self.labelFile = ""
        
        self.parse(argv, parsers)

    def parse(self, argv, parsers = []):
        parsers = parsers + [self.parseArg]
        
        i = 0
        while i < len(argv):
            j = 0
            consume = -1
            while j < len(parsers) and consume <= 0:
                consume = parsers[j](argv[i:])
                j += 1
            
            if consume == -1:
                raise "unknown argument '%s'" % argv[i]
            else:
                i += consume

    def parseArg(self, argv):
        if argv[0] == "-ptree":
            (self.mode, self.treeFile, self.labelFile) = argv[0:3]
            return 3
        elif argv[0] == "-newick":
            self.mode, self.treeFile = argv[0:2]
            return 2
        elif argv[0] == "-usedist":
            self.distfactor = float(argv[1])
            return 2
        else:
            return -1


def usage():
    print "usage: sumtree [-ptree <tree file> <label file>] [-newick <tree file>]"
    print "               [-usedist <dist factor>]"
    
    

###########################################################################
# Drawing Code
#

def nodeClick(node):
    print "----------------"
    printNode(node)

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
        if type(node.name) == str:
            if node.name.startswith("ENSG"): node.color = [1,0,0]
            elif node.name.startswith("ENSCAFG"): node.color = [1,1,0]
            elif node.name.startswith("ENSMUSG"): node.color = [0,0,1]
            elif node.name.startswith("ENSRNOG"): node.color = [0,1,0]
            else:
                node.color = [0,0,0]
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
    

def drawNode(param, node, sx, sy, height):
    vis = []
    vis.append(lines(apply(color,node.color), 
                     vertices(sx, sy, sx, sy - height)))

    # record node position in tree
    node.x = sx
    node.y = sy - node.height
    
    def func():
        globals()["selnode"] = node
        nodeClick(node)
    
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


def drawTree(param, node, sx=0, sy=0):
    vis = []

    # draw root of tree
    vis.append(drawNode(param, node, sx, sy, node.height))

    # draw children
    if len(node.children) > 0:
        try:
            x = -node.size/2.0
            for child in node.children:
                x += child.size/2.0
                vis.append(drawTree(param, child, 
                                sx+x, sy - node.height))
                x += child.size/2.0
        except RuntimeError:
            print sys.exc_type, ": Tree is too deep to draw"

    return list2group(vis)


def display(conf, tree):
    util.tic("drawing")
    
    set_bgcolor(1,1,1)
    add_group(group(
        color(0,0,0),
        drawTree(conf, tree.root)))    
    util.toc()


def find(tree, name):
    margin = 10
    
    if name in tree.nodes:
        x = tree.nodes[name].x
        y = tree.nodes[name].y
        print "found '%s' at (%f, %f)" % (name, x, y)
        set_visible(x-margin, y-margin, x+margin, y+margin)
    else:
        print "could not find '%s' in tree" % name

##########################################################################
# Reading and Initialization
#

def setupNode(param, node):
    if param.distfactor:
        if "dist" in dir(node):
            node.height = node.dist * param.distfactor
        else:
            node.height = param.distfactor
    else:
        node.height = 1
    
    for child in node.children:
        setupNode(param, child)

def setupTree(param, tree):
    tree.setSizes()
    setupNode(param, tree.root)
    setColors(tree.root)

def readTree(param):
    util.tic("reading input")
    
    tree = algorithms.Tree()
    
    if param.mode == "-ptree":
        tree.readParentTree(param.treeFile, param.labelFile)
    elif param.mode == "-newick":
        tree.readNewick(param.treeFile)
    
    print "%s: %d nodes, %d leaves\n" % \
        (param.treeFile, len(tree.nodes), len(tree.leaves()))
    
    # setup
    setupTree(param, tree)
    
    util.toc()    
    return tree
