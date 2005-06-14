from summon import *
import algorithms, util
import sys

sys.setrecursionlimit(100000)

class Param:
    def __init__(self, argv = []):
        self.distfactor = None
        self.mode = ""
        self.treeFile = ""
        self.labelFile = ""
        
        self.parse(argv)
        

    def parse(self, argv):
        i = 0
        while i < len(argv):
            if argv[i] == "-ptree":
                (self.mode, self.treeFile, self.labelFile) = argv[i:i+3]
                i += 3
            elif argv[i] == "-newick":
                self.mode, self.treeFile = argv[i:i+2]
                i += 2
            elif argv[i] == "-usedist":
                self.distfactor = float(argv[i+1])
                i += 2
            else:
                raise "unknown argument '%s'" % argv[i]


###########################################################################
# Drawing Code
#

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
    

def drawNode(param, node, sx, sy, height):
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


def drawTree(param, node, sx=0, sy=0):
    vis = []

    # draw root of tree
    vis.append(drawNode(param, node, sx, sy, node.height))

    # draw children
    if len(node.children) > 0:
        x = -node.size/2.0
        for child in node.children:
            x += child.size/2.0
            vis.append(drawTree(param, child, 
                                sx+x, sy - node.height))
            x += child.size/2.0

    return list2group(vis)


def display(conf, tree):
    util.tic("drawing")
    
    set_bgcolor(1,1,1)
    add_group(group(
        color(0,0,0),
        drawTree(conf, tree.root)))
    home()
    util.toc()
    

##########################################################################
# Reading and Initialization
#

def setupTree(param, node):
    if param.distfactor:
        if "dist" in dir(node):
            node.height = node.dist * param.distfactor
        else:
            node.height = param.distfactor
    else:
        node.height = 1
    
    for child in node.children:
        setupTree(param, child)


def readTree(param):
    util.tic("reading input")
    
    tree = algorithms.Tree()
    
    # choose format
    if param.mode == "-ptree":
        tree.readParentTree(param.treeFile, param.labelFile)
    elif param.mode == "-newick":
        tree.readNewick(param.treeFile)
    
    # setup
    tree.setSizes()
    setupTree(param, tree.root)
    setColors(tree.root)
    
    util.toc()    
    return tree
