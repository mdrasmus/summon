import algorithms, util
import sys
from summon import *

sys.setrecursionlimit(1000)

options = [
 ["p:", "ptree=", "ptree", "AUTO<ptree file>"],
 ["l:", "labels=", "labels", "AUTO<leaf labels file>"],
 ["n:", "newick=", "newick", "AUTO<newick file>"],
 ["t:", "usedist=", "usedist", "AUTO<distance factor>"]
]



def parseArgs(args, options2)
    try:
        param, rest = util.parseArgs(args, options + options2)
    except:
        sys.exit(1)


###########################################################################
# Drawing Code
#

class SumTree:
    def __init__(self):
        self.selnode = None
        self.tree = None
        self.param = {}
        

    def nodeClick(self, node):
        print "----------------"
        self.printNode(node)

    def printNode(self, node):
        if node.isLeaf():
            print node.name
        else:
            for child in node.children:
                self.printNode(child)


    def colorMix(self, colors):
        sumcolor = [0, 0, 0]
        for c in colors:
            sumcolor[0] += c[0]
            sumcolor[1] += c[1]
            sumcolor[2] += c[2]                
        for i in range(3):
            sumcolor[i] /= float(len(colors))
        return sumcolor
    
    
    def setColors(self, node):
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
                self.setColors(child)
                colors.append(child.color)
            node.color = self.colorMix(colors)

    
    def colorKey(self):
        print "color key"
        print "---------"
        print "human  red"
        print "dog    yellow"
        print "mouse  blue"
        print "rat    green"


    def drawNode(self, param, node, sx, sy, height):
        vis = []
        vis.append(lines(apply(color,node.color), 
                         vertices(sx, sy, sx, sy - height)))

        # record node position in tree
        node.x = sx
        node.y = sy - node.height

        def func():
            self.selnode = node
            self.nodeClick(node)

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


    def drawTree(self, param, node, sx=0, sy=0):
        vis = []

        # draw root of tree
        vis.append(self.drawNode(param, node, sx, sy, node.height))

        # draw children
        if len(node.children) > 0:
            try:
                x = -node.size/2.0
                for child in node.children:
                    x += child.size/2.0
                    vis.append(self.drawTree(param, child, 
                                             sx + x, sy - node.height))
                    x += child.size/2.0
            except RuntimeError:
                print sys.exc_type, ": Tree is too deep to draw"

        return list2group(vis)


    def display(self, param, tree):
        util.tic("drawing")

        set_bgcolor(1,1,1)
        add_group(group(
            color(0,0,0),
            self.drawTree(param, tree.root)))    
        util.toc()


    def find(self, name):
        margin = 10

        if name in self.tree.nodes:
            x = self.tree.nodes[name].x
            y = self.tree.nodes[name].y
            print "found '%s' at (%f, %f)" % (name, x, y)
            set_visible(x-margin, y-margin, x+margin, y+margin)
        else:
            print "could not find '%s' in tree" % name



    ##########################################################################
    # Reading and Initialization
    #

    def setupNode(self, param, node):
        if "usedist" in param:
            if "dist" in dir(node):
                node.height = node.dist * float(param["usedist"][-1])
            else:
                node.height = float(param["usedist"][-1])
        else:
            node.height = 1

        for child in node.children:
            self.setupNode(param, child)

    def setupTree(self, param, tree):
        tree.setSizes()
        self.setupNode(param, tree.root)
        self.setColors(tree.root)

    def readTree(self, param):
        util.tic("reading input")

        tree = algorithms.Tree()

        if "ptree" in param:
            tree.readParentTree(param["ptree"][-1], param["labels"][-1])
            print "%s: %d nodes, %d leaves\n" % \
                (param["ptree"][-1], len(tree.nodes), len(tree.leaves()))

        elif "newick" in param:
            tree.readNewick(param["newick"][-1])
            print "%s: %d nodes, %d leaves\n" % \
                (param["newick"][-1], len(tree.nodes), len(tree.leaves()))

        # setup
        self.setupTree(param, tree)
        self.tree = tree
        self.param = param


        util.toc()    
        return tree
