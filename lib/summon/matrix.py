"""
    SUMMON - Matrix module
    
    Include base class for matrices, functions for reading several matrix 
    formats, and base classes for matrix visualizations.

"""


# python libs
import random
import sys
import itertools

# summon libs
from summon.core import *
import summon
from summon import shapes
from summon import util
from summon import multiwindow
from summon import colors
from summon import sumtree
from summon import treelib
from summon import matrixlib


def part2perm(part):
    """Convert a partition to a simple permutation"""
    perm = range(len(part))
    perm.sort(key=lambda x: part[x])
    return perm


def set_perm_from_part(mat, rows=True, cols=True):
    
    # setup default permutation from cluster ids
    if rows and mat.rpart:
        mat.rperm = part2perm(mat.rpart)
        mat.rinv = util.invperm(mat.rperm)
    if cols and mat.cpart:
        mat.cperm = part2perm(mat.cpart)
        mat.cinv = util.invperm(mat.cperm)


#=============================================================================
# Data Structures
#


class Matrix (util.Dict):
    def __init__(self):
        util.Dict.__init__(self, dim=2, default=0)
        self.nrows = 0
        self.ncols = 0
        self.nnz   = 0
        
        self.order = None
        self.maxval = None
        self.minval = None
        
        self.rowlabels = None
        self.collabels = None
        
        self.rperm = None
        self.cperm = None
        self.rinv  = None
        self.cinv  = None
        self.rshow = None
        self.cshow = None
        
        self.rpart = None
        self.cpart = None
        
        self.colormap = None
        self.rows = []
        self.cols = []
        self.vals = []
            
    
    def setup(self, nrows=None, ncols=None, nnz=None,
              rowsample=False, colsample=False):
        # set dimensions
        if nrows is not None:
            self.nrows = nrows
        if ncols is not None:
            self.ncols = ncols
        if nnz is not None:
            self.nnz = nnz

        # set permutations
        if self.rperm is None:
            self.rperm = range(self.nrows)
        if self.cperm is None:
            self.cperm = range(self.ncols)

        # set inverse permutations
        self.rinv = util.invperm(self.rperm)
        self.cinv = util.invperm(self.cperm)
    
        # setup row/col sampling
        if rowsample is not False:
            self.rshow = set(random.sample(range(self.nrows), 
                                           int(self.nrows * rowsample)))
        else:
            self.rshow = set(range(self.nrows))

        if colsample is not False:
            self.cshow = random.sample(range(self.ncols),
                                       int(self.ncols * colsample))
        else:
            self.cshow = set(range(self.ncols))
    
    
    def from_dmat(self, mat, cutoff=-util.INF):
        """Initialize matrix from a 2D list"""

        load_dmat(mat, self, minval=cutoff, loadvals=True)
    from2DList = from_dmat
    
    
    def submatrix(self, rows=None, cols=None):
        """Returns a submatrix"""
        
        mat = Matrix()
        
        if rows == None:
            rows = range(mat.nrows)
        
        if cols == None:
            cols = range(mat.ncols)
                
        lookuprows = util.list2lookup(rows)
        lookupcols = util.list2lookup(cols)
        
        # get subset of data
        rows2, cols2, vals2 = self.rows, self.cols, self.vals
        rows3, cols3, vals3 = mat.rows, mat.cols, mat.vals
                
        for i in xrange(len(rows2)):            
            r = rows2[i]
            c = cols2[i]
            v = vals2[i]
            
            if r not in lookuprows or c not in lookupcols:
                continue

            r2 = lookuprows[r]
            c2 = lookupcols[r]
            rows3.append(r2)
            cols3.append(c2)
            vals3.append(v)
            mat[r2][c2] = v
        
        # get subset of permutation        
        ind = [self.rinv[i] for i in rows]
        lookup = util.list2lookup(util.sort(ind))
        mat.rinv = util.mget(lookup, ind)
        mat.rperm = util.invperm(mat.rinv)
        
        ind = [self.cinv[i] for i in cols]
        lookup = util.list2lookup(util.sort(ind))
        mat.cinv = util.mget(lookup, ind)
        mat.cperm = util.invperm(mat.cinv)
        
        # get subset of partition
        if mat.rpart != None:
            mat.rpart = util.mget(self.rpart, rows)
        if mat.cpart != None:
            mat.cpart = util.mget(self.cpart, cols)
        
        mat.setup(len(rows), len(cols), len(rows3))
        return mat
        

#=============================================================================
# Matrix Reading
#


def open_matrix(filename, mat, loadvals=True, minval=-util.INF,
                sample=False, rowsample=False, colsample=False,
                format="unknown"):
    """Reads a compressed row matrix file"""
    

    infile = open(filename)
    
    # read header, start iterator based on format
    if format == "rmat":
        nrows, ncols, nnz, imat = matrixlib.iter_rmat(infile)
    elif format == "imat":
        nrows, ncols, nnz, imat = matrixlib.iter_imat(infile)
    elif format == "dmat":
        nrows, ncols, nnz, imat = matrixlib.iter_dmat(infile, header=True)
    elif format == "lmat":
        nrows, ncols, nnz, imat = process_lmat(infile, mat, filename=filename)
        filename = None
    else:
        raise Exception("unknown matrix format '%s'" % format)

    load_matrix(nrows, ncols, nnz, imat, mat,
                loadvals=loadvals, minval=minval,
                sample=sample, rowsample=rowsample, colsample=colsample,
                filename=filename)


def load_matrix(nrows, ncols, nnz, imat, mat,
                loadvals=False, minval=-util.INF,
                sample=False, rowsample=False, colsample=False,
                filename=None):
    """Load matrix from an index matrix iterator"""
    
    mat.setup(nrows, ncols, nnz, rowsample=rowsample, colsample=colsample)
    rows, cols, vals = (mat.rows, mat.cols, mat.vals)

    # clear matrix
    rows[:] = []
    cols[:] = []
    vals[:] = []

    if filename:
        util.tic("reading '%s'" % filename)
        util.log("%s: %d nrows, %d ncols, %d non-zeros" %
                 (filename, nrows, ncols, nnz))
    
    try:
        for i, j, v in imat:
            # filtering: 1. random sample sample
            #            2. row/col filtering
            #            3. value cutoff
            if (sample and random.random() > sample) or \
               i not in mat.rshow or j not in mat.cshow or \
               v < minval:
                continue
            
            rows.append(i)
            cols.append(j)
            vals.append(v)
            if loadvals:
                mat[i][j] = v
                
    except Exception, e:
        if filename:
            util.toc()
        raise e

    mat.nnz = len(vals)
    mat.maxval = max(vals)
    mat.minval = min(vals)

    if filename:
        util.toc()
    

def load_dmat(dmat, mat,
              loadvals=False, minval=-util.INF,
              sample=False, rowsample=False, colsample=False):
    """Load dense matrix"""
    
    assert util.equal(* map(len, mat)), "matrix has unequal row sizes"

    nrows, ncols = len(dmat), len(dmat[0])
    nnz = nrows * ncols
    imat = matrixlib.dmat2imat(dmat)

    load_matrix(nrows, ncols, nnz, imat, mat,
                loadvals=loadvals, minval=minval,
                sample=sample, rowsample=rowsample, colsample=colsample)
        

def process_lmat(infile,  mat, filename=None):
    """Processes an lmat so that it returns as an imat"""

    if filename:
        util.tic("reading '%s'" % filename)

    rows, cols, vals = matrixlib.transpose(list(matrixlib.iter_lmat(infile)))
    
    # determine labels
    rowlabels = util.unique(rows)
    collabels = util.unique(cols)

    nrows = len(rowlabels)
    ncols = len(collabels)
    nnz = len(vals)
    
    # determine order
    if mat.order is not None:
        order = util.read_strings(mat.order)
        rowlookup = util.list2lookup(order)
        collookup = util.list2lookup(order)

        rowlabels.sort(key=lambda x: rowlookup[x])
        collabels.sort(key=lambda x: collookup[x])
    else:
        rowlookup = util.list2lookup(rowlabels)
        collookup = util.list2lookup(collabels)            
    
    mat.rowlabels = rowlabels
    mat.collabels = collabels

    # iterate with an imat, then post process
    def func():
        ilmat = itertools.izip(rows, cols, vals)
        imat = matrixlib.ilmat2imat(ilmat, rowlabels, collabels)
        for entry in imat:
            yield entry

        # also store entries by label
        for i, j, v in itertools.izip(mat.rows, mat.cols, mat.vals):
            mat[rowlabels[i]][collabels[j]] = v

    if filename:
        util.toc()
    
    return nrows, ncols, nnz, func()





#=============================================================================
# Drawing
#


def getDrawColor(bgcolor=(0,0,0)):
    if bgcolor == (0,0,0):
        return (1,1,1)
    else:
        return (0,0,0)


class MatrixMenu (summon.SummonMenu):
    """summatrix popup menu"""
    
    def __init__(self, viewer):
        summon.SummonMenu.__init__(self, viewer.win)
        
        self.viewer = viewer
        
        self.summatrix_menu = summon.Menu()
        self.summatrix_menu.add_entry("toggle labels (l)",
                                      viewer.toggle_label_windows)
        self.summatrix_menu.add_entry("toggle trees (t)",
                                      viewer.toggle_tree_windows)
        self.summatrix_menu.add_entry("toggle partitions (p)",
                                      viewer.toggle_partitions)
        self.insert_submenu(0, "Summatrix", self.summatrix_menu)
        
        
        
        

class MatrixViewer (object):
    """Base class for Matrix Visualizations"""
    
    def __init__(self, mat=None, on_click=None, 
                 bgcolor=(0,0,0), drawzeros=False, style="points",
                 show_labels=False, show_label_windows=False,
                 winsize=(400,400), title="summatrix",
                 rtree=None, ctree=None,
                 use_tree_lens=(False, False),
                 show_tree_windows=None):
        self.win = None
        self.mat = mat
        self.bgcolor = bgcolor
        self.drawzeros = drawzeros
        self.style = style
        self.winsize = winsize[:]
        self.title = title
        self.part_lines = None
        self.part_lines_visible = True
        self.first_open = True
        if on_click != None:
            self.on_click = on_click

        # labels
        self.show_labels = show_labels
        self.show_label_windows = show_label_windows        
        self.label_windows = [None, None]

        # trees
        if show_tree_windows != False and (rtree != None or ctree != None):
            self.show_tree_windows = True
        else:
            self.show_tree_windows = False
        self.tree_windows = [None, None]
        self.rtree = rtree
        self.ctree = ctree
        self.use_tree_lens = use_tree_lens

        # setup perm based on trees
        if self.rtree:
            leaves = self.rtree.leaf_names()
            if self.mat.rowlabels == None:
                self.mat.rperm = map(int, leaves)
            else:
                lookup = util.list2lookup(self.mat.rowlabels)
                self.mat.rperm = util.mget(lookup, leaves)

        if self.ctree:
            leaves = self.ctree.leaf_names()
            if self.mat.collabels == None:
                self.mat.cperm = map(int, leaves)
            else:
                lookup = util.list2lookup(self.mat.collabels)
                self.mat.cperm = util.mget(lookup, leaves)

            
        # set inverse permutations
        self.mat.rinv = util.invperm(self.mat.rperm)
        self.mat.cinv = util.invperm(self.mat.cperm)
    
    
    def set_matrix(self, mat):
        self.mat = mat
        
        
    def show(self):
        """shows the visualization window"""
        
        if self.win == None:
            self.win = summon.Window(self.title, size=self.winsize)
            self.row_ensemble = multiwindow.WindowEnsemble([self.win], 
                                   stacky=True, sameh=True,
                                   tiey=True, piny=True,
                                   master=self.win,
                                   coordsy=[0.0])

            self.col_ensemble = multiwindow.WindowEnsemble([self.win], 
                                   stackx=True, samew=True,
                                   tiex=True, pinx=True,
                                   master=self.win,
                                   coordsx=[0.0])
        else:
            #self.win.clear_groups()
            self.clear()
            
        self.win.set_antialias(False)
        self.win.set_bgcolor(* self.bgcolor)
        
        self.win.set_binding(input_key("l"), self.toggle_label_windows)
        self.win.set_binding(input_key("t"), self.toggle_tree_windows)
        self.win.set_binding(input_key("p"), self.toggle_partitions)
        self.draw_matrix(self.mat, mouse_click=self._click_callback)
        self.win.home()
        
        self.menu = MatrixMenu(self)
        self.win.set_menu(self.menu)


    def clear(self):
        """Clear graphics in all associated windows"""

        if self.win:
            self.win.clear_groups()

        for win in self.label_windows:
            if win:
                win.clear_groups()

        for win in self.tree_windows:
            if win:
                win.win.clear_groups()
                
    
    def redraw(self):
        self.first_open = False
        #self.win.clear_groups()
        self.clear()
        self.draw_matrix(self.mat, mouse_click=self._click_callback)
    
    
    def draw_matrix(self, mat, mouse_click=None):
        
        win = self.win
        
        # set default colormap
        if mat.colormap == None:
            mat.colormap = colors.RainbowColorMap()
        
        # customize colormap to matrix
        if mat.maxval == None or mat.minval == None:
            mat.maxval = max(mat.vals)
            mat.minval = min(mat.vals)
        mat.colormap.max = mat.maxval
        mat.colormap.min = mat.minval
        mat.colormap.range = mat.maxval - mat.minval
        
        
        getcolor = mat.colormap.get
        chunksize = 10000
        rows, cols, vals = (mat.rows, mat.cols, mat.vals)
        rinv, cinv = (mat.rinv, mat.cinv)
        
        # draw zeros
        if self.drawzeros:
            win.add_group(group(color(*getcolor(0)),
                          shapes.box(-.5,.5,mat.ncols-.5, -mat.nrows+.5)))
        
        # draw non-zeros
        vis = []
        vis2 = []
        for chunk in xrange(0, len(rows), chunksize):
            if self.style == "points":
                if getcolor == None:
                    for k in xrange(chunk, min(len(rows), chunk+chunksize)):
                        vis.append(cinv[cols[k]])
                        vis.append(-rinv[rows[k]])
                else:
                    for k in xrange(chunk, min(len(rows), chunk+chunksize)):
                        vis.append(color(* getcolor(vals[k])))
                        vis.append(cinv[cols[k]])
                        vis.append(-rinv[rows[k]])

                win.add_group(group(points(* vis)))

            elif self.style == "quads":

                for k in xrange(chunk, min(len(rows), chunk+chunksize)):
                    vis.extend([color(* getcolor(vals[k])),
                                cinv[cols[k]] - .5, -rinv[rows[k]] - .5,
                                cinv[cols[k]] - .5, -rinv[rows[k]] + .5,
                                cinv[cols[k]] + .5, -rinv[rows[k]] + .5,
                                cinv[cols[k]] + .5, -rinv[rows[k]] - .5
                                ])
                vis2.append(group(quads(* vis)))

            else:
                raise Exception("unknown style '%s'" % self.style)

            vis = []
        win.add_group(group(*vis2))
        
        if mouse_click != None:
            win.add_group(group(hotspot('click', -.5, .5, 
                                        mat.ncols-.5, -mat.nrows+.5, 
                                        mouse_click)))

        # draw extra
        self.draw_border(mat.nrows, mat.ncols)
        self.draw_partitions(mat)
        
        if self.show_labels:
            if self.show_label_windows:
                self.open_label_windows()
            self.draw_labels()
        
        if self.show_tree_windows:
            self.open_tree_windows()


    def toggle_partitions(self, enabled=None):
        """toggles the drawing of partition lines"""
        
        if self.part_lines:
            if enabled is None:
                enabled = not self.part_lines_visible
            
            self.part_lines.set_visible(enabled)
            self.part_lines_visible = enabled
            
    
    def toggle_label_windows(self):
        """rotates through (no labels, inline, and panels)"""
        
        if self.show_label_windows:
            self.show_labels = False
            show = False
        else:
            if not self.show_labels:
                self.show_labels = True
                show = False
            else:
                show = True
            
        self.set_label_windows(show)
    
    def set_label_windows(self, show=True):
        """sets whether label windows are visible"""
        
        self.show_label_windows = show
        
        if show:
            self.show_labels = True
        else:
            self.close_label_windows()
        
        self.redraw()
    
    
    def close_label_windows(self):
        """close down label windows"""
        
        if self.label_windows[0]:
            self.row_ensemble.remove_window(self.label_windows[0])
            self.label_windows[0].close()
        if self.label_windows[1]:
            self.col_ensemble.remove_window(self.label_windows[1])
            self.label_windows[1].close()

        self.label_windows = [None, None]
    
    
    def open_label_windows(self):
        """startup label windows"""

        x, y = self.win.get_position()
        w, h = self.win.get_size()
        deco = self.win.get_decoration()
        topcoord = .5
        leftcoord = -.5


        # open row labels
        if self.mat.rowlabels:
            if self.label_windows[0] != None and \
               self.label_windows[0].is_open():
                left = self.label_windows[0]
            else:
                maxLabelWidth = max(map(len, self.mat.rowlabels))        
                left = summon.Window(" ", 
                                     size=(maxLabelWidth*12, h),
                                     position=(x-maxLabelWidth*12-deco[0], y))
                left.set_bgcolor(*self.win.get_bgcolor())
                left.set_visible(leftcoord, 0, leftcoord-maxLabelWidth, 1)
                left.set_boundary(leftcoord, -util.INF, leftcoord-maxLabelWidth, util.INF)

                if self.tree_windows[0] == None:
                    index = 0
                else:
                    index = 1
                self.row_ensemble.add_window(left, index)
        else:
            left = None
        
        # open col labels
        if self.mat.collabels:
            if self.label_windows[1] != None and \
               self.label_windows[1].is_open():
                top = self.label_windows[1]
            else:
                maxLabelHeight = max(map(len, self.mat.collabels))
                top = summon.Window(" ", 
                                    size=(w, maxLabelHeight*12),
                                    position=(x, y-maxLabelHeight*12-deco[1]))
                top.set_bgcolor(*self.win.get_bgcolor())
                top.set_visible(0, topcoord, 1, topcoord+maxLabelHeight) 
                top.set_boundary(-util.INF, topcoord, util.INF, topcoord+maxLabelHeight)

                if self.tree_windows[1] == None:
                    index = 0
                else:
                    index = 1
                self.col_ensemble.add_window(top, index)
        else:
            top = None
        
        self.label_windows = [left, top]
    
    
    def toggle_tree_windows(self):
        """toggle tree windows"""
        
        self.set_tree_windows(not self.show_tree_windows)
    
    def set_tree_windows(self, show=True):
        """sets whether tree windows are visible"""
        
        self.show_tree_windows = show
        
        if not show:
            self.close_tree_windows()
        
        self.redraw()
    
    
    def close_tree_windows(self):
        """close down tree windows"""
        
        if self.tree_windows[0]:
            self.row_ensemble.remove_window(self.tree_windows[0].win)
            self.tree_windows[0].win.close()
        if self.tree_windows[1]:
            self.col_ensemble.remove_window(self.tree_windows[1].win)
            self.tree_windows[1].win.close()
        
        self.tree_windows = [None, None]
    
    
    def open_tree_windows(self):
        """startup tree windows"""

        x, y = self.win.get_position()
        w, h = self.win.get_size()
        deco = self.win.get_decoration()

        treesize = 100

        # open row tree
        if self.rtree:
            if self.tree_windows[0] != None and \
               self.tree_windows[0].win.is_open():
                left = self.tree_windows[0]
                left.show()
            else:
                if self.use_tree_lens[0]:
                    layout = treelib.layout_tree(self.rtree, 1, -1)
                else:
                    layout = treelib.layout_tree_hierarchical(self.rtree, 1, -1)
                offset = max(c[1] for c in layout.itervalues())
                boundary1 = min(c[0] for c in layout.itervalues()) - 1.0
                boundary2 = max(c[0] for c in layout.itervalues())
                left = sumtree.SumTree(self.rtree, name="row tree", 
                                       show_labels=False,
                                       xscale=0, layout=layout,
                                       winsize=(treesize, h),
                                       winpos=(x-treesize-deco[0], y))
                left.show()
                #left.win.set_bgcolor(*self.win.get_bgcolor())
                left.win.set_visible(boundary1, 0, boundary2, 1)
                
                if not self.use_tree_lens[0]:
                    left.win.set_boundary(boundary1, -util.INF, boundary2, util.INF)
                
                self.row_ensemble.add_window(left.win, 0, coordy=offset)
        else:
            left = None
        
        # open col tree
        if self.ctree:
            if self.tree_windows[1] != None and \
               self.tree_windows[1].win.is_open():
                top = self.tree_windows[1]
                top.show()
            else:
                if self.use_tree_lens[1]:
                    layout = treelib.layout_tree(self.ctree, 1, 1)
                else:
                    layout = treelib.layout_tree_hierarchical(self.ctree, 1, 1)
                offset = min(c[1] for c in layout.itervalues())
                boundary1 = min(c[0] for c in layout.itervalues()) - 1.0
                boundary2 = max(c[0] for c in layout.itervalues()) 
                top = sumtree.SumTree(self.ctree, "col tree", 
                                      show_labels=False,
                                      xscale=0, layout=layout,
                                      vertical=True,
                                      winsize=(w, treesize),
                                      winpos=(x, y-treesize-deco[1]))
                top.show()
                #top.win.set_bgcolor(*self.win.get_bgcolor())
                top.win.set_visible(0, -boundary1, 1, -boundary2)
                
                if not self.use_tree_lens[1]:
                    top.win.set_boundary(-util.INF, -boundary1, util.INF, -boundary2)
                
                self.col_ensemble.add_window(top.win, 0, coordx=offset)
        else:
            top = None
        
        self.tree_windows = [left, top]
        

    def draw_border(self, nrows, ncols):
        """draws the matrix boarder"""
    
        # draw boundary 
        self.win.add_group(group(color(* getDrawColor(self.bgcolor)), 
                           shapes.box(-.5,.5,ncols-.5, -nrows+.5, fill=False)))

    
    def draw_partitions(self, mat):
        """draws cluster partitions"""
    
        vis = [color(* getDrawColor(self.bgcolor))]

        # draw row partitions
        if mat.rpart != None:
            part = -1
            for i in xrange(mat.nrows):
                if mat.rpart[mat.rperm[i]] != part:
                    vis.append(lines(-.5, -i+.5, mat.ncols-.5, -i+.5))
                    part = mat.rpart[mat.rperm[i]]

        # draw col partitions
        if mat.cpart != None:
            part = -1
            for i in xrange(mat.ncols):
                if mat.cpart[mat.cperm[i]] != part:
                    vis.append(lines(i-.5, .5, i-.5, -mat.nrows+.5))
                    part = mat.cpart[mat.cperm[i]]

        self.part_lines = group(* vis)

        self.win.add_group(self.part_lines)


    
    def draw_labels(self):  
        """draws matrix labels"""
          
        mat = self.mat
        
        nrows = mat.nrows
        ncols = mat.ncols

        xstart = 0
        ystart = 0
        labelPadding = .1
        labelSpacing = .1
        height = 1
        width = 1
        maxTextSize = 12
        minTextSize = 4
        
        # determine which window to draw labels in
        rowwin, colwin = self.label_windows
        if rowwin == None:
            rowwin = self.win
        if colwin == None:
            colwin = self.win
        
        
        # draw labels
        if mat.rowlabels != None:
            vis = [lines(-.5, .5, -.5, -nrows+.5)]
            labelWidth = max(map(len, mat.rowlabels)) * 100
            for i in xrange(nrows):
                x = -.5 - labelPadding
                y = .5 - i*height + labelSpacing/2.
                vis.append(text_clip(mat.rowlabels[mat.rperm[i]],
                                     x, y, x-labelWidth, y-height+labelSpacing/2.0,
                                     minTextSize, maxTextSize, 
                                     'right', 'middle'))
            rowwin.add_group(group(color(* getDrawColor(self.bgcolor)), 
                                     translate(xstart, ystart, * vis)))
        
        if mat.collabels != None:
            vis2 = [lines(.5, .5, .5, -ncols+.5)]
            labelWidth = max(map(len, mat.collabels)) * 100
            for i in xrange(ncols):
                x = .5 + labelPadding
                y = .5 - i*height + labelSpacing/2.
                vis2.append(text_clip(mat.collabels[mat.cperm[i]],
                                  x, y, x+labelWidth, y-height+labelSpacing/2.0,
                                  minTextSize, maxTextSize, 
                                  'left', 'middle', 'vertical'))
            colwin.add_group(group(color(* getDrawColor(self.bgcolor)), 
                                     translate(xstart, ystart, 
                                               rotate(90.0, * vis2))))
        


    
    def _click_callback(self):
        """internal callback for mouse clicks"""
        
        x, y = self.win.get_mouse_pos('world')
        x = int(x+.5)
        y = -int(y-.5)

        x = self.mat.cperm[x]
        y = self.mat.rperm[y]

        if self.mat.rowlabels != None:
            row = self.mat.rowlabels[y]
        else:
            row = y
        if self.mat.collabels != None:
            col = self.mat.collabels[x]
        else:
            col = x
        
        self.on_click(row, col, y, x, self.mat[y][x])
    
    
    def on_click(self, row, col, i, j, val):
        """default callback for mouse clicks"""
        
        print row, col, "mat[%d][%d] = %f" % (i, j, self.mat[i][j])
    
    

class DenseMatrixViewer (MatrixViewer):
    """Matrix visualization specifically for dense matrices stored as a 
       list of lists"""
    
    def __init__(self, data, colormap=None, 
                 rlabels=None, clabels=None, cutoff=-util.INF,
                 rperm=None, cperm=None, rpart=None, cpart=None, 
                 style="quads", **options):
        
        mat = Matrix()
        mat.from_dmat(data, cutoff=cutoff)
        mat.rowlabels = rlabels
        mat.collabels = clabels
        mat.rperm = rperm
        mat.cperm = cperm
        mat.rpart = rpart
        mat.cpart = cpart
        mat.colormap = colormap
        
        mat.setup()
        
        MatrixViewer.__init__(self, mat, style=style, **options)
    
    
    def set_dense_matrix(self, data, colormap=None,
                       rlabels=None, clabels=None, cutoff=-util.INF,
                       rperm=None, cperm=None, rpart=None, cpart=None):
        """sets a new dense matrix to visualize"""
        
        self.mat = Matrix()
        self.mat.from_dmat(data, cutoff=cutoff)
        self.mat.rowlabels = rlabels
        self.mat.collabels = clabels
        self.mat.rperm = rperm
        self.mat.cperm = cperm
        self.mat.rpart = rpart
        self.mat.cpart = cpart
        
        if colormap != None:
            self.mat.colormap = colormap
        
        self.mat.setup()
    setDenseMatrix = set_dense_matrix
        

#=============================================================================
# OLD CODE

'''
def openCompRow(filename, mat, loadvals=False, 
                sample=False, rowsample=False, colsample=False):
    """reads a compressed row matrix file"""
    
    util.tic("reading '%s'" % filename)
    infile = file(filename)    
    
    fields = infile.next().split()
    if len(fields) == 2:
        (nrows, nnz) = map(int, fields)
        ncols = nrows
    elif len(fields) == 3:
        (nrows, ncols, nnz) = map(int, fields)
    else:
        raise "expected (nrows, nnz) or (nrows, ncols, nnz) in first line"
    
    
    util.log("%s: %d nrows, %d ncols, %d non-zeros" % (filename, nrows, ncols, nnz))
    
    mat.setup(nrows, ncols, nnz, rowsample=rowsample, colsample=colsample)
    rows, cols, vals = (mat.rows, mat.cols, mat.vals)
    
    row = 0
    maxval = -util.INF
    minval = util.INF
    nnz = 0
    for line in infile:
        fields = line.split()

        if sample != False and random.random() > sample:
            row += 1
            continue
        
        if row not in mat.rshow:
            row += 1
            continue
        
        for i in xrange(0, len(fields), 2):
            col = int(fields[i]) - 1
            val = float(fields[i+1])
            
            if col not in mat.cshow:
                continue
            
            rows.append(row)
            cols.append(col)
            vals.append(val)
            nnz += 1
            if val > maxval: maxval = val
            if val < minval: minval = val
            if loadvals:
                mat[row][col] = val
        row += 1
    util.toc()
    
    mat.maxval = maxval
    mat.minval = minval
    mat.nnz = nnz
    
    return mat

def openImat(filename, mat, loadvals=False,
             sample=False, rowsample=False, colsample=False):
    """reads a index matrix file"""
    
    util.tic("reading '%s'" % filename)
    infile = file(filename)
    
    # read header
    (nrows, ncols, nnz) = map(int, infile.next().split())
    print "%s: %d nrows, %d ncols, %d non-zeros" % (filename, nrows, ncols, nnz)        
    
    mat.setup(nrows, ncols, nnz, rowsample=rowsample, colsample=colsample)
    rows, cols, vals = (mat.rows, mat.cols, mat.vals)
    
    i = 0
    maxval = -util.INF
    minval = util.INF
    nnz = 0
    for line in infile:
        if sample != False and random.random() > sample:
            continue
        
        (row, col, val) = line.split()
        r, c = int(row), int(col)
        v = float(val)
        
        if r not in mat.rshow or c not in mat.cshow:
            continue
        
        rows.append(r)
        cols.append(c)
        vals.append(v)
        nnz += 1
        if v > maxval: maxval = v
        if v < minval: minval = v
        if loadvals:
            mat[r][c] = v
            
    util.toc()
    
    mat.maxval = maxval
    mat.minval = minval
    mat.nnz = nnz
    
    return mat

'''
