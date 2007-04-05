
import sys
from summon.core import *
from summon import shapes
from summon import algorithms
from summon import util



sys.setrecursionlimit(2000)

options = [
 ["p:", "ptree=", "ptree", "AUTO<ptree file>"],
 ["l:", "labels=", "labels", "AUTO<leaf labels file>"],
 ["n:", "newick=", "newick", "AUTO<newick file>"],
 ["t:", "usedist=", "usedist", "AUTO<distance factor>"],
 ["L", "showlabels", "showlabels", "AUTO"],
 ["H", "horizontal", "horizontal", "AUTO"]
]


###########################################################################
# Drawing Code
#

class SumTree:
    def __init__(self):
        self.selnode = None
        self.tree = None
        self.param = {}
        self.markGroup = None
        self.labelids = []
        self.showlabels = False

    
    def showLabels(self, visible):
        for label in self.labelids:
            show_group(label, visible)
    
    
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
        
        if self.showLabels and node.isLeaf() and type(node.name) == str:
            if "horizontal" in self.param:
                label = group(
                    translate(sx - node.size/2.0, sy - height,
                        rotate(-90,
                            color(0,0,0),
                            text_clip(node.name, 0, node.size*.1, 
                                      node.size*100, node.size*.9, 5, 12,
                                      "left", "middle"))))
            else:
                label = group(
                    translate(sx - node.size/2.0, sy - height,
                        rotate(-90,
                            color(0,0,0),
                            text_clip(node.name, 0, node.size*.1, 
                                      node.size*100, node.size*.9, 5, 12,
                                      "left", "middle", "vertical"))))
        
            self.labelids.append(get_group_id(label))
            vis.append(label)
            

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
        if "horizontal" in param:
            add_group(group(
                color(0,0,0),
                rotate(90, 
                    self.drawTree(param, tree.root))))
        else:
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
    
    
    def markNodes(self, names, boxColor = color(1, 0, 0)):
        if self.markGroup == None:
            self.markGroup = add_group(group())
        
        vis = [boxColor]
        found = 0
        
        for name in names:
            if name in self.tree.nodes:
                found += 1
                node = self.tree.nodes[name]
                vis.append(points(node.x, node.y))
            else:        
                print "could not find '%s' in tree" % name
                
        print "found %d of %d nodes" % (found, len(names))
        
        insert_group(self.markGroup, group(* vis))
    
    
    def flagNodes(self, names, flagColor = color(1,0,0)):
        if self.markGroup == None:
            self.markGroup = add_group(group())
        
        vis = [flagColor]
        found = 0
        
        bottom = self.tree.root.y
        for node in self.tree.nodes.values():
            bottom = min(bottom, node.y)
        bottom = bottom * 2
        
        for name in names:
            if name in self.tree.nodes:
                found += 1
                node = self.tree.nodes[name]
                vis.append(lines(node.x, node.y, node.x, bottom))
            else:        
                print "could not find '%s' in tree" % name
        
        print "found %d of %d nodes" % (found, len(names))
            
        insert_group(self.markGroup, group(* vis))
    
    
    def clearMarks(self):
        if self.markGroup != None:
            remove_group(self.markGroup)
            self.markGroup = None
    
    
    def mark(self, boxColor = color(1,0,0)):
        names = []
        while True:
            line = sys.stdin.readline().rstrip()
            if line == "": break
            names.append(line)
        self.markNodes(names, boxColor)
    
    
    def flag(self, flagColor = color(1,0,0)):
        names = []
        while True:
            line = sys.stdin.readline().rstrip()
            if line == "": break
            names.append(line)
        self.flagNodes(names, flagColor)



    ##########################################################################
    # Reading and Initialization
    #

    def setupNode(self, param, node):
        if "usedist" in param:
            node.height = node.dist * float(param["usedist"][-1])
        else:
            node.height = 1

        for child in node.children:
            self.setupNode(param, child)

    def setupTree(self, param, tree):
        tree.setSizes()
        self.setupNode(param, tree.root)
        self.setColors(tree.root)
        self.tree = tree
        self.param = param
        
        self.showLabels = "showlabels" in param

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
        
        util.toc()    
        return tree
