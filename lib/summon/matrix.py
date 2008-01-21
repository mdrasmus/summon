"""
    SUMMON Matrix module
    
    Include base class for matrices, functions for reading several matrix 
    formats, and base classes for matrix visualizations.

"""


# python libs
import random
import sys

# summon libs
from summon.core import *
import summon
from summon import shapes
from summon import util
from summon import multiwindow
from summon import colors
from summon import sumtree
from summon import treelib


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
        if nrows != None:
            self.nrows = nrows
        if ncols != None:
            self.ncols = ncols
        if nnz != None:
            self.nnz = nnz

        # set permutations
        if self.rperm == None:
            self.rperm = range(self.nrows)
        if self.cperm == None:
            self.cperm = range(self.ncols)

        # set inverse permutations
        self.rinv = util.invperm(self.rperm)
        self.cinv = util.invperm(self.cperm)
    
        # setup row/col sampling
        if rowsample != False:
            self.rshow = set(random.sample(range(self.nrows), 
                                           int(self.nrows * rowsample)))
        else:
            self.rshow = set(range(self.nrows))

        if colsample != False:
            self.cshow = random.sample(range(self.ncols), int(self.ncols * colsample))
        else:
            self.cshow = set(range(self.ncols))
    
    
    def from2DList(self, mat, cutoff=-util.INF):
        """Initialize matrix from a 2D list"""
    
        assert util.equal(* map(len, mat)), "matrix has unequal row sizes"
    
        nrows, ncols = len(mat), len(mat[0])
        self.setup(nrows, ncols, nrows * ncols)
        
        self.rows, self.cols, self.vals = [], [], []
        rows, cols, vals = self.rows, self.cols, self.vals
        
        maxval = -util.INF
        minval = util.INF
        
        for i in xrange(nrows):
            for j in xrange(ncols):
                v = mat[i][j]
                
                if v > cutoff:
                    rows.append(i)
                    cols.append(j)
                    vals.append(v)

                    self[i][j] = v

                    maxval = max(maxval, v)
                    minval = min(minval, v)
        
        self.maxval = maxval
        self.minval = minval
    
    
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
                
        for i in xrange(len(self.rows)):            
            r = rows2[i]
            c = cols2[i]
            v = vals2[i]
            
            if r not in lookuprows or c not in lookupcols:
                continue
            
            rows3.append(lookuprows[r])
            cols3.append(lookupcols[c])
            vals3.append(v)
            mat[r][c] = v
        
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


def openCompRow(filename, mat, loadvals=False, 
                sample=False, rowsample=False, colsample=False):
    """reads a compressed row matrix file"""
    
    util.tic("reading '%s'" % filename)
    infile = file(filename)
    
    # read header
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



def openLabeledMatrix(filename, mat,
                      sample=False, rowsample=False, colsample=False):
    """reads a labeled matrix file"""
    
    util.tic("reading '%s'" % filename)
    
    # read all data
    nnz = 0
    maxval = -util.INF
    minval = util.INF
    for line in file(filename):
        tokens = line.split()
        score = float(tokens[2])
        mat[tokens[0]][tokens[1]] = score
        if score > maxval: maxval = score
        if score < minval: minval = score
        nnz += 1
    
    mat.maxval = maxval
    mat.minval = minval
    
    # determine labels
    rowlabelset = set()
    collabelset = set()
    rowlabels = []
    collabels = []

    for i in mat:
        if i not in rowlabelset:
            rowlabels.append(i)
            rowlabelset.add(i)

        for j in mat[i]:
            if j not in collabelset:
                collabels.append(j)
                collabelset.add(j)
    
    # determine order
    if mat.order != None:
        order = util.readStrings(mat.order)
        rowlookup = util.list2lookup(order)
        collookup = util.list2lookup(order)
    else:
        rowlookup = util.list2lookup(rowlabels)
        collookup = util.list2lookup(collabels)        
    
    rowlabels.sort(key=lambda x: rowlookup[x])
    collabels.sort(key=lambda x: collookup[x])
    
    mat.rowlabels = rowlabels
    mat.collabels = collabels
    
    mat.setup(len(rowlabels), len(collabels), nnz, 
              rowsample=rowsample, colsample=colsample)
    rows, cols, vals = (mat.rows, mat.cols, mat.vals)
    
    # store data
    for i in mat:
        for j in mat[i]:
            rows.append(rowlookup[i])
            cols.append(collookup[j])
            vals.append(mat[i][j])
    
    util.toc()
    
    return mat
    


def openDense(filename, mat, cutoff=-util.INF,
              rowsample=False, colsample=False):
    """reads a dense matrix file"""
    
    util.tic("reading '%s'" % filename)
    infile = file(filename)
    
    # read header
    tokens = map(int, infile.next().split())
    if len(tokens) == 2:
        nrows, ncols = tokens
    elif len(tokens) == 1:
        nrows = ncols = tokens[0]
    else:
        raise Exception("Error on line 1")
    nnz = nrows * ncols
    util.log("%s: %d nrows, %d ncols, %d non-zeros" % (filename, nrows, ncols, nnz))
    

    # setup matrix
    mat.setup(nrows, ncols, nnz, rowsample=rowsample, colsample=colsample)
    rows, cols, vals = (mat.rows, mat.cols, mat.vals)
    
    # read in whole matrix
    maxval = -util.INF
    minval = util.INF
    nnz = 0
    
    for r, line in enumerate(infile):
        # sample rows
        if r not in mat.rshow:
            continue
    
        entries = map(float, line.split())
        for c in xrange(len(entries)):
            if c not in mat.cshow:
                continue
        
            if entries[c] >= cutoff:
                rows.append(r)
                cols.append(c)
                vals.append(entries[c])
                nnz += 1
                if entries[c] > maxval: maxval = entries[c]
                if entries[c] < minval: minval = entries[c]
                mat[r][c] = entries[c]
    
    mat.maxval = maxval
    mat.minval = minval
    mat.nnz = nnz
        
    util.toc()
    return mat




#=============================================================================
# Drawing
#


def getDrawColor(bgcolor=(0,0,0)):
    if bgcolor == (0,0,0):
        return (1,1,1)
    else:
        return (0,0,0)


class MatrixMenu (summon.Menu):
    """summatrix popup menu"""
    
    def __init__(self, viewer):
        summon.Menu.__init__(self)
        
        self.viewer = viewer
        
        self.summatrix_menu = summon.Menu()
        self.summatrix_menu.add_entry("toggle labels (l)", viewer.toggleLabelWindows)
        self.summatrix_menu.add_entry("toggle trees (t)", viewer.toggleTreeWindows)
        self.add_submenu("Summatrix", self.summatrix_menu)
        
        # add summon menu options
        summon.add_summon_menu_items(self, viewer.win)
        
        
        

class MatrixViewer (object):
    """Base class for Matrix Visualizations"""
    
    def __init__(self, mat=None, onClick=None, 
                 bgcolor=(0,0,0), drawzeros=False, style="points",
                 showLabels=False, showLabelWindows=False,
                 winsize=(400,400), title="summatrix",
                 rtree=None, ctree=None,
                 useTreeLens=(False, False),
                 showTreeWindows=None):
        self.win = None
        self.mat = mat
        self.bgcolor = bgcolor
        self.drawzeros = drawzeros
        self.style = style
        self.winsize = winsize[:]
        self.title = title
        self.firstOpen = True
        if onClick != None:
            self.onClick = onClick

        # labels
        self.showLabels = showLabels
        self.showLabelWindows = showLabelWindows        
        self.labelWindows = [None, None]

        # trees
        if showTreeWindows != False and (rtree != None or ctree != None):
            self.showTreeWindows = True
        else:
            self.showTreeWindows = False
        self.treeWindows = [None, None]
        self.rtree = rtree
        self.ctree = ctree
        self.useTreeLens = useTreeLens
        
        
    
    
    def setMatrix(self, mat):
        self.mat = mat
        
        
    def show(self):
        """shows the visualization window"""
        
        if self.win == None:
            self.win = summon.Window(self.title, size=self.winsize)
            self.rowEnsemble = multiwindow.WindowEnsemble([self.win], 
                                   stacky=True, sameh=True,
                                   tiey=True, piny=True,
                                   master=self.win,
                                   coordsy=[0.0])

            self.colEnsemble = multiwindow.WindowEnsemble([self.win], 
                                   stackx=True, samew=True,
                                   tiex=True, pinx=True,
                                   master=self.win,
                                   coordsx=[0.0])
        else:
            self.win.clear_groups()
        self.win.set_antialias(False)
        self.win.set_bgcolor(* self.bgcolor)
        
        self.win.set_binding(input_key("l"), self.toggleLabelWindows)
        self.win.set_binding(input_key("t"), self.toggleTreeWindows)
        self.drawMatrix(self.mat, mouseClick=self._clickCallback)
        self.win.home()
        
        self.menu = MatrixMenu(self)
        self.win.set_menu(self.menu)
        
    
    def redraw(self):
        self.firstOpen = False
        self.win.clear_groups()
        self.drawMatrix(self.mat, mouseClick=self._clickCallback)
    
    
    def drawMatrix(self, mat, mouseClick=None):
        
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
        
        if mouseClick != None:
            win.add_group(group(hotspot('click', -.5, .5, 
                                        mat.ncols-.5, -mat.nrows+.5, 
                                        mouseClick)))

        # draw extra
        self.drawBorder(mat.nrows, mat.ncols)
        self.drawPartitions(mat)
        
        if self.showLabels:
            if self.showLabelWindows:
                self.openLabelWindows()
            self.drawLabels()
        
        if self.showTreeWindows:
            self.openTreeWindows()
    
    
    def toggleLabelWindows(self):
        """rotates through (no labels, inline, and panels)"""
        
        if self.showLabelWindows:
            self.showLabels = False
            show = False
        else:
            if not self.showLabels:
                self.showLabels = True
                show = False
            else:
                show = True
            
        self.setLabelWindows(show)
    
    def setLabelWindows(self, show=True):
        """sets whether label windows are visible"""
        
        self.showLabelWindows = show
        
        if show:
            self.showLabels = True
        else:
            self.closeLabelWindows()
        
        self.redraw()
    
    
    def closeLabelWindows(self):
        """close down label windows"""
        
        if self.labelWindows[0] and self.labelWindows[0].is_open():
            self.labelWindows[0].close()
        if self.labelWindows[1] and self.labelWindows[1].is_open():
            self.labelWindows[1].close()

        self.labelWindows = [None, None]
    
    
    def openLabelWindows(self):
        """startup label windows"""

        x, y = self.win.get_position()
        w, h = self.win.get_size()
        deco = self.win.get_decoration()
        topcoord = .5
        leftcoord = -.5


        # open row labels
        if self.mat.rowlabels:
            if self.labelWindows[0] != None and \
               self.labelWindows[0].is_open():
                left = self.labelWindows[0]
            else:
                maxLabelWidth = max(map(len, self.mat.rowlabels))        
                left = summon.Window(" ", 
                                     size=(maxLabelWidth*12, h),
                                     position=(x-maxLabelWidth*12-deco[0], y))
                left.set_bgcolor(*self.win.get_bgcolor())
                left.set_visible(leftcoord, 0, leftcoord-maxLabelWidth, 1)
                left.set_boundary(leftcoord, -util.INF, leftcoord-maxLabelWidth, util.INF)

                if self.treeWindows[0] == None:
                    index = 0
                else:
                    index = 1
                self.rowEnsemble.add_window(left, index)
        else:
            left = None
        
        # open col labels
        if self.mat.collabels:
            if self.labelWindows[1] != None and \
               self.labelWindows[1].is_open():
                top = self.labelWindows[1]
            else:
                maxLabelHeight = max(map(len, self.mat.collabels))
                top = summon.Window(" ", 
                                    size=(w, maxLabelHeight*12),
                                    position=(x, y-maxLabelHeight*12-deco[1]))
                top.set_bgcolor(*self.win.get_bgcolor())
                top.set_visible(0, topcoord, 1, topcoord+maxLabelHeight) 
                top.set_boundary(-util.INF, topcoord, util.INF, topcoord+maxLabelHeight)

                if self.treeWindows[1] == None:
                    index = 0
                else:
                    index = 1
                self.colEnsemble.add_window(top, index)
        else:
            top = None
        
        self.labelWindows = [left, top]
    
    
    def toggleTreeWindows(self):
        """toggle tree windows"""
        
        self.setTreeWindows(not self.showTreeWindows)
    
    def setTreeWindows(self, show=True):
        """sets whether tree windows are visible"""
        
        self.showTreeWindows = show
        
        if not show:
            self.closeTreeWindows()
        
        self.redraw()
    
    def closeTreeWindows(self):
        """close down tree windows"""
        
        if self.treeWindows[0] and self.treeWindows[0].win.is_open():
            self.treeWindows[0].win.close()
        if self.treeWindows[1] and self.treeWindows[1].win.is_open():
            self.treeWindows[1].win.close()

        self.treeWindows = [None, None]
    
    
    def openTreeWindows(self):
        """startup tree windows"""

        x, y = self.win.get_position()
        w, h = self.win.get_size()
        deco = self.win.get_decoration()

        treesize = 100

        # open row tree
        if self.rtree:
            if self.treeWindows[0] != None and \
               self.treeWindows[0].win.is_open():
                left = self.treeWindows[0]
            else:
                if self.useTreeLens[0]:
                    layout = treelib.layoutTree(self.rtree, 1, -1)
                else:
                    layout = treelib.layoutTreeHierarchical(self.rtree, 1, -1)
                offset = max(c[1] for c in layout.itervalues())
                boundary1 = min(c[0] for c in layout.itervalues()) - 1.0
                boundary2 = max(c[0] for c in layout.itervalues())
                left = sumtree.SumTree(self.rtree, name="row tree", 
                                       showLabels=False,
                                       xscale=0, layout=layout,
                                       winsize=(treesize, h),
                                       winpos=(x-treesize-deco[0], y))
                left.show()
                left.win.set_bgcolor(*self.win.get_bgcolor())
                left.win.set_visible(boundary1, 0, boundary2, 1)
                
                if not self.useTreeLens[0]:
                    left.win.set_boundary(boundary1, -util.INF, boundary2, util.INF)
                
                self.rowEnsemble.add_window(left.win, 0, coordy=offset)
        else:
            left = None
        
        # open col tree
        if self.ctree:
            if self.treeWindows[1] != None and \
               self.treeWindows[1].win.is_open():
                top = self.treeWindows[1]
            else:
                if self.useTreeLens[1]:
                    layout = treelib.layoutTree(self.ctree, 1, 1)
                else:
                    layout = treelib.layoutTreeHierarchical(self.ctree, 1, 1)
                offset = min(c[1] for c in layout.itervalues())
                boundary1 = min(c[0] for c in layout.itervalues()) - 1.0
                boundary2 = max(c[0] for c in layout.itervalues()) 
                top = sumtree.SumTree(self.ctree, "col tree", 
                                      showLabels=False,
                                      xscale=0, layout=layout,
                                      vertical=True,
                                      winsize=(w, treesize),
                                      winpos=(x, y-treesize-deco[1]))
                top.show()
                top.win.set_bgcolor(*self.win.get_bgcolor())
                top.win.set_visible(0, -boundary1, 1, -boundary2)
                
                if not self.useTreeLens[1]:
                    top.win.set_boundary(-util.INF, -boundary1, util.INF, -boundary2)
                
                self.colEnsemble.add_window(top.win, 0, coordx=offset)
        else:
            top = None
        
        self.treeWindows = [left, top]
        

    def drawBorder(self, nrows, ncols):
        """draws the matrix boarder"""
    
        # draw boundary 
        self.win.add_group(group(color(* getDrawColor(self.bgcolor)), 
                           shapes.box(-.5,.5,ncols-.5, -nrows+.5, fill=False)))

    
    def drawPartitions(self, mat):
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

        self.win.add_group(group(* vis))


    
    def drawLabels(self):  
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
        rowwin, colwin = self.labelWindows
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
        


    
    def _clickCallback(self):
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
        
        self.onClick(row, col, y, x, self.mat[y][x])
    
    
    def onClick(self, row, col, i, j, val):
        """default callback for mouse clicks"""
        
        print row, col, "mat[%d][%d] = %f" % (i, j, self.mat[i][j])
    
    

class DenseMatrixViewer (MatrixViewer):
    """Matrix visualization specifically for dense matrices stored as a 
       list of lists"""
    
    def __init__(self, data, colormap=None, 
                 rlabels=None, clabels=None, cutoff=-util.INF,
                 rperm=[], cperm=[], rpart=None, cpart=None, 
                 style="quads", **options):
                 
                 #xdir=1, ydir=1, 
                 #labelPadding=2,
                 #labelSpacing=4,
                 #showVals=False,
                 #valColor=black):
        
        mat = Matrix()
        mat.from2DList(data, cutoff=cutoff)
        mat.rowlabels = rlabels
        mat.collabels = clabels
        mat.rperm = rperm
        mat.cperm = cperm
        mat.rpart = rpart
        mat.cpart = cpart
        mat.colormap = colormap
        
        mat.setup()
        
        MatrixViewer.__init__(self, mat, style=style, **options)
    
    
    def setDenseMatrix(self, data, colormap=None,
                       rlabels=None, clabels=None, cutoff=-util.INF,
                       rperm=[], cperm=[], rpart=None, cpart=None):
        """sets a new dense matrix to visualize"""
        
        self.mat = Matrix()
        self.mat.from2DList(data, cutoff=cutoff)
        self.mat.rowlabels = rlabels
        self.mat.collabels = clabels
        self.mat.rperm = rperm
        self.mat.cperm = cperm
        self.mat.rpart = rpart
        self.mat.cpart = cpart
        
        if colormap != None:
            self.mat.colormap = colormap
        
        self.mat.setup()
        
        
