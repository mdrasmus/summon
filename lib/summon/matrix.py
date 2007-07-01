
import random
import sys


from summon.core import *
import summon
from summon import shapes
from summon import util
from summon import multiwindow



#############################################################################
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
        
        self.rperm = [] # maybe these should be None
        self.cperm = []
        self.rinv  = []
        self.cinv  = []
        
        self.rpart = None
        self.cpart = None
        
        self.colormap = None
        self.rows = []
        self.cols = []
        self.vals = []
        
        self.maxval = 0
        self.minval = 0
            
    
    def setup(self, nrows=None, ncols=None, nnz=None):
        if nrows != None:
            self.nrows = nrows
        if ncols != None:
            self.ncols = ncols
        if nnz != None:
            self.nnz = nnz

        if self.rperm == []:
            self.rperm = range(self.nrows)
        if self.cperm == []:
            self.cperm = range(self.ncols)

        self.rinv = util.invPerm(self.rperm)
        self.cinv = util.invPerm(self.cperm)

    def setColor(colormap, low, top):
        self.colormin = float(low)
        self.colormax = float(top)
        self.colorrange = float(top - low)
        self.colormap = colormap
        self.colorlen = colorlen
        

    def getColor(self, val):
        i = int((val - self.colormin) / self.colorrange)
        i = min(max(i, 0), self.colorlen)
        return self.colormap[i]


    def from2DList(self, mat, cutoff=-util.INF):
        assert util.equal(* map(len, mat)), "matrix has unequal row sizes"
    
        nrows, ncols = len(mat), len(mat[0])
        self.setup(nrows, ncols, nrows * ncols)
        
        self.rows, self.cols, self.vals = [], [], []
        rows, cols, vals = self.rows, self.cols, self.vals
        
        maxval = -1e1000
        minval = 1e1000
        
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
        

#############################################################################
# Matrix Reading
#


def openCompRow(filename, mat, conf={}):
    util.tic("reading '%s'" % filename)
    infile = file(filename)
    
    if "loadvals" in conf:
        loadvals = conf["loadvals"]
    else:
        loadvals = False    
    
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
    
    mat.setup(nrows, ncols, nnz)
    rows, cols, vals = (mat.rows, mat.cols, mat.vals)
        
    row = 0
    maxval = -1e1000
    minval = 1e1000
    for line in infile:
        fields = line.split()

        if "sample" in conf and random.random() > conf["sample"]:
            row += 1
            continue

        for i in xrange(0, len(fields), 2):
            col = int(fields[i]) - 1
            val = float(fields[i+1])
            rows.append(row)
            cols.append(col)
            vals.append(val)
            if val > maxval: maxval = val
            if val < minval: minval = val
            if loadvals:
                mat[r][c] = val
        row += 1
    util.toc()
    
    mat.maxval = maxval
    mat.minval = minval
    
    return mat


def openImat(filename, mat, conf={}):
    util.tic("reading '%s'" % filename)
    infile = file(filename)
    
    if "loadvals" in conf:
        loadvals = conf["loadvals"]
    else:
        loadvals = False
    
    # read header
    (nrows, ncols, nnz) = map(int, infile.next().split())
    print "%s: %d nrows, %d ncols, %d non-zeros" % (filename, nrows, ncols, nnz)        
    
    mat.setup(nrows, ncols, nnz)
    rows, cols, vals = (mat.rows, mat.cols, mat.vals)
    
    i = 0
    maxval = -1e1000
    minval = 1e1000
    for line in infile:
        if "sample" in conf and random.random() > conf["sample"]:
            continue
        
        (row, col, val) = line.split()
        r, c = int(row), int(col)
        v = float(val)
        rows.append(r)
        cols.append(c)
        vals.append(v)
        if v > maxval: maxval = v
        if v < minval: minval = v
        if loadvals:
            mat[r][c] = v
            
    util.toc()
    
    mat.maxval = maxval
    mat.minval = minval
    
    return mat



def openLabeledMatrix(filename, mat, conf={}):
    util.tic("reading '%s'" % filename)
    
    # read all data
    nnz = 0
    maxval = -1e1000
    minval = 1e1000
    for line in file(filename):
        tokens = line.split()
        score = float(tokens[2])
        mat[tokens[0]][tokens[1]] = score
        if score > maxval: maxval = score
        if score < minval: minval = score
        nnz += 1
    
    mat.maxval = maxval
    mat.minval = minval
    
    # determine order
    if mat.order != None:
        labels = util.readStrings(mat.order)
    else:
        set = {}
        for i in mat:
            set[i] = 1
            for j in mat[i]:
                set[j] = 1
        labels = util.unique(set.keys())
        labels.sort()
    lookup = util.list2lookup(labels)
    
    mat.setup(len(labels), len(labels), nnz)
    rows, cols, vals = (mat.rows, mat.cols, mat.vals)
    
    # store data
    for i in mat:
        for j in mat[i]:
            rows.append(lookup[i])
            cols.append(lookup[j])
            vals.append(mat[i][j])
    
    util.toc()
    
    return mat
    


def openDense(filename, mat, conf={"cutoff": -util.INF}):
    util.tic("reading '%s'" % filename)
    infile = file(filename)
    
    # read header
    (nrows, ncols) = map(int, infile.next().split())
    nnz = nrows * ncols
    util.log("%s: %d nrows, %d ncols, %d non-zeros" % (filename, nrows, ncols, nnz))
    
    mat.setup(nrows, ncols, nnz)
    rows, cols, vals = (mat.rows, mat.cols, mat.vals)
    
    cutoff = conf['cutoff']
    
    # read in whole matrix
    maxval = -1e1000
    minval = 1e1000
    r = 0
    
    for line in infile:
        entries = map(float, line.split())
        for c in xrange(len(entries)):
            if entries[c] >= cutoff:
                rows.append(r)
                cols.append(c)
                vals.append(entries[c])
                if entries[c] > maxval: maxval = entries[c]
                if entries[c] < minval: minval = entries[c]
                mat[r][c] = entries[c]
        r += 1
    
    mat.maxval = maxval
    mat.minval = minval
        
    util.toc()
    return mat




#######################################################################
# Drawing
#


def getDrawColor(bgcolor=(0,0,0)):
    if bgcolor == (0,0,0):
        return (1,1,1)
    else:
        return (0,0,0)


class MatrixViewer (object):
    def __init__(self, mat=None, onClick=None, 
                 bgcolor=(0,0,0), drawzeros=False, style="points",
                 showLabels=True, showLabelWindows=True):
        self.win = None
        self.mat = mat
        self.bgcolor = bgcolor
        self.drawzeros = drawzeros
        self.style = style
        self.showLabels = showLabels
        self.labelWindows = None
        self.showLabelWindows = showLabelWindows
        
        if onClick != None:
            self.onClick = onClick
    
    
    def setMatrix(self, mat):
        self.mat = mat
        
        
    def show(self):
        if self.win == None:
            self.win = summon.Window()
        else:
            self.win.clear_groups()
        self.win.set_antialias(False)
        self.win.set_bgcolor(* self.bgcolor)
        
        self.win.set_binding(input_key("1"), self.one2one)
        self.drawMatrix(self.mat, mouseClick=self.clickCallback)
        self.win.home()
    
    def redraw(self):
        self.win.clear_groups()
        self.drawMatrix(self.mat, mouseClick=self.clickCallback)
    
    
    def drawMatrix(self, mat, mouseClick=None):
        
        util.tic("drawing matrix")
        win = self.win
        mat.colormap.max = mat.maxval
        mat.colormap.min = mat.minval
        mat.colormap.range = mat.maxval - mat.minval
        getcolor = mat.colormap.get
        vis = []
        chunksize = 1000
        rows, cols, vals = (mat.rows, mat.cols, mat.vals)
        rinv, cinv = (mat.rinv, mat.cinv)
        
        # draw zeros
        if self.drawzeros:
            win.add_group(group(color(*getcolor(0)),
                          shapes.box(-.5,.5,mat.ncols-.5, -mat.nrows+.5)))
        
        # draw non-zeros
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
                win.add_group(group(quads(* vis)))

            else:
                raise Exception("unknown style '%s'" % self.style)

            vis = []
        
        if mouseClick != None:
            win.add_group(group(hotspot('click', -.5, .5, 
                                        mat.ncols-.5, -mat.nrows+.5, 
                                        mouseClick)))

        # draw extra
        self.drawBorder(mat.nrows, mat.ncols)
        self.drawPartitions(mat)
        
        
        if self.showLabels:
            if self.showLabelWindows and self.labelWindows == None:
                top = summon.Window("cols3")
                left = summon.Window("rows3")
                self.labelWindows = [left, top]
                top.set_bgcolor(*self.win.get_bgcolor())
                left.set_bgcolor(*self.win.get_bgcolor())
                left.set_size(* self.win.get_size())
                top.set_size(* self.win.get_size())
                left.set_visible(* self.win.get_visible())
                top.set_visible(* self.win.get_visible())
                
                multiwindow.tie_windows([left, self.win], tiey=True, piny=True) 
                multiwindow.tie_windows([top, self.win], tiex=True, pinx=True)                
                
                self.ensembl1 = multiwindow.WindowEnsembl([left, self.win], 
                                              stacky=True, sameh=True)
                self.ensembl2 = multiwindow.WindowEnsembl([top, self.win], 
                                              stackx=True, samew=True)
                summon.begin_updating()
                
            self.drawLabels()

        util.toc()
    
    

    def drawBorder(self, nrows, ncols):
        # draw boundary 
        self.win.add_group(group(color(* getDrawColor(self.bgcolor)), 
                           shapes.boxStroke(-.5,.5,ncols-.5, -nrows+.5)))

    
    def drawPartitions(self, mat):
        vis = [color(1,1,1)]

        # draw row partitions
        if mat.rpart != None:
            part = -1
            for i in xrange(mat.nrows):
                if mat.rpart[mat.rperm[i]] != part:
                    vis.append(lines(-.5, -i+.5, mat.ncols, -i+.5))
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
        mat = self.mat
        
        nrows = mat.nrows
        ncols = mat.ncols

        xstart = 0
        ystart = 0
        labelPadding = .1
        labelSpacing = .1
        height = 1
        width = 1
        
        if self.labelWindows != None:
            rowwin, colwin = self.labelWindows
        else:
            rowwin = colwin = self.win
        
        
        # draw labels
        if mat.rowlabels != None:
            vis = [lines(0, 0, 0, -nrows)]
            labelWidth = max(map(len, mat.rowlabels)) * 100
            for i in xrange(nrows):
                x = -.5 - labelPadding
                y = .5 - i*height + labelSpacing/2.
                vis.append(text_clip(mat.rowlabels[i],
                                     x, y, x-labelWidth, y-height+labelSpacing/2.0,
                                     4, 20, 'right', 'middle'))
            rowwin.add_group(group(color(* getDrawColor(self.bgcolor)), 
                                     translate(xstart, ystart, * vis)))
        
        if mat.collabels != None:
            vis2 = []
            labelWidth = max(map(len, mat.collabels)) * 100
            for i in xrange(ncols):
                x = .5 + labelPadding
                y = .5 - i*height + labelSpacing/2.
                vis2.append(text_clip(mat.collabels[i],
                                     x, y, x+labelWidth, y-height+labelSpacing/2.0,
                                     4, 20, 'left', 'middle', 'vertical'))
            colwin.add_group(group(color(* getDrawColor(self.bgcolor)), 
                                     translate(xstart, ystart, 
                                               rotate(90.0, * vis2))))
        
        
    
    
    def clickCallback(self):
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
        print row, col, "mat[%d][%d] = %f" % (i, j, self.mat[i][j])
    
    
    def one2one(self):
        """Make zoom one2one"""

        x, y, x2, y2 = self.win.get_visible()
        vwidth = x2 - x
        vheight = y2 - y
        width, height = self.win.get_size()

        zx = width / vwidth
        zy = height / vheight

        if zx > zy:
            self.win.zoomy(zx / zy)
        else:
            self.win.zoomx(zy / zx)


class DenseMatrixViewer (MatrixViewer):
    
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
        
        
