import random
import sys

from summon import *
from summonlib import shapes

class Param:
    def __init__(self):
        self.keep = False
        self.sample = False
        self.sampleLevel = 1

mat = {}
param = Param()


def openadj(filename):
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
    
    
    dots = []
    row = 0
    for line in infile:
        fields = line.split()

        if param.sample and random.random() > param.sampleLevel:
            row += 1
            continue

        for i in xrange(0, len(fields), 2):
            col = int(fields[i]) - 1
            val = float(fields[i])
            dots.append(row)
            dots.append(col)
            
            if param.keep:
                mat[row][col] = val
        
        # draw 10000 points at a time
        if len(dots) > 10000:
            add_group(group(color(1,0,0), points(apply(vertices, dots))))
            dots = []
        row += 1
    
    # draw remaining points
    add_group(group(color(1,0,0), points(apply(vertices, dots))))

    # draw boundary 
    add_group(group(color(1,1,1), 
              shapes.boxStroke(-.5,-.5,ncols-.5, nrows-.5)))

    
    set_antialias(False)
    home()


def opensmat(filename):
    infile = file(filename)
    
    # read header
    (nrows, ncols, nnz) = map(int, infile.next().split())
    print "%s: %d nrows, %d ncols, %d non-zeros" % (filename, nrows, ncols, nnz)        
        
    
    dots = []
    mat.clear()
    keys = {}
    for line in infile:
        if param.sample and random.random() > param.sampleLevel:
            continue
        (row, col, val) = line.split()
        r = int(row)
        c = int(col)
        dots.append(r)
        dots.append(c)
        keys[r] = 1
    
    # build matrix
    for key in keys:
        mat[key] = {}
    for i in range(0, len(dots), 2):
        mat[dots[i]][dots[i+1]] = 1
    
    # draw remaining points
    add_group(group(color(1,0,0), points(apply(vertices, dots))))

    # draw boundary 
    add_group(group(color(1,1,1), 
              shapes.boxStroke(-.5,-.5,nrows-.5, nrows-.5)))

    
    set_antialias(False)
    home()



def parseArgs(argv):
    i = 0
    while i < len(argv):
        if argv[i] == "-adj":
            openadj(argv[i+1])
            i += 2
        elif argv[i] == "-smat":
            opensmat(argv[i+1])
            i += 2
        elif argv[i] == "-keep":
            param.keep = True
            i += 1
        elif argv[i] == "-sample":
            param.sample = True
            param.sampleLevel = float(argv[i+1])
            i += 2
        else:
            raise "unknown argument '%s'" % argv[i]

if len(sys.argv) > 2:
    parseArgs(sys.argv[2:])
else:
    print "usage: summatrix [-adj <file>] [-smat <file>] [-keep]"


