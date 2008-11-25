"""

 Summon - tree visualization

"""

import sys
import math
import copy

from summon.core import *
from summon import shapes
from summon import treelib
from summon import util
import summon


# TODO: customize scale color
# TODO: customize bgcolor, auto choose branch color based on bgcolor
# TODO: switch to python naming scheme word_and_word


#=============================================================================
# Drawing Code
#

class SumTreeMenu (summon.SummonMenu):
    """summatrix popup menu"""
    
    def __init__(self, viewer):
        summon.SummonMenu.__init__(self, viewer.win)
        
        self.viewer = viewer
        
        self.sumtree_menu = summon.Menu()
        self.sumtree_menu.add_entry("toggle labels (l)", viewer.toggleLabels)
        self.insert_submenu(0, "Sumtree", self.sumtree_menu)
        

class SumTree (object):
    """SUMMON Tree Visualizer"""

    def __init__(self, tree, name="SUMTREE",
                       showLabels=True, 
                       xscale=1.0,
                       showBranchLabels=True,
                       showBoot=True,
                       branchLabels={},
                       autozoom=True,
                       vertical=False,
                       winsize=(400, 400),
                       winpos=None,
                       colormap=None,
                       layout=None):
        self.tree = tree   
        
        self.win = None
        self.winsize = winsize
        self.winpos = winpos
        self.name = name 
        self.selnode = None
        self.markGroup = None
        self.labels = []
        self.showBoot = showBoot
        self.showLabels = showLabels
        self.showBranchLabels = showBranchLabels
        self.branchLabels = copy.copy(branchLabels)
        self.xscale = float(xscale)
        self.win = None
        self.vertical = vertical
        self.winsize = winsize
        self.colormap = colormap
        self.layout = layout
        self.autozoom = autozoom
        
        self._setupTree()
        
    
    def setTree(self, tree, layout=None, branchLabels=None):
        """Changes the tree in the visualization"""
        self.tree = tree
        self.layout = layout
        
        if branchLabels != None:
            self.branchLabels = copy.copy(branchLabels)
        else:
            self.branchLabels = {}
        
        self._setupTree()
        
        
    def _setupTree(self):
        """Private function for configuring a new tree"""
        
        # setup layout
        if self.layout == None:
            if self.xscale > 0:
                self.layout = treelib.layoutTree(self.tree, self.xscale, -1.0)
            else:
                self.layout = treelib.layoutTree(self.tree, 1.0, -1.0, minlen=1.0)
        
        if self.showBoot:
            for node in self.tree:
                if node.data.get("boot", 0) > 0:
                    self.branchLabels[node.name] = "%.2f" % node.data["boot"] + \
                        self.branchLabels.get(node.name, "")
                
        
        # setup colors
        self.setColors(self.tree)
    
    
    
    def show(self):
        """Shows visualization window"""
        
        # create window if needed
        if self.win == None:
            self.win = summon.Window(self.name, size=self.winsize,
                                     position=self.winpos)
            newwin = True
            
            self.win.set_binding(input_key("l"), self.toggleLabels)
            self.menu = SumTreeMenu(self)
            self.win.set_menu(self.menu)
        else:
            self.win.clear_groups()
            newwin = False
        self.win.set_bgcolor(1,1,1)
        
        
        # draw tree
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
                
                y = min(node.y for node in self.tree) - 2
                self.win.add_group(self.drawScale(0, y, length, self.xscale))
        

        # put tree into view
        if newwin:
            w, h = self.win.get_size()
            x1, y1, x2, y2 = self.win.get_root().get_bounding()
            vw = x2 - x1
            vh = y2 - y1            
            
            if self.autozoom:
                self.win.set_visible(x1-.1*vw, y1-.1*vh, x2+.1*vw, y2+.4*vh,
                                     "exact")
            else:
                self.win.set_visible(x1-.1*vw, y1-.1*vh, x2+.1*vw, y2+.4*vh,
                                     "one2one")
    
    
    def toggleLabels(self):
        """toggle the visibility of leaf names"""
        
        self.enableLabels(not self.showLabels)
    
    
    #======================================================================
    # graphics functions
    #
    
    def drawTree(self, node):
        vis = []

        # draw node
        vis.append(self.drawNode(node))
        
        # recurse
        try:
            for child in node.children:
                vis.append(self.drawTree(child))
        except RuntimeError:
            print sys.exc_type, ": Tree is too deep to draw"

        return group(*vis)
    
    
    def drawNode(self, node):
        vis = []
        
        # get relavent coordinates
        nx, ny = self.layout[node]
        if node.parent:
            px = self.layout[node.parent][0]
        else:
            px = nx

        # record node position in tree
        node.x = nx
        node.y = ny
        
        # draw branch
        vis.append(lines(color(*node.color), 
                         nx, ny, px, ny))
        
        # draw cross bar
        if len(node.children) > 0:
            bot = self.layout[node.children[-1]][1]
            top = self.layout[node.children[0]][1]
            
            vis.append(lines(color(*node.color), nx, bot, nx, top))
        else:
            bot = ny -.5
            top = ny + .5

        # hotspot
        def func():
            self.selnode = node
            self.nodeClick(node)
        vis.append(hotspot("click", nx, bot, px, top, func))

        # branch label
        if node.name in self.branchLabels and nx != px:
            if self.vertical:
                d = ["vertical"]
            else:
                d = []
            h = top - ny
            
            label = group(color(0,0,1),
                          text_clip(self.branchLabels[node.name], 
                                    nx, ny+h*.1, px, top, 
                                    5, 8,
                                    "center", "bottom", *d))
            
            if not self.showBranchLabels:
                label.set_visible(False)
            self.labels.append(label)
            vis.append(label)

        # leaf label
        if node.isLeaf() and type(node.name) == str:
            if self.vertical:
                label = group(color(0,0,0),
                              text_clip(node.name, nx, top, 
                                        nx*10000, bot, 5, 12,
                                        "left", "middle", "vertical"))
            else:
                label = group(color(0,0,0),
                              text_clip(node.name, nx, top, 
                                        nx*10000, bot, 5, 12,
                                        "left", "middle"))
        
            if not self.showLabels:
                label.set_visible(False)
            self.labels.append(label)
            vis.append(label)

        
        return group(*vis)
        
    
    def drawScale(self, x, y, length, xscale):
        return group(color(0, 0, 1), lines(x, y, x + xscale*length, y),
                     text_clip("%.2f" % length, x + xscale*length, y-2, 
                                          x+10*xscale*length, y+2,
                                          3, 12,
                                "left", "middle"))
   
    
    def enableLabels(self, visible=True):
        self.showLabels = visible
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
    

