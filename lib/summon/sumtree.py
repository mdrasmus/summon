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
        self.sumtree_menu.add_entry("toggle labels (l)", viewer.toggle_labels)
        self.insert_submenu(0, "Sumtree", self.sumtree_menu)
        

def nodelabel(node):
    return node.name


class SumTree (object):
    """SUMMON Tree Visualizer"""

    def __init__(self, tree, name="SUMTREE",
                       show_labels=True, 
                       xscale=1.0,
                       show_branch_labels=True,
                       show_boot=True,
                       branch_labels={},
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
        self.show_boot = show_boot
        self.show_labels = show_labels
        self.show_branch_labels = show_branch_labels
        self.branch_labels = copy.copy(branch_labels)
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
        
    
    def set_tree(self, tree, layout=None, branch_labels=None):
        """Changes the tree in the visualization"""
        self.tree = tree
        self.layout = layout
        
        if branch_labels != None:
            self.branch_labels = copy.copy(branch_labels)
        else:
            self.branch_labels = {}
        
        self._setup_tree()
        
        
    def _setup_tree(self):
        """configure a new tree"""
        
        # setup layout
        if self.layout is None:
            if self.xscale > 0:
                self.layout = treelib.layout_tree(self.tree, self.xscale, -1.0)
            else:
                self.layout = treelib.layout_tree(
                    self.tree, 1.0, -1.0, minlen=1.0)
        

        # setup branch labels
        if self.show_boot:
            for node in self.tree:
                if node.data.get("boot", 0) > 0:
                    self.branch_labels[node.name] = (
                        "%.2f" % node.data["boot"] + 
                        self.branch_labels.get(node.name, ""))
                
        
        # setup colors
        self.set_colors(self.tree)

        # setup labels
        self._label_viewer.set_tree(self.tree, self.layout)
    

    def _new_window(self):
        """Create a new window"""
        self.win = summon.Window(self.name, size=self.winsize,
                                 position=self.winpos)
        self.win.set_binding(input_key("l"), self.toggle_labels)
        self.menu = SumTreeMenu(self)
        self.win.set_menu(self.menu)
        
    
    def show(self):
        """Shows visualization window"""

        # TODO: customize color
        
        # create window if needed
        if self.win == None:
            self._new_window()
            newwin = True
        else:
            self.win.clear_groups()
            newwin = False

        # set background
        self.win.set_bgcolor(1,1,1)
        
        # draw labels
        self._label_viewer.set_window(self.win)
        self._label_viewer.enable_updating(True, .5)
        
        # draw tree
        tree_labels = group(
            color(0,0,0),
            self.draw_tree(self.tree.root),
            self._label_viewer.get_group(True))
        if self.vertical:
            # draw vertical tree
            self.win.add_group(rotate(-90, tree_labels))
        else:        
            # draw horizontal tree
            self.win.add_group(tree_labels)
        
        # draw branch length legend
        if not self.vertical and self.xscale != 0:
            self.win.add_group(self.draw_scale())

        
        # put tree into view
        if newwin:
            w, h = self.win.get_size()
            x1, y1, x2, y2 = self.win.get_root().get_bounding()
            vw = x2 - x1
            vh = y2 - y1            
            
            if self.autozoom:
                self.win.set_visible(x1-.1*vw, y1-.1*vh, x2+.1*vw, y2+.2*vh,
                                     "exact")
            else:
                self.win.set_visible(x1-.1*vw, y1-.1*vh, x2+.1*vw, y2+.2*vh,
                                     "one2one")
    
    
    def toggle_labels(self):
        """toggle the visibility of leaf names"""
        
        self.enable_labels(not self.show_labels)
    
    
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
        if node.name in self.branch_labels and nx != px:
            if self.vertical:
                d = ["vertical"]
            else:
                d = []
            h = top - ny
            
            # TODO: parameterize color
            label = group(color(0,0,1),
                          text_clip(self.branch_labels[self.nodelabel(node)], 
                                    nx, ny+h*.1, px, top, 
                                    5, 8,
                                    "center", "bottom", *d))
            
            if not self.show_branch_labels:
                label.set_visible(False)
            self.labels.append(label)
            vis.append(label)

        return group(*vis)
        
    
    def draw_scale(self, x=0, y=None, length=None, xscale=None, 
                   col=(0,0,1)):

        maxwidth = (max(self.layout[node][0] for node in self.tree) - 
                    min(self.layout[node][0] for node in self.tree))
        
        if maxwidth == 0:
            return group()

        if y is None:
            legend_offset = 2
            y = min(self.layout[node][1] for node in self.tree) - legend_offset

        if xscale is None:
            xscale = self.xscale

        if length is None:
            # automatically choose a scale
            length = maxwidth / xscale
            order = math.floor(math.log10(length))
            length = 10 ** order

        return translate(x, y, color(*col), 
                         lines(0, 0, xscale*length, 0),
                         text_clip("%.2f" % length, 
                                   xscale*length, -2, 
                                   10*xscale*length, 2,
                                   3, 12, "left", "middle"))
   
    
    def enable_labels(self, visible=True):
        self.show_labels = visible
        self._label_viewer.get_group().set_visible(visible)
    
    
    def node_click(self, node):
        print "-" * 20
        self.print_node(node)
    
    def print_node(self, node):
        if node.is_leaf():
            print node.name
        else:
            for child in node.children:
                self.print_node(child)

    
    def set_colors(self, tree):
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
    
    
    def mark(self, names, col=(1, 0, 0), kind="box"):
        if self.mark_group == None:
            self.mark_group = self.win.add_group(group())
        
        vis = group(color(*col))
        found = 0
        
        for name in names:
            if name in self.tree.nodes:
                found += 1
                node = self.tree.nodes[name]

                if kind == "box":
                    vis.append(points(node.x, node.y))

                elif kind == "label":
                    name2 = self._label_viewer.nodelabel(node)
                    x, top, bot = self._label_viewer.get_label_coords(node)
                    vis.append(self._label_viewer.draw_label(
                            name2, x, top, bot, col=col, 
                            vertical=self.vertical))

                else:
                    raise Exception("unknown kind '%s'" % kind)
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

    def __init__(self, nodelabel=nodelabel, vertical=False,
                 text_color=(0,0,0)):
        summon.VisObject.__init__(self)
        self.multiscale = multiscale.Multiscale()
        self.quadtree = None
        self.tree = None
        self.layout = None

        self.nodelabel = nodelabel
        self.vertical = vertical
        self.group = group()
        self.text_color = text_color

        self.total_width = 0

    
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
        self.total_width = size * 2.0

        # populate quadtree
        for node in tree:
            x, y = layout[node]
            self.quadtree.insert(node, (x, y, x, y))
        
        

    def update(self):
        view = self.win.get_visible()

        if self.vertical:
            xres = 2
            yres = 0
        else:
            xres = 0
            yres = 2

        # if the view is different and atleast close enough
        if self.multiscale.atleast(xres, yres, view=view) and \
           not self.multiscale.same_view(view):
            self.draw_labels()


    def draw_labels(self):
        
        # clear existing labels
        self.group.clear()

        # get visible nodes
        v = self.win.get_visible()
        if self.vertical:
            v = (-v[1], v[0], -v[3], v[2])
        # add buffer around view
        w = (v[2] - v[0]) / 2.0
        h = (v[3] - v[1]) / 2.0
        v = (v[0] - w, v[1] - h, v[2] + w, v[3] + h)
        nodes = self.quadtree.query(v)

        for node in nodes:
            # only handle leaves with names
            name = self.nodelabel(node)
            if not node.is_leaf() or not isinstance(name, basestring):
                continue
            
            # layout text
            nx, top, bot = self.get_label_coords(node)

            # draw text
            self.group.append(self.draw_label(name, nx, top, bot, 
                                              self.vertical))


    def get_label_coords(self, node):
        
        nx, ny = self.layout[node]

        if len(node.children) > 0:
            bot = self.layout[node.children[-1]][1]
            top = self.layout[node.children[0]][1]
        else:
            bot = ny -.5
            top = ny + .5

        return nx, top, bot


    def draw_label(self, name, x, top, bottom, vertical=False, col=None,):

        if col is None:
            col = self.text_color

        if vertical:
            return group(color(*col),
                         text_clip(name, x, top, 
                                   x+self.total_width*10, bottom, 4, 12,
                                   "left", "middle", "vertical"))
        else:
            return group(color(*col),
                         text_clip(name, x, top, 
                                   x+self.total_width*10, bottom, 4, 12,
                                   "left", "middle"))


#=============================================================================
# low-level reusable drawing functions




def draw_tree(tree, layout, vertical=False):
    """
    Draw a tree using a layout
    """

    vis = group()
    bends = {}

    for node in tree.postorder():
        # get node coordinates
        nx, ny = layout[node]
        px, py = layout[node.parent] if node.parent else (nx, ny)

        # determine bend point
        if vertical:
            bends[node] = (px, ny)
        else:
            bends[node] = (nx, py)
        
        # draw branch
        vis.append(lines(nx, ny, bends[node][0], bends[node][1]))

        # draw cross bar
        if len(node.children) > 0:
            a = bends[node.children[-1]]
            b = bends[node.children[0]]
            vis.append(lines(a[0], a[1], b[0], b[1]))

    return vis

 
