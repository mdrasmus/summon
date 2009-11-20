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
from summon import multiscale, quadtree


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
        

def nodelabel(node):
    return node.name


class SumTree (object):
    """SUMMON Tree Visualizer"""

    def __init__(self, tree, name="SUMTREE",
                       show_labels=True, 
                       xscale=1.0,
                       showBranchLabels=True,
                       showBoot=True,
                       branchLabels={},
                       autozoom=True,
                       vertical=False,
                       winsize=(400, 400),
                       winpos=None,
                       colormap=None,
                       layout=None,
                       nodelabel=nodelabel):
        self.tree = tree   
        
        self.win = None
        self.winsize = winsize
        self.winpos = winpos
        self.name = name 
        self.selnode = None
        self.mark_group = None
        self.labels = []
        self.showBoot = showBoot
        self.show_labels = show_labels
        self.showBranchLabels = showBranchLabels
        self.branchLabels = copy.copy(branchLabels)
        self.xscale = float(xscale)
        self.win = None
        self.vertical = vertical
        self.winsize = winsize
        self.colormap = colormap
        self.layout = layout
        self.autozoom = autozoom
        self.nodelabel = nodelabel

        self._label_viewer = LabelViewer(nodelabel=nodelabel,
                                         vertical=vertical)
        
        self._setup_tree()
        
    
    def set_tree(self, tree, layout=None, branchLabels=None):
        """Changes the tree in the visualization"""
        self.tree = tree
        self.layout = layout
        
        if branchLabels != None:
            self.branchLabels = copy.copy(branchLabels)
        else:
            self.branchLabels = {}
        
        self._setup_tree()
        
        
    def _setup_tree(self):
        """Private function for configuring a new tree"""
        
        # setup layout
        if self.layout is None:
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

        # setup labels
        self._label_viewer.set_tree(self.tree, self.layout)
    
    
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

        
        # draw labels
        self._label_viewer.set_window(self.win)
        self._label_viewer.enable_updating(True, .5)
        
        # draw tree
        if self.vertical:
            # draw vertical tree
            self.win.add_group(group(
                color(0,0,0),
                rotate(-90, 
                       self.drawTree(self.tree.root),
                       self._label_viewer.get_group(True))))
                
        else:        
            # draw horizontal tree
            self.win.add_group(group(
                    color(0,0,0),
                    self.drawTree(self.tree.root),
                    self._label_viewer.get_group(True)))


        
        
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
    
    
    def toggle_labels(self):
        """toggle the visibility of leaf names"""
        
        self.enable_labels(not self.show_labels)
    toggleLabels = toggle_labels
    
    
    #======================================================================
    # graphics functions
    #
    
    def draw_tree(self, node):
        vis = []

        # draw node
        vis.append(self.draw_node(node))
        
        # recurse
        try:
            for child in node.children:
                vis.append(self.draw_tree(child))
        except RuntimeError:
            print sys.exc_type, ": Tree is too deep to draw"

        return group(*vis)
    drawTree = draw_tree
    
    
    def draw_node(self, node):
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
            self.node_click(node)
        vis.append(hotspot("click", nx, bot, px, top, func))

        # TODO: move to label viewer
        # branch label
        if node.name in self.branchLabels and nx != px:
            if self.vertical:
                d = ["vertical"]
            else:
                d = []
            h = top - ny
            
            label = group(color(0,0,1),
                          text_clip(self.branchLabels[self.nodelabel(node)], 
                                    nx, ny+h*.1, px, top, 
                                    5, 8,
                                    "center", "bottom", *d))
            
            if not self.showBranchLabels:
                label.set_visible(False)
            self.labels.append(label)
            vis.append(label)

        return group(*vis)
    drawNode = draw_node
        
    
    def draw_scale(self, x, y, length, xscale):
        return group(color(0, 0, 1), lines(x, y, x + xscale*length, y),
                     text_clip("%.2f" % length, x + xscale*length, y-2, 
                                          x+10*xscale*length, y+2,
                                          3, 12,
                                "left", "middle"))
    drawScale = draw_scale
   
    
    def enable_labels(self, visible=True):
        self.show_labels = visible
        self._label_viewer.get_group().set_visible(visible)
    enableLabels = enable_labels
    
    
    def node_click(self, node):
        print "-" * 20
        self.print_node(node)
    
    def print_node(self, node):
        if node.is_leaf():
            print node.name
        else:
            for child in node.children:
                self.print_node(child)
    printNode = print_node

    
    def set_colors(self, tree):
        if self.colormap != None:
            self.colormap(tree)
        else:
            for node in tree:
                node.color = (0, 0, 0)
    setColors = set_colors
    
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
    
    
    def mark(self, names, col=(1, 0, 0)):
        if self.mark_group == None:
            self.mark_group = self.win.add_group(group())
        
        vis = group(color(*col))
        found = 0
        
        for name in names:
            if name in self.tree.nodes:
                found += 1
                node = self.tree.nodes[name]
                vis.append(points(node.x, node.y))
            else:        
                print "could not find '%s' in tree" % name
                
        print "found %d of %d nodes" % (found, len(names))
        
        self.mark_group.append(vis)
    
    
    def flag(self, names, col=(1,0,0)):
        if self.mark_group == None:
            self.mark_group = self.win.add_group(group())
        
        vis = group(color(*col))
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
            
        self.mark_group.append(vis)
    
    
    def clear_marks(self):
        if self.mark_group != None:
            self.win.remove_group(self.mark_group)
            self.mark_group = None
    


class LabelViewer (summon.VisObject):
    """A dynamic viewer for tree labels"""

    def __init__(self, nodelabel=nodelabel, vertical=False):
        summon.VisObject.__init__(self)
        self.multiscale = multiscale.Multiscale()
        self.quadtree = None
        self.tree = None
        self.layout = None

        self.nodelabel = nodelabel
        self.vertical = vertical
        self.group = group()

    
    def set_window(self, win):
        summon.VisObject.set_window(self, win)
        self.multiscale.init(win)
        self.multiscale.reset()

    def get_group(self, unparent=False):

        if unparent and self.group.get_parent():
            self.group.remove_self()
        
        return self.group

    def set_tree(self, tree, layout):

        self.multiscale.reset()
        
        self.tree = tree
        self.layout = layout

        # init quad tree
        # find boundary
        x1 = util.INF
        x2 = -util.INF
        y1 = util.INF
        y2 = -util.INF

        for node in tree:
            x, y = layout[node]
            x1 = min(x1, x)
            x2 = max(x2, x)
            y1 = min(y1, y)
            y2 = max(y2, y)

        cx = (x1 + x2) / 2.0
        cy = (y1 + y2) / 2.0
        size = max(x2 - x1, y2 - y1) / 2.0
        self.quadtree = quadtree.QuadTree(cx, cy, size)

        # populate quadtree
        for node in tree:
            x, y = layout[node]
            self.quadtree.insert(node, (x, y, x, y))
        
        

    def update(self):
        view = self.win.get_visible()

        if self.vertical:
            xres = 2
            yres = .5
        else:
            xres = .5
            yres = 2

        # if the view is different and atleast close enough
        if self.multiscale.atleast(xres, yres, view=view) and \
           not self.multiscale.same_view(view):
            self.draw_labels()



    def draw_labels(self):
        
        # clear existing labels
        self.group.clear()

        # get visible nodes
        v = self.multiscale.get_view()
        if self.vertical:
            v = (-v[1], v[0], -v[3], v[2])
        nodes = self.quadtree.query(v)
        
        for node in nodes:
            # only handle leaves with names
            name = self.nodelabel(node)
            if not node.is_leaf() or not isinstance(name, basestring):
                continue
            
            # layout text
            nx, ny = self.layout[node]

            if len(node.children) > 0:
                bot = self.layout[node.children[-1]][1]
                top = self.layout[node.children[0]][1]
            else:
                bot = ny -.5
                top = ny + .5

            # draw text
            if self.vertical:
                label = group(color(0,0,0),
                              text_clip(name, nx, top, 
                                        nx*10000, bot, 4, 12,
                                        "left", "middle", "vertical"))
            else:
                label = group(color(0,0,0),
                              text_clip(name, nx, top, 
                                        nx*10000, bot, 4, 12,
                                        "left", "middle"))

            self.group.append(label)

