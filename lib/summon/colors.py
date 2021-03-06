"""

    Summon - colors modules

"""

import copy

from summon.core import *

# color constants
red    = color(1, 0, 0)
orange = color(1, .5, 0)
yellow = color(1, 1, 0)
green  = color(0, 1, 0)
blue   = color(0, 0, 1)
purple = color(1, 0, 1)
white  = color(1, 1, 1)
black  = color(0, 0, 0)
grey   = color(.5, .5, .5)
brown  = color(.7, .4, 0)


class ColorMap:
    """ColorMap maps values on the real line to colors"""
    
    
    def __init__(self, table=[]):
        """
        'table' should be the following format:
        
        [
          [val1, color1],
          [val2, color2],
          [val3, color3],
          ...etc..
        ]
        
        Values bewteen val1 and val2 will be assigned a blend of 
        color1 and color2.  value-color pairs can be specified in any order 
        within table.
        
        """
        self.table = copy.copy(table)
        
        self.table.sort(key=lambda x: x[0])
    
    
    def get(self, value):
        """Returns values in [0, 1]"""
        value = float(value)
    
        # determine where color falls in table
        for i in xrange(len(self.table)):
            if value <= self.table[i][0]:
                break
        if i > 0:
            i -= 1
        
        
        if value <= self.table[i][0]:
            # return lower bound color
            return self.table[i][1]
        elif value >= self.table[i+1][0]:
            # return upper bound color
            return self.table[i+1][1]
        else:
            # blend two nearest colors
            part = value - self.table[i][0]
            tot = float(self.table[i+1][0] - self.table[i][0])
            weight1 = (tot-part)/tot
            weight2 = part/tot
            
            newcolor = []
            color1 = self.table[i][1]
            color2 = self.table[i+1][1]
            for j in range(len(color1)):
                newcolor.append(weight1 * color1[j] + 
                                weight2 * color2[j])
            
            return newcolor
    
    
    def get_int(self, value):
        return [int(x*255) for x in self.get(value)]



class RainbowColorMap:
    def __init__(self, maxv=1.0, minv=0.0):
        self.max = maxv
        self.min = minv
        self.range = maxv - minv
        if self.range == 0.0:
            self.range = 1.0
    
    def get(self, val):
        x = (float(val) - self.min) / self.range
        return [-2 + 4*x, 2 - 4*abs(.5-x), 2 - 4*x]
    

class PosNegColorMap:
    def __init__(self, minv=-1.0, maxv=1.0):
        self.max = maxv
        self.min = minv
    
    def get(self, val):
        if val >= 0:
            return [1, 0, 0, float(val)/self.max]
        else:
            return [0, 1, 0, float(val)/self.min]

class SolidColorMap:
    def __init__(self, maxv=1.0, minv=0.0):
        self.max = maxv
        self.min = minv
        self.range = maxv - minv
    
    def get(self, val):
        return [1, 1, 1]


def read_color_map(filename):
    mat = []

    for line in file(filename):
        value, red, green, blue = map(float, line.rstrip().split())
        mat.append([value, [red, green, blue, 1.0]])
        
    try:
        return ColorMap(mat)
    except:
        print "error reading colormap, using default"
        return RainbowColorMap()

