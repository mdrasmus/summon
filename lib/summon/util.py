"""

    Common Utilities

    file: util.py 
    authors: Matt Rasmussen
    date: 11/30/05
 
    Provides basic functional programming functions for manipulating lists and 
    dicts.  Also provides common utilities (timers, plotting, histograms)
    
"""



# python libs
import copy
import math
import os
import re
import sys
from itertools import imap, izip



#
# see bottom of file for other imports
#


# Note: I had trouble using 1e1000 directly, because bytecode had trouble
# representing infinity (possibly)
INF = float("1e1000") 
   


class Bundle (dict):
    """
    A small class for creating a closure of variables
    handy for nested functions that need to assign to variables in an 
    outer scope

    Example:

    def func1():
        this = Bundle(var1 = 0, var2 = "hello")
        def func2():
            this.var1 += 1
        func2()
        print this.var1   
    func1()
    
    will produce:
    1
    
    """

    def __init__(self, **variables):
        for key, val in variables.iteritems():
            setattr(self, key, val)
            dict.__setitem__(self, key, val)
    
    def __setitem__(self, key, val):
        setattr(self, key, val)
        dict.__setitem__(self, key, val)
    


class Dict (dict):
    """My personal nested Dictionary (with default values)"""
    
    
    def __init__(self, items=None, dim=1, default=None, insert=True):
        """
        items   -- items to initialize Dict (can be dict, list, iter)
        dim     -- number of dimensions of the dictionary
        default -- default value of a dictionary item
        """
        
        if isinstance(items, int):
            # backwards compatiability
            default = dim
            dim = items            
        elif items is not None:
            dict.__init__(self, items)
        
        self._dim = dim
        self._null = default
        self._insert = insert
        
        # backwards compatiability
        self.data = self
    
    
    def __getitem__(self, i):
        if not i in self:
            if self._dim > 1:
                ret = Dict(self._dim - 1, self._null)
            else:
                ret = copy.copy(self._null)
            if self._insert:
                self[i] = ret
            return ret
        return dict.__getitem__(self, i)

        
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




class Percent (float):
    """Representation of a percentage"""
    digits = 1
    
    def __str__(self):
        return (("%%.%df" % self.digits) % (float(self) * 100))
    
    def __repr__(self):
        return str(self)


class PushIter (object):
    """Wrap an iterator in another iterator that allows one to push new
       items onto the front of the iteration stream"""
    
    def __init__(self, it):
        self._it = iter(it)
        self._queue = []

    def __iter__(self):
        return self
        
    def next(self):
        if len(self._queue) > 0:
            return self._queue.pop()
        else:
            return self._it.next()

    def push(self, item):
        """Push a new item onto the front of the iteration stream"""
        self._queue.append(item)

    def peek(self, default=None):
        try:
            next = self.next()
        except StopIteration:
            return default

        self.push(next)
        return next
        
       


#=============================================================================
# list and dict functions for functional programming

def equal(* vals):
    """Returns True if all arguments are equal"""
    if len(vals) < 2:
        return True
    a = vals[0]
    for b in vals[1:]:
        if a != b:
            return False
    return True


def remove(lst, *vals):
    """Returns a copy of list 'lst' with values 'vals' removed
    """
    lst2 = []
    delset = set(vals)
    for i in lst:
        if i not in delset:
            lst2.append(i)
    return lst2


def sort(lst, compare=cmp, key=None, reverse=False):
    """Returns a sorted copy of a list
       
       python2.4 now has sorted() which fulfills the same purpose
       
       lst     -- a list to sort
       compare -- a function for comparing items (default: cmp)
       key     -- function of one arg to map items
       reverse -- when True reverse sorting
    """
    lst2 = list(lst)
    lst2.sort(compare, key=key, reverse=reverse)
    return lst2


def reverse(lst):
    """Returns a reversed copy of a list
    """
    lst2 = list(lst)
    lst2.reverse()
    return lst2


def cget(mat, *i):
    """Returns the column(s) '*i' of a 2D list 'mat'
        
       mat -- matrix or 2D list 
       *i  -- columns to extract from matrix
       
       notes:
       If one column is given, the column is returned as a list.
       If multiple columns are given, a list of columns (also lists) is returned
    """
    
    if len(i) == 1:
        return [row[i[0]] for row in mat]
    else:
        return [[row[index] for row in mat]
                for index in i]


def mget(lst, ind):
    """Returns a list 'lst2' such that lst2[i] = lst[ind[i]]
       
       Or in otherwords, get the subsequence 'lst'       
    """
    return [lst[i] for i in ind]



def concat(* lists):
    """Concatenates several lists into one
    """
    
    lst = []
    for l in lists:
        lst.extend(l)
    return lst



def subdict(dic, keys):
    """
    Returns a new dictionary dic2 such that
    dic2[i] = dic[i] for all i in keys

    dic  -- a dictionary
    keys -- a list of keys
    """
    dic2 = {}
    for key in keys:
        if key in dic:
            dic2[key] = dic[key]
    return dic2


def revdict(dic, allowdups=False):
    """
    Reverses a dict 'dic' such that the keys become values and the 
    values become keys.
    
    allowdups -- if True, one of several key-value pairs with the same value 
                 will be arbitrarily choosen.  Otherwise an expection is raised
    """
    
    dic2 = {}
    if allowdups:
        for key, val in dic.iteritems():
            dic2[val] = key
    else:
        for key, val in dic.iteritems():
            assert key not in dic2, "duplicate value '%s' in dict" % val
            dic2[val] = key
    
    return dic2


def list2lookup(lst):
    """
    Creates a dict where each key is lst[i] and value is i
    """
    
    lookup = {}
    for i, elm in enumerate(lst):
        lookup[elm] = i
    return lookup


def mapdict(dic, key=lambda x: x, val=lambda x: x,
            keyfunc=None, valfunc=None):
    """
    Creates a new dict where keys and values are mapped
    
    keyfunc and valfunc are DEPRECATED
    
    """
    
    if keyfunc is not None:
        key = keyfunc
    if valfunc is not None:
        val = valfunc
    
    dic2 = {}
    for k, v in dic.iteritems():
        dic2[key(k)] = val(v)
    
    return dic2


def mapwindow(func, size, lst):
    """Apply a function 'func' to a sliding window of size 'size' within
       a list 'lst'"""
    lst2 = []
    lstlen = len(lst)
    radius = int(size // 2)

    for i in xrange(lstlen):
        radius2 = min(i, lstlen - i - 1, radius)
        lst2.append(func(lst[i-radius2:i+radius2+1]))

    return lst2


def groupby(func, lst, multi=False):
    """Places i and j of 'lst' into the same group if func(i) == func(j).
       
       func -- is a function of one argument that maps items to group objects
       lst  -- is a list of items
       multi -- if True, func must return a list of keys (key1, ..., keyn) for
                item a.  groupby will retrun a nested dict 'dct' such that
                dct[key1]...[keyn] == a
       
       returns:
       a dictionary such that the keys are groups and values are items found in
       that group
    """
    
    if not multi:
        dct = {}
        for i in lst:
            dct.setdefault(func(i), []).append(i)
    else:
        dct = {}
        for i in lst:
            keys = func(i)
            d = dct
            for key in keys[:-1]:
                d = d.setdefault(key, {})
            d.setdefault(keys[-1], []).append(i)
    
    return dct


def unique(lst):
    """
    Returns a copy of 'lst' with only unique entries.
    The list is stable (the first occurance is kept).
    """
    
    found = set()
    
    lst2 = []
    for i in lst:
        if i not in found:
            lst2.append(i)
            found.add(i)
    
    return lst2


def flatten(lst, depth=INF):
    """
    Flattens nested lists/tuples into one list
    
    depth -- specifies how deep flattening should occur
    """
    
    flat = []
    
    for elm in lst:
        if hasattr(elm, "__iter__") and depth > 0:
            flat.extend(flatten(elm, depth-1))
        else:
            flat.append(elm)
    
    return flat


def mapapply(funcs, lst):
    """
    apply each function in 'funcs' to one element in 'lst'
    """
    
    lst2 = []
    for func, item in izip(funcs, lst):
        lst2.append(func(item))
    return lst2


def cumsum(vals):
    """Returns a cumalative sum of vals (as a list)"""
    
    lst = []
    tot = 0
    for v in vals:
        tot += v
        lst.append(tot)
    return lst

def icumsum(vals):
    """Returns a cumalative sum of vals (as an iterator)"""
    
    lst = []
    tot = 0
    for v in vals:
        tot += v
        yield tot


def frange(start, end, step):
    """
    Generates a range of floats 
    
    start -- begining of range
    end   -- end of range
    step  -- step size
    """
    
    i = 0
    val = start
    while val < end:
        yield val
        i += 1
        val = start + i * step


def ilen(iterator):
    """
    Returns the size of an iterator
    """
    return sum(1 for i in iterator)



#=============================================================================
# simple matrix functions

def make_matrix(nrows, ncols, val = 0):
    mat = []
    for i in xrange(nrows):
        row = []
        mat.append(row)
        for j in xrange(ncols):
            row.append(copy.copy(val))
    return mat
makeMatrix = make_matrix


def transpose(mat):
    """
    Transpose a matrix
    
    Works better than zip() in that rows are lists not tuples
    """
    
    assert equal(* map(len, mat)), "rows are not equal length"
    
    mat2 = []
    
    for j in xrange(len(mat[0])):
        row2 = []
        mat2.append(row2)
        for row in mat:
            row2.append(row[j])
    
    return mat2
    

def submatrix(mat, rows=None, cols=None):
    """
    Returns a submatrix of 'mat' with only the rows and columns specified

    Rows and columns will appear in the order as indicated in 'rows' and 'cols'
    """
    
    if rows == None:
        rows = xrange(len(mat))
    if cols == None:
        cols = xrange(len(mat[0]))
    
    mat2 = []
    
    for i in rows:
        newrow = []
        mat2.append(newrow)
        for j in cols:
            newrow.append(mat[i][j])
    
    return mat2


def map2(func, *matrix):
    """
    Maps a function onto the elements of a matrix
    
    Also accepts multiple matrices.  Thus matrix addition is
    
    map2(add, matrix1, matrix2)
    
    """
    
    matrix2 = []
    
    for i in xrange(len(matrix[0])):
        row2 = []    
        matrix2.append(row2)

        for j in xrange(len(matrix[0][i])):
            args = [x[i][j] for x in matrix]
            row2.append(func(* args))
    
    return matrix2


def min2(matrix):
    """Finds the minimum of a 2D list or matrix
    """
    return min(imap(min, matrix))


def max2(matrix):
    """Finds the maximum of a 2D list or matrix
    """
    return max(imap(max, matrix))


def range2(width, height):
    """Iterates over the indices of a matrix
    
       Thus list(range2(3, 2)) returns
        [(0, 0), (0, 1), (1, 0), (1, 1), (2, 0), (2, 1)]
    """
    
    for i in xrange(width):
        for j in xrange(height):
            yield i, j


#=============================================================================
# list counting and finding functions


def count(func, lst):
    """
    Counts the number of times func(x) is True for x in list 'lst'
       
    See also:
        counteq(a, lst)   count items equal to a
        countneq(a, lst)  count items not equal to a
        countle(a, lst)   count items less than or equal to a
        countlt(a, lst)   count items less than a
        countge(a, lst)   count items greater than or equal to a
        countgt(a, lst)   count items greater than a
    """
    n = 0
    for i in lst:
        if func(i):
            n += 1
    return n

def counteq(a, lst): return count(eqfunc(a), lst)
def countneq(a, lst): return count(neqfunc(a), lst)
def countle(a, lst): return count(lefunc(a), lst)
def countlt(a, lst): return count(ltfunc(a), lst)
def countge(a, lst): return count(gefunc(a), lst)
def countgt(a, lst): return count(gtfunc(a), lst)


def find(func, *lsts):
    """
    Returns the indices 'i' of 'lst' where func(lst[i]) == True
    
    if N lists are passed, N arguments are passed to 'func' at a time.
    Thus, find(func, list1, list2) returns the list of indices 'i' where 
    func(list1[i], list2[i]) == True
    
    See also:
        findeq(a, lst)   find items equal to a
        findneq(a, lst)  find items not equal to a
        findle(a, lst)   find items less than or equal to a
        findlt(a, lst)   find items less than a
        findge(a, lst)   find items greater than or equal to a
        findgt(a, lst)   find items greater than a
    """
    
    pos = []
    
    if len(lsts) == 1:
        # simple case, one list
        lst = lsts[0]
        for i in xrange(len(lst)):
            if func(lst[i]):
                pos.append(i)
    else:
        # multiple lists given
        assert equal(* map(len, lsts)), "lists are not same length"
    
        nvars = len(lsts)
        for i in xrange(len(lsts[0])):
            if func(* [x[i] for x in lsts]):
                pos.append(i)
        
    return pos

def findeq(a, lst): return find(eqfunc(a), lst)
def findneq(a, lst): return find(neqfunc(a), lst)
def findle(a, lst): return find(lefunc(a), lst)
def findlt(a, lst): return find(ltfunc(a), lst)
def findge(a, lst): return find(gefunc(a), lst)
def findgt(a, lst): return find(gtfunc(a), lst)


def islands(lst):
    """Takes a iterable and returns islands of equal consequtive items 
    
    Return value is a dict with the following format
    
    counts = {elm1: [(start,end), (start,end), ...],
              elm2: [(start,end), (start,end), ...]
              ...}
    
    where for each (start,end) in counts[elm1] we have lst[start:end] only 
    containing elm1
    
    """
    
    counts = {}
    NULL = Bundle() # unique NULL
    last = NULL
    start = 0
    
    for i, x in enumerate(lst):
        if x != last and last != NULL:
            counts.setdefault(last, []).append((start, i))
            start = i
        last = x
    if last != NULL:
        counts.setdefault(last, []).append((start, i+1))
    
    return counts



def binsearch(lst, val, compare=cmp, order=1):
    """Performs binary search for val in lst using compare
    
       if val in lst:
          Returns (i, i) where lst[i] == val
       if val not in lst  
          Returns index i,j where
            lst[i] < val < lst[j]
        
       runs in O(log n)
    """

    #TODO: make a funtion based linear search
    
    assert order == 1 or order == -1
    
    low = 0
    top = len(lst) - 1
    
    if len(lst) == 0:
        return None, None
    
    if compare(lst[-1], val) * order == -1:
        return (top, None)
    
    if compare(lst[0], val) * order == 1:
        return (None, low)
    
    while top - low > 1:
        ptr = (top + low) // 2
        
        comp = compare(lst[ptr], val) * order
        
        if comp == 0:
            # have we found val exactly?
            return ptr, ptr
        elif comp == -1:
            # is val above ptr?
            low = ptr
        else:
            top = ptr
            
    
    # check top and low for exact hits
    if compare(lst[low], val) == 0:
        return low, low
    elif compare(lst[top], val) == 0:
        return top, top
    else:
        return low, top





#=============================================================================
# max and min functions

def argmax(lst, key=lambda x: x):
    """
    Find the index 'i' in 'lst' with maximum lst[i]
    
    lst -- list to search
    key -- function to apply to each lst[i].
           argmax(lst, key=func) --> argmax(map(key, lst))
    """
    
    assert len(lst) > 0
    top = 0
    topval = key(lst[0])
    for i in xrange(1, len(lst)):
        val = key(lst[i])
        if val > topval:
            top = i
            topval = val
    return top


def argmin(lst, key=lambda x: x):
    """
    Find the index 'i' in 'lst' with minimum lst[i]
    
    lst -- list to search
    key -- function to apply to each lst[i].
           argmin(lst, key=func) --> argmin(map(key, lst))
    """
    
    assert len(lst) > 0
    low = 0
    lowval = key(lst[0])
    for i in xrange(1, len(lst)):
        val = key(lst[i])
        if val < lowval:
            low = i
            lowval = val
    return low

def maxfunc(func, lst):
    """Find the element 'e' in 'lst' with maximum func(e)"""
    top = -INF
    topi = None
    for i in lst:
        val = func(i)
        if val > top:
            top = val
            topi = i
    return topi


def minfunc(func, lst):
    """Find the element 'e' in 'lst' with minimum func(e)"""
    low = INF
    lowi = None
    for i in lst:
        val = func(i)
        if val < low:
            low = val
            lowi = i
    return lowi



#=============================================================================
# math functions

#
# comparison function factories
#
# These functions will return convenient comparison functions.  
#
# example:
#   filter(ltfunc(4), lst) ==> returns all values in lst less than 4
#   count(ltfunc(4), lst)  ==> returns the number of values in lst < 4
#

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
    """Returns the sign of a number"""
    return cmp(num, 0)

def lg(num):
    """Retruns the log_2 of a number"""
    return math.log(num, 2)

def add(a, b): return a + b
def sub(a, b): return a - b
def mul(a, b): return a * b
def idiv(a, b): return a / b
def div(a, b): return a / float(b)

def safediv(a, b, default=INF):
    try:
        return a / float(b)
    except ZeroDivisionError:
        return default

def safelog(x, base=math.e, default=-INF):
    try:
        return math.log(x, base)
    except (OverflowError, ValueError):
        return default
        
def invcmp(a, b): return cmp(b, a)

def clamp(x, low, high):
    """Clamps a value 'x' between the values 'low' and 'high'
       If low == None, then there is no lower bound
       If high == None, then there is no upper bound
    """
    
    if high != None and x > high:
        return high
    elif low != None and x < low:
        return low
    else:
        return x    

def clampfunc(low, high):
    return lambda x: clamp(x, low, high)



def compose2(f, g):
    """
    Compose two functions into one

    compose2(f, g)(x) <==> f(g(x))
    """
    return lambda *args, **kargs: f(g(*args, **kargs))
    

def compose(*funcs):
    """Composes two or more functions into one function
    
       example:
       compose(f,g,h,i)(x) <==> f(g(h(i(x))))
    """

    funcs = reversed(funcs)
    f = funcs.next()
    for g in funcs:
        f = compose2(g, f)
    return f


def overlap(a, b, x, y, inc=True):
    """
    Returns True if range [a,b] overlaps [x,y]
    
    inc -- if True, treat [a,b] and [x,y] as inclusive
    """
    if inc:
        return (y >= a) and (x <= b)
    else:
        return (y > a) and (x < b)



#=============================================================================
# regex
#

def match(pattern, text):
    """
    A quick way to do pattern matching.
    
    remember: to name tokens use (?P<name>pattern)
    """
    
    m = re.match(pattern, text)
    
    if m == None:
        return {}
    else:
        return m.groupdict()

    
def evalstr(text):
    """Replace expressions in a string (aka string interpolation)

    ex:
    >>> name = 'Matt'
    >>> evalstr("My name is ${name} and my age is ${12+12}")
    'My name is Matt and my age is 24'
    
    "${!expr}" expands to "${expr}"
    
    """
    
    # get environment of caller
    frame = sys._getframe(1)
    global_dict = frame.f_globals
    local_dict = frame.f_locals
    
    # find all expression to replace
    m = re.finditer("\$\{(?P<expr>[^\}]*)\}", text)
    
    # build new string
    try:
        strs = []
        last = 0
        for x in m:
            expr = x.groupdict()['expr']
                   
            strs.append(text[last:x.start()])            
            
            if expr.startswith("!"):
                strs.append("${" + expr[1:] + "}")
            else:
                strs.append(str(eval(expr, global_dict, local_dict)))
            last = x.end()
        strs.append(text[last:len(text)])
    except Exception, e:
        raise Exception("evalstr: " + str(e))
    
    return "".join(strs)


#=============================================================================
# common Input/Output

def read_ints(filename):
    """Read a list of integers from a file (one int per line)
    
       filename may also be a stream
    """
    
    infile = open_stream(filename)
    vec = []
    for line in infile:
        vec.append(int(line))
    return vec
readInts = read_ints


def read_floats(filename):
    """Read a list of floats from a file (one float per line)
    
       filename may also be a stream
    """
    infile = open_stream(filename)
    vec = []
    for line in infile:
        vec.append(float(line))
    return vec
readFloats = read_floats


def read_strings(filename):
    """Read a list of strings from a file (one string per line)
    
       filename may also be a stream
    """
    infile = open_stream(filename)
    vec = [line.rstrip("\n") for line in infile]
    return vec
readStrings = read_strings

def read_dict(filename, delim="\t", keytype=str, valtype=str):
    """Read a dict from a file
       
       filename may also be a stream
    """
    
    infile = open_stream(filename)
    dct = {}
    
    for line in infile:
        tokens = line.rstrip("\n").split(delim)
        assert len(tokens) >= 2, line
        dct[keytype(tokens[0])] = valtype(tokens[1])
    
    return dct
readDict = read_dict

def write_list(filename, lst):
    """Write a list of anything (ints, floats, strings, etc) to a file.
    
       filename may also be a stream
    """
    out = open_stream(filename, "w")
    for i in lst:
        print >>out, i
writeVector = write_list


def write_dict(filename, dct, delim="\t"):
    """Write a dictionary to a file"""
    
    out = open_stream(filename, "w")
    for k, v in dct.iteritems():
        out.write("%s%s%s\n" % (str(k), delim, str(v)))
writeDict = write_dict


'''
def makeReopenStream(stream):
    """Object used to wrap a stream that is 'opened' multiple times.
       Will ignore first close"""
    
    closecount = [0]
    
    # save old close
    old_close = stream.close
    
    def new_close():
        closecount[0] += 1
        if closecount[0] > 1:
            old_close()
    
    # dynamically replace close function
    stream.close = new_close
'''


# TODO: add code for multiple close() calls
def open_stream(filename, mode = "r"):
    """Returns a file stream depending on the type of 'filename' and 'mode'
    
       The following types for 'filename' are handled:
       
       stream         - returns 'filename' unchanged
       iterator       - returns 'filename' unchanged
       URL string     - opens http pipe
       '-'            - opens stdin or stdout, depending on 'mode'
       other string   - opens file with name 'filename'
       
       mode is standard mode for file(): r,w,a,b
    """
    
    # if filename has a file interface then return it back unchanged
    if hasattr(filename, "read") or \
       hasattr(filename, "write"):
        return filename
    
    # if mode is reading and filename is an iterator
    if "r" in mode and hasattr(filename, "next"):
        return filename
    
    # if filename is a string then open it
    elif isinstance(filename, str):
        # open URLs
        if filename.startswith("http://"):
            import urllib2
            return urllib2.urlopen(filename)
        
        # open stdin and stdout
        elif filename == "-":
            if "w" in mode:
                return sys.stdout
            elif "r" in mode:
                return sys.stdin
            else:
                raise Exception("stream '-' can only be opened with modes r/w")
        
        # open regular file
        else:
            return file(filename, mode)
    
    # cannot handle other types for filename
    else:
        raise Exception("unknown filename type '%s'" % type(filename))



#=============================================================================
# Delimited files
#                

class DelimReader:
    """Reads delimited files"""

    def __init__(self, filename, delim="\t"):
        """Constructor for DelimReader
            
           arguments:
           filename  - filename or stream to read from
           delim     - delimiting character
        """
        
        self.infile = open_stream(filename)
        self.delim = delim
        
    def __iter__(self):
        return self
    
    def next(self):
        line = self.infile.next()
        return line.rstrip("\n").split(self.delim)


def read_delim(filename, delim=None):
    """Read an entire delimited file into memory as a 2D list"""
    
    return list(DelimReader(filename, delim))
readDelim = read_delim

def write_delim(filename, data, delim="\t"):
    """Write a 2D list into a file using a delimiter"""
    
    out = open_stream(filename, "w")
    for line in data:
        print >>out, delim.join(map(str, line))
writeDelim = write_delim

#=============================================================================
# printing functions
#

def default_justify(val):
    if isinstance(val, int) or \
       isinstance(val, float):
        return "right"
    else:
        return "left"


def default_format(val):
    if isinstance(val, int) and \
       not isinstance(val, bool):
        return int2pretty(val)
    elif isinstance(val, Percent):
        return str(val)
    elif isinstance(val, float):
        if abs(val) < 1e-4:
            return "%.2e" % val
        else:
            return "%.4f" % val
    else:
        return str(val)


def printcols(data, width=None, spacing=1, format=default_format, 
              justify=default_justify, out=sys.stdout,
              colwidth=INF, overflow="!"):
    """Prints a list or matrix in aligned columns
        
       data    - a list or matrix
       width   - maxium number of characters per line (default: 75 for lists)
       spacing - number of spaces between columns (default: 1)
       out     - stream to print to (default: sys.stdout)
    """
    
    if len(data) == 0:
        return
    
    if isinstance(data[0], list) or \
       isinstance(data[0], tuple):
        # matrix printing has default width of unlimited
        if width == None:
            width = 100000
        
        mat = data
    else:
        # list printing has default width 75
        if width == None:
            width = 75
        
        ncols = int(width / (max(map(lambda x: len(str(x)), data))+ spacing))
        mat = list2matrix(data, ncols=ncols, bycols=True)
    
    
    # turn all entries into strings
    matstr = map2(format, mat)
    
    # overflow
    for row in matstr:
        for j in xrange(len(row)):
            if len(row[j]) > colwidth:
                row[j] = row[j][:colwidth-len(overflow)] + overflow
    
    # ensure every row has same number of columns
    maxcols = max(map(len, matstr))
    for row in matstr:
        if len(row) < maxcols:
            row.extend([""] * (maxcols - len(row)))
    
    
    # find the maximum width char in each column
    maxwidths = map(max, map2(len, zip(* matstr)))
    
    
    # print out matrix with whitespace padding
    for i in xrange(len(mat)):
        fields = []
        for j in xrange(len(mat[i])):
            just = justify(mat[i][j])
            
            if just == "right":
                fields.append((" " * (maxwidths[j] - len(matstr[i][j]))) + \
                              matstr[i][j] + \
                              (" " * spacing))
            else:
                # do left by default            
                fields.append(matstr[i][j] + 
                              (" " * (maxwidths[j] - len(matstr[i][j]) + spacing)))
        out.write("".join(fields)[:width] + "\n")


def list2matrix(lst, nrows=None, ncols=None, bycols=True):
    """Turn a list into a matrix by wrapping its entries"""
    
    mat = []
    
    if nrows == None and ncols == None:
        nrows = int(math.sqrt(len(lst)))
        ncols = int(math.ceil(len(lst) / float(nrows)))
    elif nrows == None:
        nrows = int(math.ceil(len(lst) / float(min(ncols, len(lst)))))
    else:
        ncols = int(math.ceil(len(lst) / float(min(nrows, len(lst)))))

    for i in xrange(nrows):
        mat.append([])
        for j in xrange(ncols):
            if bycols:
                k = i + j*nrows
            else:
                k = i*ncols + j
            if k < len(lst):
                mat[-1].append(lst[k])
    
    return mat


def printwrap(text, width=80, prefix="", out=sys.stdout):
    """Prints text with wrapping"""
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



def int2pretty(num):
    """Returns a pretty-printed version of an int"""
    
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
    """Parses a pretty-printed version of an int into an int"""
    return int(string.replace(",", ""))


def str2bool(val):
    """Correctly converts the strings "True" and "False" to the 
       booleans True and False
    """
    
    if val == "True":
        return True
    elif val == "False":
        return False
    else:
        raise Exception("unknown string for bool '%s'" % val)



def print_dict(dic, key=lambda x: x, val=lambda x: x,
              num=None, cmp=cmp, order=None, reverse=False,
              spacing=4, out=sys.stdout,
              format=default_format, 
              justify=default_justify):
    """Print s a dictionary in two columns"""
    
    if num == None:
        num = len(dic)
    
    dic = mapdict(dic, key=key, val=val)
    items = dic.items()

    if order is not None:
        items.sort(key=order, reverse=reverse)
    else:
        items.sort(cmp, reverse=reverse)
    
    printcols(items[:num], spacing=spacing, out=out, format=format, 
              justify=justify)
printDict = print_dict


#=============================================================================
# Parsing
#  

class SafeReadIter:
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

def read_word(infile, delims = [" ", "\t", "\n"]):
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

def read_until(stream, chars):
    token = ""
    while True:
        char = stream.read(1)
        if char in chars or char == "":
            return token, char
        token += char

def read_while(stream, chars):
    token = ""
    while True:
        char = stream.read(1)
        if char not in chars or char == "":
            return token, char
        token += char

def skip_comments(infile):
    for line in infile:
        if line.startswith("#") or line.startswith("\n"):
            continue
        yield line



class IndentStream:
    """
    Makes any stream into an indent stream.
    
    Indent stream auto indents every line written to it
    """
    
    def __init__(self, stream):
        self.stream = open_stream(stream, "w")
        self.linestart = True
        self.depth = 0
    
    def indent(self, num=2):
        self.depth += num
    
    def dedent(self, num=2):
        self.depth -= num
        if self.depth < 0:
            self.depth = 0
    
    def write(self, text):
        lines = text.split("\n")
        
        for line in lines[:-1]:
            if self.linestart:
                self.stream.write(" "*self.depth)
                self.linestart = True
            self.stream.write(line + "\n")
        
        if len(lines) > 0:
            if text.endswith("\n"):
                self.linestart = True
            else:
                self.stream.write(" "*self.depth + lines[-1])
                self.linestart = False




    
    
#=============================================================================
# file/directory functions
#
def list_files(path, ext=""):
    """Returns a list of files in 'path' ending with 'ext'"""
    
    files = filter(lambda x: x.endswith(ext), os.listdir(path))
    files.sort()
    return [os.path.join(path, x) for x in files]
listFiles = list_files


def tempfile(path, prefix, ext):
    """Generates a a temp filename 'path/prefix_XXXXXX.ext'

    DEPRECATED: use this instead
    fd, filename = temporaryfile.mkstemp(ext, prefix)
    os.close(fd)
    """
    
    import warnings
    warnings.filterwarnings("ignore", ".*", RuntimeWarning)
    filename = os.tempnam(path, "____")      
    filename = filename.replace("____", prefix) + ext
    warnings.filterwarnings("default", ".*", RuntimeWarning)
    
    return filename


def deldir(path):
    """Recursively remove a directory"""
    
    # This function is slightly more complicated because of a 
    # strange behavior in AFS, that creates .__afsXXXXX files
    
    dirs = []
    
    def cleandir(arg, path, names):
        for name in names:
            filename = os.path.join(path, name)
            if os.path.isfile(filename):
                os.remove(filename)
        dirs.append(path)
    
    # remove files
    os.path.walk(path, cleandir, "")
    
    # remove directories
    for i in xrange(len(dirs)):
        # AFS work around
        afsFiles = list_files(dirs[-i])
        for f in afsFiles:
            os.remove(f)
        
        while True:
            try:
                if os.path.exists(dirs[-i]):
                    os.rmdir(dirs[-i])
            except Exception, e:
                continue
            break


def replace_ext(filename, oldext, newext):
    """Safely replaces a file extension new a new one"""
    
    if filename.endswith(oldext):
        return filename[:-len(oldext)] + newext
    else:
        raise Exception("file '%s' does not have extension '%s'" % (filename, oldext))


def makedirs(filename):
    """
    Makes a path of directories.
    Does not fail if filename already exists
    """

    if not os.path.isdir(filename):
        os.makedirs(filename)


#=============================================================================
# sorting
#


def sortindex(lst, cmp=cmp, key=None, reverse=False):
    """Returns the sorted indices of items in lst"""
    ind = range(len(lst))
    
    if key is None:
        compare = lambda a, b: cmp(lst[a], lst[b])
    else:
        compare = lambda a, b: cmp(key(lst[a]), key(lst[b]))
    
    ind.sort(compare, reverse=reverse)
    return ind


def sortranks(lst, cmp=cmp, key=None, reverse=False):
    """Returns the ranks of items in lst"""
    return invperm(sortindex(lst, cmp, key, reverse))
    

def sort_many(lst, *others, **args):
    """Sort several lists based on the sorting of 'lst'"""

    args.setdefault("reverse", False)

    if "key" in args:    
        ind = sortindex(lst, key=args["key"], reverse=args["reverse"])
    elif "cmp" in args:
        ind = sortindex(lst, cmp=args["cmp"], reverse=args["reverse"])
    else:
        ind = sortindex(lst, reverse=args["reverse"])
    
    lsts = [mget(lst, ind)]
    
    for other in others:
        lsts.append(mget(other, ind))
    
    return lsts


def invperm(perm):
    """Returns the inverse of a permutation 'perm'"""
    inv = [0] * len(perm)
    for i in range(len(perm)):
        inv[perm[i]] = i
    return inv

   

#=============================================================================
# histograms, distributions
#

def one_norm(vals):
    """Normalize values so that they sum to 1"""
    s = float(sum(vals))
    return [x/s for x in vals]


def bucketSize(array, ndivs=None, low=None, width=None):
    """Determine the bucket size needed to divide the values in array into 
       'ndivs' evenly sized buckets"""
    
    if low is None:
        low = min(array)
    
    if ndivs is None:
        if width is None:
            ndivs = 20
        else:
            ndivs = int(math.ceil(max((max(array) - low) / float(width), 1)))
    
    if width is None:
        width = (max(array) - low) / float(ndivs)
    
    return ndivs, low, width


def bucketBin(item, ndivs, low, width):
    """
    Return the bin for an item
    """
    
    assert item >= low, Exception("negative bucket index")
    return min(int((item - low) / width), ndivs-1)


def bucket(array, ndivs=None, low=None, width=None, key=lambda x: x):
    """Group elements of 'array' into 'ndivs' lists"""

    keys = map(key, array)

    # set bucket sizes
    ndivs, low, width = bucketSize(keys, ndivs, low, width)
    
    # init histogram
    h = [[] for i in range(ndivs)]
    x = []
    
    # bin items
    for i in array:
        if i >= low:
            h[bucketBin(key(i), ndivs, low, width)].append(i)
    for i in xrange(ndivs):
        x.append(i * width + low)
    return (x, h)


def hist(array, ndivs=None, low=None, width=None):
    """Create a histogram of 'array' with 'ndivs' buckets"""
    
    # set bucket sizes
    ndivs, low, width = bucketSize(array, ndivs, low, width)
    
    # init histogram
    h = [0] * ndivs
    x = []
    
    # count items
    for i in array:
        if i >= low:
            j = bucketBin(i, ndivs, low, width)
            if j < ndivs:
                h[j] += 1
    for i in xrange(ndivs):
        x.append(i * width + low)
    return (x, h)


def hist2(array1, array2, 
          ndivs1=None, ndivs2=None,
          low1=None, low2=None,
          width1=None, width2=None):
    """Perform a 2D histogram"""
    
    
    # set bucket sizes
    ndivs1, low1, width1 = bucketSize(array1, ndivs1, low1, width1)
    ndivs2, low2, width2 = bucketSize(array2, ndivs2, low2, width2)
    
    # init histogram
    h = [[0] * ndivs1 for i in xrange(ndivs2)]
    labels = []
    
    for j,i in zip(array1, array2):
        if j > low1 and i > low2:
            h[bucketBin(i, ndivs2, low2, width2)] \
             [bucketBin(j, ndivs1, low1, width1)] += 1
    
    for i in range(ndivs2):
        labels.append([])
        for j in range(ndivs1):        
            labels[-1].append([j * width1 + low1,
                               i * width2 + low2])
    return labels, h
    

def histbins(bins):
    """Adjust the bins from starts to centers, this will allow GNUPLOT to plot
       histograms correctly"""
    
    bins2 = []
    
    if len(bins) == 1:
        bins2 = [bins[0]]
    else:
        for i in range(len(bins) - 1):
            bins2.append((bins[i] + bins[i+1]) / 2.0)
        bins2.append(bins[-1] + (bins[-1] - bins[-2]) / 2.0)
    
    return bins2
    

def distrib(array, ndivs=None, low=None, width=None):
    """Find the distribution of 'array' using 'ndivs' buckets"""
    
    # set bucket sizes
    ndivs, low, width = bucketSize(array, ndivs, low, width)
    
    h = hist(array, ndivs, low, width)
    area = 0
    
    total = float(sum(h[1]))
    return (h[0], map(lambda x: (x/total)/width, h[1]))


def hist_int(array):
    """Returns a histogram of integers as a list of counts"""
    
    hist = [0]  * (max(array) + 1)
    negative = []
    for i in array:
        if (i >= 0):
            hist[i] += 1
        else:
            negative.append(i)
    return hist
histInt = hist_int

def hist_dict(array):
    """Returns a histogram of any items as a dict.
    
       The keys of the returned dict are elements of 'array' and the values
       are the counts of each element in 'array'.
    """
    
    hist = {}
    for i in array:
        if i in hist:
            hist[i] += 1
        else:
            hist[i] = 1
    return hist
histDict = hist_dict


def print_hist(array, ndivs=20, low=None, width=None,
              cols=75, spacing=2, out=sys.stdout):
    data = list(hist(array, ndivs, low=low, width=width))
    
    # find max bar
    maxwidths = map(max, map2(compose(len, str), data))
    maxbar = cols- sum(maxwidths) - 2 * spacing
    
    # make bars
    bars = []
    maxcount = max(data[1])
    for count in data[1]:
        bars.append("*" * int(count * maxbar / float(maxcount)))
    data.append(bars)
    
    printcols(zip(* data), spacing=spacing, out=out)
printHist = print_hist




# import common functions from other files, 
# so that only util needs to be included

try:
    from rasmus.timer import *
except ImportError:
    try:
        from timer import *
    except ImportError:
        pass

try:
    from rasmus.vector import *
except ImportError:
    try:
        from vector import *
    except ImportError:
        pass


try:
    from rasmus.options import *
except ImportError:
    try:
        from options import *
    except ImportError:
        pass

    
try:
    from rasmus.plotting import *
except Exception, e:
    try:
        from plotting import *
    except ImportError:
        pass




