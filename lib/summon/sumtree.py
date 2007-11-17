#
# phylogenetic tree visualization
#
#

import sys
import math

from summon.core import *
from summon import shapes
from summon import treelib
from summon import util
import summon





#=============================================================================
# Drawing Code
#

class SumTree (object):
    """SUMMON Tree Visualizer"""

    def __init__(self, tree, name="SUMTREE",
                       showLabels=True, xscale=1.0,
                       vertical=False,
                       winsize=(400, 400),
                       winpos=None,
                       colormap=None):
        self.tree = tree   
        
        self.win = None
        self.winsize = winsize
        self.winpos = winpos
        self.name = name 
        self.selnode = None
        self.markGroup = None
        self.labels = []
        self.showLabels = showLabels
        self.xscale = float(xscale)
        self.win = None
        self.vertical = vertical
        self.winsize = winsize
        self.colormap = colormap
        
        self.setupTree(self.tree)
        
    
    def setTree(self, tree):
        self.tree = tree
        
        
    def setupTree(self, tree):
        sizes = treelib.countDescendents(tree.root)
        for node, size in sizes.iteritems():
            node.size = size
        
        self.setupNode(tree.root)
        self.setColors(tree)
    
    
    def setupNode(self, node):
        if self.xscale == 0:
            node.height = 1
        else:
            node.height = node.dist * self.xscale
        

        for child in node.children:
            self.setupNode(child)
    
    
    def show(self):
        """shows visualization window"""
        
        self.setupTree(self.tree)
        
        if self.win == None:
            self.win = summon.Window(self.name, size=self.winsize,
                                     position=self.winpos)
            newwin = True
        else:
            self.win.clear_groups()
            newwin = False
        self.win.set_bgcolor(1,1,1)
        
        if self.vertical:
            # draw vertical tree
            self.win.add_group(group(
                color(0,0,0),
                rotate(-90,                 
                    self.drawTree(self.tree.root))))
                
        else:        
            # draw horizontal tree
            self.win.add_group(group(
                    color(0,0,0),
                    self.drawTree(self.tree.root)))

        
        
        # draw branch length legend
        if not self.vertical and self.xscale != 0:
            maxwidth = max(node.x for node in self.tree.nodes.itervalues())
            
            # if tree has no lengths, do not draw legend
            if maxwidth > 0:            
                # automatically choose a scale
                length = maxwidth / self.xscale
                order = math.floor(math.log10(length))
                length = 10 ** order
    
                self.win.add_group(self.drawScale(0, -self.tree.root.size/2.0 - 2 , 
                                                  length, self.xscale))
        

        # put tree into view
        if newwin:
            w, h = self.win.get_size()        
            self.win.home()
            self.win.focus(w/2, h/2)
            self.win.zoom(.9, .9)
    

    
    
    #======================================================================
    # graphics functions
    #
    
    def drawTree(self, node, sx=0, sy=0):
        vis = []

        # draw root of tree
        vis.append(self.drawNode(node, sx, sy, node.height))

        # draw children
        if len(node.children) > 0:
            try:
                y = -node.size/2.0
                for child in node.children:
                    y += child.size/2.0
                    vis.append(self.drawTree(child, sx + node.height, sy - y))
                    y += child.size/2.0
            except RuntimeError:
                print sys.exc_type, ": Tree is too deep to draw"

        return group(*vis) 
    
    
    def drawScale(self, x, y, length, xscale):
        
        
        return group(color(0, 0, 1), lines(x, y, x + xscale*length, y),
                     text_clip("%.2f" % length, x + xscale*length, y-2, 
                                          x+10*xscale*length, y+2,
                                          3, 12,
                                "left", "middle"))
    
    
    def drawNode(self, node, sx, sy, height):
        vis = []
        vis.append(lines(color(*node.color), 
                         sx, sy, sx + height, sy))

        # record node position in tree
        node.x = sx + node.height
        node.y = sy

        def func():
            self.selnode = node
            self.nodeClick(node)

        vis.append(hotspot("click", 
                           sx, sy - node.size/2.0,
                           sx + height, sy + node.size/2.0,
                           func))

        
        if self.showLabels and node.isLeaf() and type(node.name) == str:
            if self.vertical:
                label = group(
                    translate(sx + height, sy - node.size/2.0,
                            color(0,0,0),
                            text_clip(node.name, 0, node.size*.1, 
                                      node.size*1000, node.size*.9, 5, 12,
                                      "left", "middle", "vertical")))
            else:
                label = group(
                    translate(sx + height, sy - node.size/2.0,
                            color(0,0,0),
                            text_clip(node.name, 0, node.size*.1, 
                                      node.size*1000, node.size*.9, 5, 12,
                                      "left", "middle")))
        
            self.labels.append(label)
            vis.append(label)
            
        
        # draw vertical line
        if len(node.children) > 0:
            bottom = sy - node.size/2.0 + node.children[-1].size/2.0
            top = sy + node.size/2.0 - node.children[0].size/2.0
            
            vis.append(lines(color(*node.color),
                             sx + height, bottom,
                             sx + height, top))
        return group(*vis)


   
    
    def enableLabels(self, visible):
        for label in self.labels:
            self.win.show_group(label, visible)
    
    
    def nodeClick(self, node):
        print "-" * 20
        self.printNode(node)
    
    
    def printNode(self, node):
        if node.isLeaf():
            print node.name
        else:
            for child in node.children:
                self.printNode(child)

    
    def setColors(self, tree):
        if self.colormap != None:
            self.colormap(tree)
        else:
            for node in tree:
                node.color = (0, 0, 0)

    
    #======================================================================
    # interactive functions

    def find(self, name):
        margin = 10

        if name in self.tree.nodes:
            x = self.tree.nodes[name].x
            y = self.tree.nodes[name].y
            print "found '%s' at (%f, %f)" % (name, x, y)
            self.win.set_visible(x-margin, y-margin, x+margin, y+margin)
        else:
            print "could not find '%s' in tree" % name
    
    
    def mark(self, names, boxColor = color(1, 0, 0)):
        if self.markGroup == None:
            self.markGroup = self.win.add_group(group())
        
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
        
        self.win.insert_group(self.markGroup, group(* vis))
    
    
    def flag(self, names, flagColor = color(1,0,0)):
        if self.markGroup == None:
            self.markGroup = self.win.add_group(group())
        
        vis = [flagColor]
        found = 0
        
        far = self.tree.root.x
        for node in self.tree.nodes.values():
            far = max(far, node.x)
        far = far * 2
        
        for name in names:
            if name in self.tree.nodes:
                found += 1
                node = self.tree.nodes[name]
                vis.append(lines(node.x, node.y, far, node.y))
            else:        
                print "could not find '%s' in tree" % name
        
        print "found %d of %d nodes" % (found, len(names))
            
        self.win.insert_group(self.markGroup, group(* vis))
    
    
    def clearMarks(self):
        if self.markGroup != None:
            self.win.remove_group(self.markGroup)
            self.markGroup = None
    

