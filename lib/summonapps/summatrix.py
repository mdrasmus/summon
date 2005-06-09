import random
import sys
import util

from summon import *
from summonlib import shapes


# global info
mat = {}

nrows = 0
ncols = 0
nnz = 0
rperm = []
cperm = []
rinv  = []
cinv  = []


def usage():
    print "usage: summatrix (-adj|-smat|-dense) <file> [-keep]"

class Param:
    def __init__(self):
        self.keep = False
        self.sample = False
        self.sampleLevel = 1
        self.matfile = ""
        self.mattype = "undefined"

param = Param()

def parseArgs(argv):
    i = 0
    while i < len(argv):
        # matrix types
        if argv[i] == "-adj":
            param.mattype = "adj"
            param.matfile = argv[i+1]
            i += 2
        elif argv[i] == "-smat":
            param.mattype = "smat"
            param.matfile = argv[i+1]
            i += 2
        elif argv[i] == "-dense":
            param.mattype = "dense"
            param.matfile = argv[i+1]
            i += 2
        
        # permutations
        elif argv[i] == "-rp":
            rperm[:] = util.readInts(argv[i+1])
            i += 2
        elif argv[i] == "-cp":
            cperm[:] = util.readInts(argv[i+1])
            i += 2
        elif argv[i] == "-rcp":
            rperm[:] = util.readInts(argv[i+1])        
            cperm[:] = util.readInts(argv[i+1])
            i += 2
        
        # other options
        elif argv[i] == "-keep":
            param.keep = True
            i += 1
        elif argv[i] == "-sample":
            param.sample = True
            param.sampleLevel = float(argv[i+1])
            i += 2
        else:
            raise "unknown argument '%s'" % argv[i]




#############################################################################
# Matrix Reading
#

def setup(nrows, ncols, nnz):
    globals()["nrows"] = nrows
    globals()["ncols"] = ncols
    globals()["nnz"] = nnz        

    if rperm == []:
        rperm[:] = range(nrows)
    if cperm == []:
        cperm[:] = range(ncols)
    
    rinv[:] = util.invPerm(rperm)
    cinv[:] = util.invPerm(cperm)


def openAdj(filename):
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
    
    
    print "%s: %d nrows, %d ncols, %d non-zeros" % (filename, nrows, ncols, nnz)        
    
    setup(nrows, ncols, nnz)
    
    dots  = []
    row = 0
    for line in infile:
        fields = line.split()

        if param.sample and random.random() > param.sampleLevel:
            row += 1
            continue

        for i in xrange(0, len(fields), 2):
            col = int(fields[i]) - 1
            val = float(fields[i])
            dots.append(cinv[col])
            dots.append(rinv[row])
        row += 1
        
        if len(dots) > 100000:
            add_group(group(color(1,0,0), points(apply(vertices, dots))))
            dots = []
    add_group(group(color(1,0,0), points(apply(vertices, dots))))
    


def openSmat(filename):
    infile = file(filename)
    
    # read header
    (nrows, ncols, nnz) = map(int, infile.next().split())
    print "%s: %d nrows, %d ncols, %d non-zeros" % (filename, nrows, ncols, nnz)        
    
    setup(nrows, ncols, nnz)
    
    dots = []
    mat.clear()
    for line in infile:
        if param.sample and random.random() > param.sampleLevel:
            continue
        (row, col, val) = line.split()
        r = int(row)
        c = int(col)
        dots.append(cinv[c])        
        dots.append(rinv[r])
        
        keys[r] = 1
    
    # build matrix
    if param.keep:
       for i in xrange(nrows):
           mat[i] = {}
       for i in range(0, len(dots), 2):
           mat[dots[i]][dots[i+1]] = 1
    
    # draw remaining points
    add_group(group(color(1,0,0), points(apply(vertices, dots))))
    

def openDense(filename):
    infile = file(filename)
    
    # read header
    (nrows, ncols) = map(int, infile.next().split())
    nnz = nrows * ncols
    print "%s: %d nrows, %d ncols, %d non-zeros" % (filename, nrows, ncols, nnz)        
    
    setup(nrows, ncols, nnz)
    
    mat.clear()
    maxval = -1e1000
    minval = 1e1000
    r = 0
    for line in infile:
        vals = map(float, line.split())
        mat[r] = {}
        for c in xrange(len(vals)):
            mat[r][c] = vals[c]
            if vals[c] > maxval:
                maxval = vals[c]
            if vals[c] < minval:
                minval = vals[c]
            
        r += 1
    
    # draw remaining points
    vis = []
    for i in xrange(nrows):
        for j in xrange(ncols):
            if mat[i][j] >= 0:
                vis.append(color(mat[i][j] / maxval, 0, 0))
            else:
                vis.append(color(0, mat[i][j] / minval, 0))
            vis.append(vertices(j, i))
        if len(vis) > 10000:
            add_group(group(apply(points, vis)))
            vis = []
    add_group(group(apply(points, vis)))
    

    
    

def openmat(matfile, mattype):  
    util.tic("read matrix")
    
    if True:
        if mattype == "adj":
            openAdj(matfile)
        elif mattype == "smat":
            openSmat(matfile)
        elif mattype == "dense":
            openDense(matfile)
        else:
            raise "unknown matrix type %s" % mattype
    #except:
    #    print "error reading matrix file"
    util.toc()

#######################################################################
# Drawing
#

def drawBorder(nrows, ncols):
    # draw boundary 
    add_group(group(color(1,1,1), 
              shapes.boxStroke(-.5,-.5,ncols-.5, nrows-.5)))
    home()


#######################################################################
# Main Execution
#

print "SUMMATRIX (SUMMON Matrix Visualizer)"
print "Matt Rasmussen 2005"
print


set_antialias(False)


if len(sys.argv) > 2:
    parseArgs(sys.argv[2:])
    if param.mattype != "":
        openmat(param.matfile, param.mattype)
        drawBorder(nrows, ncols)
    else:
        usage()
else:
    usage()



