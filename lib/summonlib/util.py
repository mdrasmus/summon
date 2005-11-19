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
        self.streams =  [stream]
        self.maxdepth = 1e1000


    def start(self, msg = ""):
        t = time.time()
        if msg != "" and self.depth() <= self.maxdepth:
            self.indent()
            self.write("BEGIN %s:\n" % msg)
        self.msg.append(msg)
        self.starts.append(t)
        self.flush()
    
    def time(self):
        return self.starts[-1] - time.clock()
    
    def stop(self):
        duration = time.time() - self.starts.pop()
        msg = self.msg.pop()
        if msg != "" and self.depth() <= self.maxdepth:
            self.indent()
            self.write("END   %s: %f s\n" % (msg, duration))
        self.flush()
        return duration
    
    def log(self, *text):
        if self.depth() <= self.maxdepth:
            self.indent()
            for i in text:
                self.write("%s " % str(i))
            self.write("\n")
            self.flush()
    
    def logExact(self, text):
        if self.depth() <= self.maxdepth:
            self.write(text)
            self.flush()
    
    def indent(self):
        for i in range(self.depth()):
            self.write("  ")
    
    def reset(self):
        self.msg = []
        self.starts = []
    
    def depth(self):
        return len(self.msg)
    
    def write(self, text):
        for stream in self.streams:
            stream.write(text)
    
    def flush(self):
        for stream in self.streams:
            stream.flush()
    
    def addStream(self, stream):
        self.streams.append(stream)
    
    def removeStream(self, stream):
        self.streams.remove(stream)


def log(*text):
    return globalTimer().log(*text)

def logger(*text):
    return globalTimer().log(*text)

def note(*text):
    print >>notefile(), " ".join(text)

def noteflush():
    return notfile().flush()

def notefile(out = None):
    if out == None:
        out = file("/dev/null", "w")
    if "notes" not in GLOBALS():
        GLOBALS()["notes"] = out
    return GLOBALS()["notes"]

        
def logExact(text):
    return globalTimer().logExact(text)

def globalTimer():
    if not "timer" in GLOBALS():
        GLOBALS()["timer"] = Timer()
    return GLOBALS()["timer"]

def tic(msg = ""):
    return globalTimer().start(msg)

def toc():
    return globalTimer().stop()

def indent():
    return globalTimer().indent()

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
            arg = option[3][4:]
            if len(option) > 4:
                helptext = option[4]
            else:
                helptext = ""
            option[3] = "-%s | --%s %s \t%s" % \
                (option[0].replace(":", ""),
                 option[1].replace("=", ""), arg, helptext)
    
    
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
        
        self.margin = .1
        self.enable = True

        self.options = {
            # plot options
            "style" : "points",
            "main"  : "",
            "xlab"  : "",
            "ylab"  : "",
            "zlab"  : "",
            "plab"  : "",
            "xfunc" : float,
            "yfunc" : float,
            "zfunc" : float,
            "eqn": None,
            
            # graph options
            "xmin" : None,
            "xmax" : None,
            "ymin" : None,
            "ymax" : None,
            "xtics" : None,
            "ytics" : None,
            "ztics" : None,
            "xlog": None,
            "ylog": None,
            "zlog": None,
            "margin": None
            }
    

    def set(self, **options):
        for key in options:
            self.options[key] = options[key]
        self.replot()
    
    def gnuplot(self, text):
        self.stream.write(text)
    
    def xrange(self, start = None, end = None):
        self.options["xmin"] = start
        self.options["xmax"] = end
        self.replot()
    
    def yrange(self, start = None, end = None):
        self.options["ymin"] = start
        self.options["ymax"] = end
        self.replot()
    
    def unlog(self):
        self.options["xlog"] = False
        self.options["ylog"] = False
        self.replot()
    
    def xlog(self, base=10):
        self.options["xlog"] = base
        self.replot()
    
    def ylog(self, base=10):
        self.options["ylog"] = base
        self.replot()
        
    def zlog(self, base=10):
        self.options["zlog"] = base
        self.replot()
    
    def loglog(self, base=10):
        self.options["xlog"] = base
        self.options["ylog"] = base
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
            #elif self.options[axis] == "log":
            #    funcs.append(mylog)
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
            if graph.options["eqn"]:
                continue
            
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
        
        # auto scale
        if bestLeft >= .1e500:   bestLeft = "*"
        if bestRight <= -1e500:  bestRight = "*"
        if bestTop <= -1e500:     bestTop = "*"
        if bestBottom >= 1e500: bestBottom = "*"
        
        return (bestTop, bestBottom, bestLeft, bestRight)
    
        
    
    def replot(self):
        if len(self.data) == 0:
            return  
        

        # find defualt ranges
        (maxy, miny, minx, maxx) = self.findRange()
        
        # configure 
        print >>self.stream, "set mouse"
        print >>self.stream, "set mxtics"
        print >>self.stream, "set mytics"
        print >>self.stream, "set mztics"
        
        # margins
        if self.options["margin"]:
            print >>self.stream, "set tmargin %f" % self.options["margin"]
            print >>self.stream, "set bmargin %f" % self.options["margin"]
            print >>self.stream, "set lmargin %f" % self.options["margin"]
            print >>self.stream, "set rmargin %f" % self.options["margin"]
        else:
            print >>self.stream, "set tmargin"
            print >>self.stream, "set bmargin"
            print >>self.stream, "set lmargin"
            print >>self.stream, "set rmargin"
        
        # tics
        if self.options["xtics"] == None:
            print >>self.stream, "set xtics autofreq"
        else:
            print >>self.stream, "set xtics %f" % self.options["xtics"]
        if self.options["ytics"] == None:
            print >>self.stream, "set ytics autofreq"
        else:
            print >>self.stream, "set ytics %f" % self.options["ytics"]
        if self.options["ztics"] == None:
            print >>self.stream, "set ztics autofreq"
        else:
            print >>self.stream, "set ztics %f" % self.options["ztics"]

        # log scale
        print >>self.stream, "unset logscale xyz"
        if self.options["xlog"]:
            print >>self.stream, "set logscale x %d" % self.options["xlog"]
        if self.options["ylog"]:
            print >>self.stream, "set logscale y %d" % self.options["ylog"]
        if self.options["zlog"]:
            print >>self.stream, "set logscale z %d" % self.options["zlog"]
        
        # setup ranges
        if self.options["xmin"] != None: minx = self.options["xmin"]
        if self.options["xmax"] != None: maxx = self.options["xmax"]
        if self.options["ymin"] != None: miny = self.options["ymin"]
        if self.options["ymax"] != None: maxy = self.options["ymax"]
        
        print >>self.stream, "set xrange[%s:%s]" % tuple(map(str, [minx, maxx]))
        print >>self.stream, "set yrange[%s:%s]" % tuple(map(str, [miny, maxy]))
        
        
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
            
            if graph.options["eqn"]:
                # specify direct equation
                print >>self.stream, graph.options["eqn"], 
            else:
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
            if graph.options["eqn"]:
                continue
            self.outputData(graph.xlist, graph.ylist, graph.zlist)
            
        
        # need to make sure gnuplot gets what we have written
        self.stream.flush()
    
    def prepareData(self, list1, list2=[], list3=[]):
        if list2 == []:
            list2 = list1
            list1 = range(len(list1))
        
        if len(list1) != len(list2):
            raise "ERROR: arrays are not same length"
        return list1, list2, list3
    
    
    def outputData(self, list1, list2, list3=[]):
        (xfunc, yfunc, zfunc) = self.getTransform()
    
        for i in range(len(list1)):
            if list3 == []:
                print >>self.stream, xfunc(list1[i]), \
                                     yfunc(list2[i])
            else:
                print >>self.stream, xfunc(list1[i]), \
                                     xfunc(list2[i]), \
                                     yfunc(list3[i])
        print >>self.stream, "e"
    
    
    def plot(self, list1, list2=[], list3=[], **options):
        self.set(**options)
        
        list1, list2, list3 = self.prepareData(list1, list2, list3)
        self.data.append(self.Plot(list1, list2, list3, copy.copy(self.options)))
        
        if self.enable:
            self.stream = os.popen("gnuplot", "w")
            self.replot()
    
    
    # all syntax should be valid GNUPLOT syntax
    # func - a string of the function call i.e. "f(x)"
    # eqn  - a string of a GNUPLOT equation  "a*x**b"
    # params - a dictionary of parameters in eqn and their initial values
    #        ex: {"a": 1, "b": 3}
    def gfit(self, func, eqn, params, list1, list2=[], list3=[], ** options):
        self.set(** options)
    
        print len(list1), len(list2), len(list3)
    
        if not self.enable:
            raise "must be output must be enabled for fitting"
        
        list1, list2, list3 = self.prepareData(list1, list2, list3)
        
        # add data to graph
        self.data.append(self.Plot(list1, list2, list3, copy.copy(self.options)))
        
        
        # perform fitting
        self.stream = os.popen("gnuplot", "w")
        print >>self.stream, "%s = %s" % (func, eqn)
        for param, value in params.items():
            print >>self.stream, "%s = %f" % (param, value)
        print >>self.stream, "fit %s '-' via %s" % \
            (func, ",".join(params.keys()))
        self.outputData(list1, list2, list3)
       
                
        # save and read parameters
        outfile = tempfile(".", "plot", ".txt")        
        print >>self.stream, "save var '%s'" % outfile
        print >>self.stream, "print 'done'"
        self.stream.flush()     
        
        # wait for variable file
        while not os.path.isfile(outfile): pass
        params = self.readParams(outfile)
        os.remove(outfile)
        
        # build eqn for plotting
        paramlist = ""
        for param, value in params.items():
            paramlist += "%s = %s, " % (param, value)
        self.options["eqn"] = paramlist + "%s = %s, %s" % \
            (func, eqn, func)
        self.options["style"] = "lines"
        
        # add fitted eqn to graph
        self.data.append(self.Plot([], [], [], copy.copy(self.options)))
        
        self.replot()
        
    
    def readParams(self, filename):
        params = {}
        
        for line in file(filename):
            if line[0] == "#":
                continue
                
            var, value = line.split("=")
            if not var.startswith("MOUSE_"):
                params[var.replace(" ", "")] = float(value)
        
        return params
    
    
    def plotfunc(self, func, start, end, step, **options):
        x = []
        y = []
        while start < end:
            try:
                y.append(func(start))
                x.append(start)
            except ZeroDivisionError:
                pass
            start += step
        
        self.plot(x, y, style="lines", ** options)
    
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

def gfit(func, eqn, params, list1, list2=[], list3=[], ** options):
    g = Gnuplot()
    g.gfit(func, eqn, params, list1, list2, list3, ** options)
    return g


class MultiPlot:
    def __init__(self, plots, ncols=None, nrows=None, direction="row",
        width=800, height=800):
        self.plots = plots
        self.stream = os.popen("gnuplot -geometry %dx%d" % (width, height), "w")
        
        self.nrows = nrows
        self.ncols = ncols
        self.direction = direction
        
        self.replot()

        
    
    def replot(self):
        
        
        # determine layout
        nplots = len(self.plots)
        
        if self.nrows == None and self.ncols == None:
            self.ncols = int(math.sqrt(nplots))
        
        if self.ncols != None:
            self.nrows = int(math.ceil(nplots / float(self.ncols))) 
        else:
            self.ncols = int(math.ceil(nplots / float(self.nrows)))

        xstep = 1.0 / self.ncols
        ystep = 1.0 / self.nrows
        ypos = 0
        xpos = 0
        xorigin = 0.0
        yorigin = 1.0
        
        print >>self.stream, "set multiplot"
        for plot in self.plots:
            xpt = xorigin + xpos * xstep
            ypt = yorigin - (ypos+1) * ystep
            
            print >>self.stream, "set origin %f, %f" % (xpt, ypt)
            print >>self.stream, "set size %f, %f" % (xstep, ystep)
            plot.stream = self.stream
            plot.replot()
            
            if self.direction == "row":
                xpos += 1
            elif self.direction == "col":
                ypos += 1
            else:
                raise "unknown direction '%s'" % self.direction
            
            if xpos >= self.ncols:
                xpos = 0
                ypos += 1
            if ypos >= self.nrows:
                ypos = 0
                xpos += 1
            
        
        print >>self.stream, "unset multiplot"


###############################################################################
# my personal nested Dictionary (with default values)

class Dict (dict):
    def __init__(self, dim = 1, null=None):
        self.dim = dim
        self.null = null
        
        # backwards compatiability
        self.data = self
    
    def __getitem__(self, i):
        if not i in self:
            if self.dim > 1:
                self[i] = Dict(self.dim - 1, self.null)
            else:
                self[i] = copy.copy(self.null)
                return self[i]
        return dict.__getitem__(self, i)
    
    def __len__(self):
        if self.dim == 1:
            return dict.__len__(self)
        else:
            nnz = 0
            for i in self:
                nnz += len(self[i])
            return nnz
        
    def has_keys(self, *keys):
        if len(keys) == 0:
            return True
        elif len(keys) == 1:
            return dict.has_key(self, keys[0])
        else:
            return dict.has_key(self, keys[0]) and \
                   self[keys[0]].has_keys(*keys[1:])
    
    def write(self, out = sys.stdout):
        def walk(node, path):
            if node.dim == 1:
                for i in node:
                    print >>out, "  ",
                    for j in path:
                        print str(j) + ", ",
                    print >>out, i, ":", node[i]
            else:
                for i in node:
                    walk(node[i], path + [i])
        
        print >>out, "< DictMatrix "
        walk(self, [])
        print >>out, ">"


class Dict2:
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
    
    def items(self):
        return self.data.items()
    
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


# print a list in columns
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


# print text with wrapping
def printWrap(text, width=80, prefix="", out=sys.stdout):
    if width == None:
        out.write(text)
        out.write("\n")
        return
    
    pos = 0
    while pos < len(text):
        out.write(prefix)
        out.write(text[pos:pos+width])
        out.write("\n")
        pos += width
    
# returns True if all arguments are equal
def equal(vals):
    if len(vals) < 2:
        return True
    a = vals[0]
    for b in vals[1:]:
        if a != b:
            return False
    return True


# removes a value from a list and return the list
def remove(lst, val):
    lst.remove(val)
    return lst

# returns the column(s) of a 2D list
def cget(mat, *i):
    if len(i) == 1:
        return [row[i[0]] for row in mat]
    else:
        return [[row[j] for j in i] for row in mat]


# returns a subdictionary
def subdict(dic, keys):
    dic2 = {}
    for key in keys:
        if key in dic:
            dic2[key] = dic[key]
    return dic2

# concatenates several lists into one
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


#
# returns a sorted copy of a list
#
def sort(lst, func=cmp):
    lst2 = copy.copy(lst)
    lst2.sort(func)
    return lst2

#
# counts the number of times func(x) is True for x in list 'lst'
#
def count(func, lst):
    n = 0
    for i in lst:
        if func(i):
            n += 1
    return n

#
# counts the number of occurrences of a in lst
#
def counteq(a, lst):
    return count(eqfunc(a), lst)

#
# returns the indices of func(x) == True for x in list 'lst'
#
def find(func, lst):
    pos = []
    for i in xrange(len(lst)):
        if func(lst[i]):
            pos.append(i)
    return pos

#
# returns the indices of a occuring in list 'lst'
#
def findeq(a, lst):
    return find(eqfunc(a), lst)



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


#
# returns True if range [a,b] overlaps [x,y]
#
def overlap(a, b, x, y):
    return (y >= a) and (x <= b)


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

def argmaxfunc(func, lst, index=True):
    if len(lst) == 0:
        return -1
    top = 0
    topval = func(lst[top])
    for i in xrange(1,len(lst)):
        val = func(lst[i])
        if val > topval:
            topval = val
            top = i
    if index:
        return top
    else:
        return lst[top]
    
def argminfunc(func, lst, index=True):
    if len(lst) == 0:
        return -1
    low = 0
    lowval = func(lst[low])
    for i in xrange(1, len(lst)):
        val = func(lst[i])
        if val < lowval:
            lowval = val
            low = i
    if index:
        return low
    else:
        return lst[low]

def eqfunc(a): return lambda x: x == a
def neqfunc(a): return lambda x: x != a
def ltfunc(a): return lambda x: x < a
def gtfunc(a): return lambda x: x > a
def lefunc(a): return lambda x: x <= a
def gefunc(a): return lambda x: x >= a
def withinfunc(a, b, ainc=True, binc=True):
    if ainc:
        if binc:
            return lambda x: a <= x <= b
        else:
            return lambda x: a <= x < b
    else:
        if binc:
            return lambda x: a < x <= b
        else:
            return lambda x: a < x < b


def sign(num):
    return cmp(num,0)

def lg(num):
    return math.log(num, 2)



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
        if filename.startswith("http://"):
            return urllib2.urlopen(filename)
        elif filename == "-":
            if mode == "w":
                return sys.stdout
            elif mode == "r":
                return sys.stdin
            else:
                raise "stream '-' can only be opened with modes r/w"
        else:
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
        return IterFunc(lambda: " ".join(sublist(myiter.next().split(), cols)))
    else:
        return IterFunc(lambda: delim.join(
                                       sublist(myiter.next().split(delim), cols)))

def writeDelim(filename, data, delim="\t"):
    out = openStream(filename, "w")
    for line in data:
        print >>out, delim.join(line)

def clearFile(filename):
    out = file(filename, "w")
    out.close()

def fileSize(filename):
    return os.stat(filename)[6]

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
    for line in openStream(filename):
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
    

def readTable(filename, delim="\t"):
    infile = openStream(filename)
    
    types = []
    header = None
    table = []
    
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
        else:
            # parse data
            row = {}
            for i in xrange(len(tokens)):
                if len(types) > 0:
                    row[header[i]] = types[i](tokens[i])
                else:
                    row[header[i]] = tokens[i]
            table.append(row)
    
    return table, header


def writeTable(filename, table, header=None, delim="\t"):
    out = openStream(filename, "w")
    
    # set default header if needed
    if not header:
        header = table[0].keys()
    
    # write types    
    entry = table.values()[0]
    types = map(lambda x: type(entry[x]), header)
    out.write("#Types:" + formatTableTypes(types, delim) + "\n")
    
    # write header
    print >>out, delim.join(header)
    
    # write data
    for row in table:
        print >>out, delim.join(map(lambda x: str(row[x]), header))


def lookupTable(table, key, index=False):
    lookup = {}
    if index:
        for i in xrange(len(table)):
            key2 = table[i][key]
            if key2 in lookup:
                raise "duplicate key '%s'" % str(key2)
            lookup[key2] = i
    else:
        for i in xrange(len(table)):
            key2 = table[i][key]
            if key2 in lookup:
                raise "duplicate key '%s'" % str(key2)
            lookup[key2] = table[i]
    return lookup


def lookupTableMulti(table, * keys):
    lookup = {}
    key = keys[0]
    for i in xrange(len(table)):
        key2 = table[i][key]
        if key2 not in lookup:
            lookup[key2] = []
        lookup[key2].append(table[i])
    
    if len(keys) > 1:
        keys2 = keys[1:]
        for key, value in lookup.iteritems():
            lookup[key] = lookupTableMulti(value, * keys2)
    
    return lookup

    
    
###############################################################################
# file/directory stuff
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
def sortInd(array, compare = cmp):
    ind = range(len(array))
    ind.sort(lambda x, y: compare(array[x], array[y]))
    return ind

def sortPerm(array, compare = cmp):
    perm = sortPerm(array, compare)
    sorted = permute(array, perm)
    return (sorted, perm)

def permute(lst, perm):
    sorted = [0] * len(lst)
    for i in range(len(sorted)):
        sorted[i] = lst[perm[i]]
    return sorted
    
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
    total = float(sum(h[1]))
    return (h[0], map(lambda x: (x/total)/delta, h[1]))

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

def showvars(table=None, width=70, out=sys.stdout):
    names = varset().keys()
    names.sort()
    
    if not table:
        printcols(names, width, out)
    else:
        maxname = max(map(len, names))
        
        
        for name in names:
            out.write(name + " "*(maxname-len(name)) + "  ")
            out.write("type: " + type(table[name]).__name__)
            
            if "__len__" in dir(table[name]):
                out.write(", size: %d\n" % len(table[name]))
            


def saveall(table, filename = "all.pickle"):
    binsave(filename, getvars(table))
    
    # display variables saved
    keys = varset().keys()
    keys.sort()
    for key in keys:
        log("%s: saved '%s'" % (filename, key))

def loadall(table, filename = "all.pickle"):
    set = binload(filename)
    setvars(table, set)
    
    # also add new var names to varset
    set2 = varset()
    keys = set.keys()
    keys.sort()
    for key in keys:
        set2[key] = 1
        log("%s: loaded '%s'" % (filename, key))
        
def clearall():
    varset().clear()
