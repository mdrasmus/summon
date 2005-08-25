import copy
import cPickle
import math
import os
import sys
import time


def GLOBALS():
    # install globals root if needed
    if not "RASMUS_GLOBALS" in globals():
        globals()["RASMUS_GLOBALS"] = {}
    return globals()["RASMUS_GLOBALS"]
    
###############################################################################
# Timer class for timing nested sections of code

class Timer:
    def __init__(self, stream = sys.stderr):
        self.reset()
        self.stream =  stream
        self.maxdepth = 1e1000


    def start(self, msg = ""):
        t = time.clock()
        if msg != "" and self.depth() <= self.maxdepth:
            self.indent()
            print >>self.stream, "BEGIN %s: " % msg
        self.msg.append(msg)
        self.starts.append(t)
    
    def time(self):
        return self.starts[-1] - time.clock()
    
    def stop(self):
        duration = time.clock() - self.starts.pop()
        msg = self.msg.pop()
        if msg != "" and self.depth() <= self.maxdepth:
            self.indent()
            print >>self.stream, "END   %s: %f s" % (msg, duration)
        return duration
    
    def log(self, *text):
        if self.depth() <= self.maxdepth:
            self.indent()
            for i in text:
                print >>self.stream, i,
            print >>self.stream
    
    def logExact(self, text):
        if self.depth() <= self.maxdepth:
            self.stream.write(text)
    
    def indent(self):
        for i in range(self.depth()):
            print >>self.stream, "  ",
    
    def reset(self):
        self.msg = []
        self.starts = []
    
    def depth(self):
        return len(self.msg)


def log(*text):
    globalTimer().log(*text)

def logger(*text):
    globalTimer().log(*text)

def note(*text):
    print >>notefile(), " ".join(text)

def noteflush():
    notfile().flush()

def notefile(out = None):
    if out == None:
        out = file("notes", "w")
    if "notes" not in GLOBALS():
        GLOBALS()["notes"] = out
    return GLOBALS()["notes"]

        
def logExact(text):
    globalTimer().logExact(text)

def globalTimer():
    if not "timer" in GLOBALS():
        GLOBALS()["timer"] = Timer()
    return GLOBALS()["timer"]

def tic(msg = ""):
    globalTimer().start(msg)

def toc():
    globalTimer().stop()

def indent():
    globalTimer().indent()

###############################################################################
# Progress classes

class Progress:
    def __init__(self, *args):
       if len(args) == 1:           
           self.pos = 0
           self.end = float(args[0])
           self.step = .1
       elif len(args) == 2:
           self.pos = args[0]
           self.end = float(args[1])
           self.step = .1
       elif len(args) == 3:
           self.pos = args[0]
           self.end = float(args[1])
           self.step = args[2]
       else:
           raise "wrong number of arguments"

       self.prog = self.pos
   
    def update(self, stream = None, msg = "progress %2.0f%%"):
        self.pos += 1      
        if (self.pos > self.prog):
            self.prog += self.step * self.end
            if stream != None:
                print>>stream, msg % (100 * self.pos / self.end)
            else:
                log(msg % (100 * self.pos / self.end))


class ProgressBar (Progress):
    def __init__(self, *args, **dargs):
        Progress.__init__(self, *args)
        self.width = 60
        self.step = 1 / self.width
        self.bar = 0
        
        if "title" in dargs:
            title = dargs["title"]
        else:
            title = "progress"
        
        log("+-" + title + ("-"*(self.width-len(title)-1)) + "+")
        indent()
        logExact(" |")
        self.printBar()
    
    def update(self):
        self.pos += 1        
        if (self.pos > self.prog):
            self.prog += int(self.step * self.end)
            self.printBar()
            if self.pos == self.end:
                logExact("|\n")
    
    def printBar(self):
        amount = int((self.pos / self.end * self.width) - self.bar)
        logExact("*" * amount)
        self.bar += amount


###############################################################################
# Argument parsing

def parseArgs(argv, options, resthelp = ""):
    """ 
    options = [["a:", "a_long=", "a_name", "[-a <arg>] [--a_long=<arg>]"], ...]
    """
    
    import getopt
    
    
    
    short_opts = "".join(cget(options, 0))
    long_opts  = [cget(options, 1)]
    lookup = {}
    for option in options:
        lookup["-" + option[0].replace(":", "")] = option[2]
        lookup["--" + option[1].replace("=", "")] = option[2]
        
        if option[3].startswith("AUTO"):
            word = option[3][4:]
            option[3] = "[-%s %s] [--%s %s]" % \
                (option[0].replace(":", ""), word,
                 option[1].replace("=", ""), word)
    
    
    if len(argv) < 2:
        usage(os.path.basename(argv[0]), options, resthelp)
        raise "no options given"
    
    
    try:
        args, rest = getopt.getopt(argv[1:], short_opts, long_opts)
    except getopt.GetoptError, msg:
        print "error:", msg 
        usage(os.path.basename(argv[0]), options, resthelp)
        raise Exception
    
    param = {}
    for name, value in args:
        param.setdefault(lookup[name], []).append(value)
    
    return param, rest


def usage(progname, options, resthelp = ""):
    """ 
    options = [("a:", "a_long=", "a_name", "[-a <arg>] [--a_long=<arg>]"), ...]
    """
    
    print >>sys.stderr, "usage: %s [options] %s" % (progname, resthelp)
    for option in options:
        print >>sys.stderr, "   ", option[3]
    print
    
    

def getopt(* lst):
    import getopt
    param = Dict(1, [])
    
    options, rest = getopt.getopt(* lst)
    
    for option in options:
        param[option[0]].append(option[1])
    return (param.data, rest)
        

###############################################################################
# Graph plot class (GNUPLOT wrapper)

class Gnuplot:
    class Plot:
        def __init__(self, xlist, ylist, zlist, options):
            self.xlist = copy.copy(xlist)
            self.ylist = copy.copy(ylist)
            self.zlist = copy.copy(zlist)
            self.options = copy.copy(options)

    def __init__(self):
        self.data = []
        self.stream = None
        
        self.xstart = None
        self.xend = None
        self.ystart = None
        self.yend = None
        self.margin = .1
        self.enable = True

        self.options = {
            "style" : "points",
            "main"  : "",
            "xlab"  : "",
            "ylab"  : "",
            "zlab"  : "",
            "plab"  : "",
            "xfunc" : float,
            "yfunc" : float,
            "zfunc" : float
            }
    
    def setOptions(self, **options):
        for key in options:
            self.options[key] = options[key]
        self.replot()
    
    def xrange(self, start = None, end = None):
        self.xstart = start
        self.xend = end
        self.replot()
    
    def yrange(self, start = None, end = None):
        self.ystart = start
        self.yend = end
        self.replot()
    
    def unlog(self):
        self.options["xfunc"] = "none"
        self.options["yfunc"] = "none"
        self.replot()
    
    def log(self):
        self.options["xfunc"] = "none"
        self.options["yfunc"] = "log"
        self.replot()
    
    def loglog(self):
        self.options["xfunc"] = "log"
        self.options["yfunc"] = "log"
        self.replot()

    def clear(self):
        self.data = []       
    
        
    def save(self, filename = ""):
        if filename == "":
            filename = "out.ps"
        print >>self.stream, "set output \"%s\"" % filename
        if filename[-3:] == ".ps":
            print >>self.stream, "set terminal postscript color"
        if filename[-4:] == ".pdf":
            print >>self.stream, "set terminal pdf"
        if filename[-4:] == ".gif":
            print >>self.stream, "set terminal gif"
        if filename[-4:] == ".jpg":
            print >>self.stream, "set terminal jpeg"
        self.replot()
    
    
    def getTransform(self):
        def mylog(x):
            if x > 0:
                return math.log10(x)
            else:
                return 0.0
            
        funcs = []
            
        # determine transformation functions
        for axis in ["xfunc", "yfunc", "zfunc"]:
            if self.options[axis] == "none":
                funcs.append(lambda x: float(x))
            elif self.options[axis] == "log":
                funcs.append(mylog)
            else:
                funcs.append(self.options[axis])
        return funcs
    
    def findRange(self):
        (xfunc, yfunc, zfunc) = self.getTransform()
    
        bestLeft = 1e500
        bestRight = -1e500
        bestTop = -1e500
        bestBottom = 1e500
    
        # find ranges for each graph that is plotted
        for graph in self.data:
            list1 = graph.xlist
            list2 = graph.ylist

            # find border
            top    = yfunc(max(list2))
            bottom = yfunc(min(list2))
            left   = xfunc(min(list1))
            right  = xfunc(max(list1))
            
            # find margin
            ymargin = (top - bottom) * self.margin
            xmargin = (right - left) * self.margin
            if xmargin == 0: xmargin = 1
            if ymargin == 0: ymargin = 1
            
            # find new border
            top    += ymargin
            bottom -= ymargin
            left   -= xmargin
            right  += xmargin
            
            # record biggest range thus far
            if top > bestTop:       bestTop = top
            if bottom < bestBottom: bestBottom = bottom
            if left < bestLeft:     bestLeft = left
            if right > bestRight:   bestRight = right
        
        return (bestTop, bestBottom, bestLeft, bestRight)
            
    
    def replot(self):
        if len(self.data) == 0:
            return
    
        (xfunc, yfunc, zfunc) = self.getTransform()

        # find defualt ranges
        (maxy, miny, minx, maxx) = self.findRange()
        
        # configure 
        print >>self.stream, "set mouse"
        
        # setup ranges
        if self.xstart == None:
            print >>self.stream, "set xrange[%f:%f]" % (minx, maxx)
        else:
            print >>self.stream, "set xrange[%f:%f]" % (self.xstart, self.xend)
        if self.ystart == None:
            print >>self.stream, "set yrange[%f:%f]" % (miny, maxy)
        else:
            print >>self.stream, "set yrange[%f:%f]" % (self.ystart, self.yend)
        
        # set labels
        if self.options["main"] != "":
            print >>self.stream, "set title \"" + self.options["main"] + "\""            
        if self.options["xlab"] != "":
            print >>self.stream, "set xlabel \"" + self.options["xlab"] + "\""
        if self.options["ylab"] != "":
            print >>self.stream, "set ylabel \"" + self.options["ylab"] + "\""
        if self.options["zlab"] != "":
            print >>self.stream, "set zlabel \"" + self.options["zlab"] + "\""        
        
        # give plot command
        if self.data[0].zlist == []:
            print >>self.stream, "plot ",
        else:
            print >>self.stream, "splot ",
        for i in range(len(self.data)):
            graph = self.data[i]
            
            # specify inline data
            print >>self.stream, "\"-\" ",
            
            # specify style
            if graph.options["style"] != "":
                print >>self.stream, "with ", graph.options["style"],
                
            # specify plot label
            if graph.options["plab"] != "":
                print >>self.stream, " title \""+ graph.options["plab"] +"\"",
            else:
                print >>self.stream, " notitle",

            
            if i < len(self.data) - 1:
                print >>self.stream, ",",
        print >>self.stream, ""
        
        
        # output data  
        for graph in self.data:
            for i in range(len(graph.xlist)):
                if graph.zlist == []:
                    print >>self.stream, xfunc(graph.xlist[i]), \
                                         yfunc(graph.ylist[i])
                else:
                    print >>self.stream, xfunc(graph.xlist[i]), \
                                         xfunc(graph.ylist[i]), \
                                         yfunc(graph.zlist[i])
            print >>self.stream, "e"
        
        # need to make sure gnuplot gets what we have written
        self.stream.flush()
    
    
    def plot(self, list1, list2=[], list3=[], **options):
        self.setOptions(**options)
        
        if list2 == []:
            list2 = list1
            list1 = range(len(list1))
    
        if len(list1) != len(list2):
            raise "ERROR: arrays are not same length"
        
        self.data.append(self.Plot(list1, list2, list3, copy.copy(self.options)))
        
        if self.enable:
            self.stream = os.popen("gnuplot", "w")
            self.replot()
    
    def plotfunc(self, func, start, end, step, **options):
        list1 = []
        while start < end:
            list1.append(start)
            start += step    
        list2 = map(func, list1)
        self.plot(list1, list2, style="lines", ** options)
    
    def enableOutput(self, enable = True):
        self.stream = os.popen("gnuplot", "w")
        self.enable = enable
    
    

def plot(list1, list2=[], list3=[], **options):
    g = Gnuplot()
    g.plot(list1, list2, list3, **options)
    return g

def plotfunc(func, start, end, step, **options):
    g = Gnuplot()
    g.plotfunc(func, start, end, step, ** options)
    return g


###############################################################################
# my personal nested Dictionary (with default values)

class Dict:
    def __init__(self, dim = 1, null=None):
        self.dim = dim
        self.data = {}
        self.null = null
    
    def __getitem__(self, i):
        if not i in self.data:
            if self.dim > 1:
                self.data[i] = Dict(self.dim - 1, self.null)
            else:
                self.data[i] = copy.copy(self.null)
                return self.data[i]
        return self.data[i]
    
    def __setitem__(self, i, val):
        self.data[i] = val            
    
    def __delitem__(self, i):
        del self.data[i]
    
    def __len__(self):
        if self.dim == 1:
            return len(self.data)
        else:
            nnz = 0
            for i in self.data:
                nnz += len(self.data[i])
            return nnz
    
    def __iter__(self):
        return iter(self.data)
    
    def __repr__(self):
        return str(self.data)

    def clear(self):
        return self.data.clear()    
    
    def has_keys(self, *keys):
        if len(keys) == 0:
            return True
        elif len(keys) == 1:
            return self.data.has_key(keys[0])
        else:
            return self.data.has_key(keys[0]) and \
                   self.data[keys[0]].has_keys(*keys[1:])
    
    def has_key(self, key):
        return self.data.has_key(key)
    
    def keys(self):
        return self.data.keys()
    
    def values(self):
        return self.data.values()
    
    def write(self, out = sys.stdout):
        def walk(node, path):
            if node.dim == 1:
                for i in node.data:
                    print >>out, "  ",
                    for j in path:
                        print str(j) + ", ",
                    print >>out, i, ":", node.data[i]
            else:
                for i in node:
                    walk(node.data[i], path + [i])
        
        print >>out, "< DictMatrix "
        walk(self, [])
        print >>out, ">"


################################################################################
# small functions that I use a lot

def printcols(lst, width=75, out=sys.stdout):
    if len(lst) == 0:
        return
    
    lst = map(str,lst)    
    lens = map(len, lst)
    maxwidth = max(lens) + 1
    ncols = width / maxwidth
    nrows = int(math.ceil(len(lst) / float(ncols)))

    for i in xrange(nrows):
        for j in xrange(ncols):
            k = i + j*nrows
            if k < len(lst):
                out.write(lst[k] + (" " * (maxwidth - len(lst[k]))))
        out.write("\n")
    

def equal(* vals):
    if len(vals) < 2:
        return True
    a = vals[0]
    for b in vals[1:]:
        if a != b:
            return False
    return True


def remove(lst, val):
    lst.remove(val)
    return lst

def cget(mat, *i):
    if len(i) == 1:
        return [row[i[0]] for row in mat]
    else:
        return [[row[j] for j in i] for row in mat]

#def getkeys(dic, keys):
#    dic2 = {}
#    for key in keys:
#        if key in dic:
#            dic2[key] = dic[key]
#    return dic2

def subdict(dic, keys):
    dic2 = {}
    for key in keys:
        if key in dic:
            dic2[key] = dic[key]
    return dic2


def overlap(a, b, x, y):
    return (y >= a) and (x <= b)


def count(func, lst):
    n = 0
    for i in lst:
        if func(i):
            n += 1
    return n

def counteq(a, lst):
    return count(eqfunc(a), lst)

def find(func, lst):
    pos = []
    for i in xrange(len(lst)):
        if func(lst[i]):
            pos.append(i)
    return pos

def findeq(a, lst):
    return find(eqfunc(a), lst)


def argmax(lst):
    if len(lst) == 0:
        return -1
    top = 0
    for i in xrange(1, len(lst)):
        if lst[i] > lst[top]:
            top = i
    return top

def argmin(lst):
    if len(lst) == 0:
        return -1
    low = 0
    for i in xrange(1, len(lst)):
        if lst[i] < lst[low]:
            low = i
    return low

def maxfunc(func, lst):
    top = -1e1000
    for i in lst:
        val = func(i)
        if val > top:
            top = val
    return top

def minfunc(func, lst):
    low = -1e1000
    for i in lst:
        val = func(i)
        if val < low:
            low = val
    return low

def argmaxfunc(func, lst):
    if len(lst) == 0:
        return -1
    top = 0
    topval = func(lst[top])
    for i in xrange(1,len(lst)):
        val = func(lst[i])
        if val > topval:
            topval = val
            top = i
    return top
    
def argminfunc(func, lst):
    if len(lst) == 0:
        return -1
    low = 0
    lowval = func(lst[low])
    for i in xrange(1, len(lst)):
        val = func(lst[i])
        if val < lowval:
            lowval = val
            low = i
    return low

def eqfunc(a): return lambda x: x == a
def neqfunc(a): return lambda x: x != a
def ltfunc(a): return lambda x: x < a
def gtfunc(a): return lambda x: x > a
def lefunc(a): return lambda x: x <= a
def gefunc(a): return lambda x: x >= a
def withinfunc(a,b): return lambda x: a <= x <= b

def sign(num):
    return cmp(num,0)

def lg(num):
    return math.log(num, 2)

#def grab(lst, ind):
#    util.log("util.grab is deprecated")
#    lst2 = []
#    for i in ind:
#        lst2.append(lst[i])
#    return lst2


def concat(* lists):
    lst = []
    for l in lists:
        lst.extend(l)
    return lst


def sublist(lst, ind):
    lst2 = []
    for i in ind:
        lst2.append(lst[i])
    return lst2


def catchall(func):
    try:
        func()
    except:
        return None

def revdict(dct):
    dct2 = {}
    for key, val in dct.iteritems():
        dct2[val] = key
    return dct2

def list2lookup(lst):
    lookup = {}
    for i in range(len(lst)):
        lookup[lst[i]] = i
    return lookup

def list2dict(lst, val=1):
    dct = {}
    for i in lst:
        dct[i] = val
    return dct

def uniq(lst):
    return list2dict(lst).keys()

def islands(lst):

    # takes a list, or a string, and gather islands of identical elements.
    # it returns a dictionary counting where
    # counting = {element: [(start,end), (start,end), ...],
    #             element: [(start,end), (start,end), ...],
    #             ...}
    # counting.keys() is the list of unique elements of the input list
    # counting[element] is the list of all islands of occurence of element
    # counting[element][i] = (start,end)
    #  is such that list[start-1:end] only contains element
    if not list: return {}

    counting = {}

    i,current_char, current_start = 0,lst[0], 0
    
    while i < len(lst):

        if current_char == lst[i]:
            i = i+1
        else:
            if not counting.has_key(current_char): counting[current_char] = []
            counting[current_char].append((current_start, i))
            current_char = lst[i]
            current_start = i

    if not counting.has_key(current_char): counting[current_char] = []
    counting[current_char].append((current_start, i))

    return counting


# set operations
def makeset(lst):
    return list2dict(lst)

def union(set1, set2):
    set = {}
    set.update(set1)
    set.update(set2)
    return set

def intersect(set1, set2):
    set = {}
    for i in set1:
        if i in set2:
            set[i] = 1
    return set

def setSubtract(set1, set2):
    set = {}
    for i in set1:
        if i not in set2:
            set[i] = 1
    return set


# pretty printing
def int2pretty(num):
    string = str(num)
    parts = []
    l = len(string)
    for i in xrange(0, l, 3):
        t = l - i
        s = t - 3
        if s < 0: s = 0
        parts.append(string[s:t])
    parts.reverse()
    return ",".join(parts)

def pretty2int(string):
    return int(string.replace(",", ""))


###############################################################################
# common Input/Output

def readInts(filename):
    infile = openStream(filename)
    vec = []
    for line in infile:
        vec.append(int(line))
    return vec

def readFloats(filename):
    infile = openStream(filename)
    vec = []
    for line in infile:
        vec.append(float(line))
    return vec

def readStrings(filename):
    infile = openStream(filename)
    vec = [line.rstrip() for line in infile]
    return vec

def writeVector(filename, vec):
    out = openStream(filename, "w")
    for i in vec:
        print >>out, i
        
def openStream(filename, mode = "r"):
    if "read" in dir(filename):
        return filename
    elif type(filename) == str:
        return file(filename, mode)
    else:
        raise "unknown filename type"

        
class IterFunc:
    def __init__(self, func):
        self.func = func
    
    def __iter__(self):
        return self
    
    def next(self):
        return self.func()
        

class DelimReader:
    def __init__(self, filename, delim="", header=False):
        self.infile = openStream(filename)
        self.header = header
        self.delim = delim
        self.headers = []
        
        if self.header:
            self.headers = self.split(self.infile.next())

    def __iter__(self):
        return self
    
    def next(self):
        line = self.infile.next().rstrip()
        fields = self.split(line)
        
        if self.header:
            row = {}
            for i,j in zip(self.headers, fields):
                row[i] = j
            return row
        else:
            return fields

    def split(self, line):
        line = line.rstrip()
        if self.delim == "":
            return line.split()
        else:
            return line.split(self.delim)

def selcolIter(myiter, cols):       
    return IterFunc(lambda: grab(myiter.next(), cols))

def joinIter(myiter, delim):
    return IterFunc(lambda: delim.join(myiter.next()))

def readDelim(filename, delim="", header=False):
    reader = DelimReader(filename, delim, header)
    data = [row for row in reader]
    return data

def cutIter(myiter, cols, delim=""):
    if delim == "":
        return IterFunc(lambda: " ".join(grab(myiter.next().split(), cols)))
    else:
        return IterFunc(lambda: delim.join(
                                       grab(myiter.next().split(delim), cols)))

def writeDelim(out, data, delim="\t"):
    for line in data:
        print >>out, delim.join(line)

def clearFile(filename):
    out = file(filename, "w")
    out.close()

def openZip(filename):
    (infile, outfile) = os.popen2("zcat '"+filename+"' ")
    return outfile


class SafeReadIter:
    infile = None
    
    def __init__(self, infile):
        self.infile = infile
    
    def __iter__(self):
        return self
    
    def next(self):
        line = self.infile.readline()
        if line == "":
            raise StopIteration
        else:
            return line

def linecount(filename):
    count = 0
    for line in file(filename):
        count += 1
    return count

def readWord(infile, delims = [" ", "\t", "\n"]):
    word = ""
    
    while True:
        char = infile.read(1)
        if char == "":
            return word
        if char not in delims:
            word += char
            break
    
    while True:
        char = infile.read(1)
        if char == "" or char in delims:
            return word
        word += char


########################################################
# Table functions
#

tableTypesLookup = {
        "string": str,
        "int": int,
        "float": float
    }


def parseTableTypes(line, delim):
    lookup = tableTypesLookup
    names = line.replace("#Types:", "").split(delim)
    types = []
    
    for name in names:
        if name in lookup:
            types.append(lookup[name])
        else:
            types.append(eval(name))
    return types


def formatTableTypes(types, delim):
    lookup = revdict(tableTypesLookup)
    names = []
    
    for t in types:
        if t in lookup:
            names.append(lookup[t])
        else:
            names.append(t.__name__)
    return delim.join(names)
    

def readTable(filename, key, delim="\t"):
    infile = openStream(filename)
    
    types = []
    header = None
    keyid = 0
    lookup = {}
    table = {}
    
    for line in infile:
        line = line.rstrip()
        
        # handle comments
        if line[0] == "#":
            if line.startswith("#Types:"):
                types = parseTableTypes(line, delim)
            continue
        
        tokens = line.split(delim)
        
        
        if not header:
            # parse header
            header = tokens
            lookup = list2lookup(header)
            keyid = lookup[key]
        else:
            # parse data
            row = {}
            for i in xrange(len(tokens)):
                row[header[i]] = types[i](tokens[i])
            table[tokens[lookup[key]]] = row
    
    return table, header


def writeTable(filename, table, header=None, delim="\t"):
    out = openStream(filename, "w")
    
    # set default header if needed
    if not header:
        header = table.values()[0].keys()
    
    # write types    
    entry = table.values()[0]
    types = map(lambda x: type(entry[x]), header)
    out.write("#Types:" + formatTableTypes(types, delim) + "\n")
    
    # write header
    print >>out, delim.join(header)
    
    # write data
    for key,entry in table.iteritems():
        print >>out, delim.join(map(lambda x: str(entry[x]), header))
    
    
    
###############################################################################
# directory stuff
#
def listFiles(path, extension=""):
    if path[-1] != "/":
        path += "/"
    files = filter(lambda x: x.endswith(extension), os.listdir(path))
    files.sort()
    files = map(lambda x: path + x, files)
    return files

def tempfile(dir, prefix, ext): 
    import warnings
    warnings.filterwarnings("ignore", ".*", RuntimeWarning)
    filename = os.tempnam(dir, "_") + ext
    filename = filename.replace(dir + "/", dir + "/" + prefix)
    warnings.filterwarnings("default", ".*", RuntimeWarning)
    
    return filename

###############################################################################
# sorting
#
def sortPerm(array, order = "asc"):
    ind = range(len(array))
    if (order == "asc"):
        ind.sort(lambda x, y: array[x] - array[y])
    elif (order == "desc"):
        ind.sort(lambda x, y: array[y] - array[x])
    else:
        print "ERROR: bad order", order
    return ind


def sort(array, order):
    perm = sortPerm(array, order)
    sorted = [0] * len(array)
    for i in range(len(sorted)):
        sorted[i] = array[perm[i]]
    return (sorted, perm)
    
def invPerm(perm):
    inv = [0] * len(perm)
    for i in range(len(perm)):
        inv[perm[i]] = i
    return inv


###############################################################################
# histograms, distributions
#
def oneNorm(vals):
    s = float(sum(vals))
    return map(lambda x: x/s, vals)

def bucketSize(array, ndivs = 20):
    return abs(max(array) - min(array) + 1) / float(ndivs)

def hist(array, ndivs = 20):
    h = [0] * ndivs
    x = []
    bucket = bucketSize(array, ndivs)
    low = min(array)
    for i in array:
        h[int((i - low) / bucket)] += 1
    for i in range(ndivs):
        x.append(i * bucket + low)
    return (x, h)

def distrib(array, ndivs = 20):
    h = hist(array, ndivs)
    area = 0 
    delta = bucketSize(array, ndivs)
    for i in h[1]:
        area += delta * i
    return (h[0], map(lambda x: x/area, h[1]))

def plothist(array, ndivs = 20):
    h = hist(array, ndivs)
    g = Gnuplot()    
    g.plot(h[0], h[1], style="boxes")
    return (h, g)

def plotdistrib(array, ndivs = 20):
    d = distrib(array, ndivs)
    g = Gnuplot()
    g.plot(d[0], d[1], style="boxes")
    return (d, g)

def histInt(array):
    hist = [0]  * (max(array) + 1)
    negative = []
    for i in array:
        if (i >= 0):
            hist[i] += 1
        else:
            negative.append(i)
    return hist

def histDict(array):
    hist = {}
    for i in array:
        if i in hist.keys():
            hist[i] += 1
        else:
            hist[i] = 1
    return hist

###############################################################################
# pickling
#
def binsave(filename, var):
    cPickle.dump(var, openStream(filename, "w"), 2)

def binload(filename):
    return cPickle.load(openStream(filename))

def varset():
    if "varset" not in GLOBALS():
        GLOBALS()["varset"] = {}
    return GLOBALS()["varset"]

def addvar(* varnames):
    for name in varnames:
        varset()[name] = 1

def delvar(* varnames):
    for name in varnames:
        del varset()[name]

def getvars(table):
    set = subdict(table, varset())
    return set

def setvars(table, dct):
    for name in dct:
        table[name] = dct[name]

def showvars(width=70, out=sys.stdout):
    printcols(varset().keys(), width, out)

def saveall(table, filename = "all.pickle"):
    binsave(filename, getvars(table))

def loadall(table, filename = "all.pickle"):
    set = binload(filename)
    setvars(table, set)
    
    # also add new var names to varset
    set2 = varset()
    for key in set.keys():
        set2[key] = 1
        log("%s: loaded '%s'" % (filename, key))
        
def clearall():
    varset().clear()
