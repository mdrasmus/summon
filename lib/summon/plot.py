from summon.core import *
import os


def plotList(arr):
    pts = []
    for i in range(len(arr)):
        pts.append(i)
        pts.append(arr[i])
    return group(line_strip(* pts))

def plotFunc(func, start, end, step):
    pts = []
    i = start
    while i < end:
        pts.append(i)
        pts.append(func(i))
        i += step
    return group(line_strip(* pts))



class ScatterPlot (object):
    def __init__(self, scale=[1.0, 1.0], onClick=None):
        self.data = []
        self.scale = scale
        self.selgroup = None
        
        if onClick != None:
            self.onClick = onClick
        
    
    def plot(self, x, y, names, col=[0,0,0], style="points", size=1.0):
        self.data.append(util.Bundle(x=x, y=y, 
                                     names=names, 
                                     color=col,
                                     style=style,
                                     size=size))
    
    
    def show(self):
        self.win = summon.Window()
        self.win.set_bgcolor(1, 1, 1)
        
        self.win.add_group(group(scale(self.scale[0], self.scale[1],
                                       self.draw())))
        
        self.win.reset_binding(input_key('d'), self.clearSelection)
        
        minx = util.INF
        maxx = -util.INF
        miny = util.INF
        maxy = -util.INF
            
        for dat in self.data:
            minx = min(minx, min(dat.x))
            maxx = max(maxx, max(dat.x))
            miny = min(miny, min(dat.y))
            maxy = max(maxy, max(dat.y))
        self.win.set_visible(minx * self.scale[0], miny * self.scale[1], 
                             maxx * self.scale[0], maxy * self.scale[1])
    
        
    def draw(self):
        vis = []
        
        minx = util.INF
        maxx = -util.INF
        miny = util.INF
        maxy = -util.INF        
        
        # draw data
        for dat in self.data:
            minx = min(minx, min(dat.x))
            maxx = max(maxx, max(dat.x))
            miny = min(miny, min(dat.y))
            maxy = max(maxy, max(dat.y))
        
        
            if dat.style == "points":
                vis2 = [color(* dat.color)]

                for x, y in zip(dat.x, dat.y):
                    vis2.extend([x, y])
                vis.append(points(* vis2))
            
            elif dat.style == "diamonds":
                vis2 = [color(* dat.color)]
                vis3 = []
                
                for i in xrange(len(dat.x)):
                    x = dat.x[i]
                    y = dat.y[i]
                    size = dat.size
                
                    vis2.extend([x + size, y,
                                 x       , y + size,
                                 x - size, y,
                                 x       , y - size])
                    vis3.extend([x, y + size, x, y - size])
                vis.append(quads(*vis2))
                vis.append(lines(*vis3))
        
        # add hotspot
        width = maxx * self.scale[0] - minx * self.scale[0]
        height = maxy * self.scale[1] - miny * self.scale[1]
        vis.append(hotspot("click", minx * self.scale[0] - width*.1, 
                                    miny * self.scale[1] - height*.1, 
                                    maxx * self.scale[1] + height*.1, 
                                    maxy * self.scale[1] + height*.1, 
                                    self.clickCallback))
    
        self.selgroup = group()
        vis.append(group(self.selgroup))
            
        return group(*vis)
    
    
    def clickCallback(self):
        x, y = self.win.get_mouse_pos('world')
        x /= float(self.scale[0])
        y /= float(self.scale[1])
        
        closest_dist = util.INF
        closest = None
        
        for dat in self.data:
            x2 = dat.x
            y2 = dat.y
        
            for i in xrange(len(dat.x)):
                dist = math.sqrt((x2[i] - x)**2 + (y2[i] - y)**2)
                
                if dist < closest_dist:
                    closest_dist = dist
                    closest = (dat, i)
        
        if closest != None:
            dat, i = closest
            x, y, size = dat.x[i], dat.y[i], dat.size
            self.onClick(x, y, dat.names[i])
            
            self.win.insert_group(self.selgroup, 
                                  self.drawSelect(x, y, size, col=[1, 0, 0]))

    def drawSelect(self, x, y, size, col):
        return group(color(* col), 
                     line_strip(x - size, y - size,
                                x - size, y + size,
                                x + size, y + size,
                                x + size, y - size,
                                x - size, y - size))

    def onClick(self, x, y, name):
        print name, x, y
        

    def clearSelection(self):
        self.selgroup = self.win.replace_group(self.selgroup, group())

    
    def select(self, func, col=[0, 0, 1]):
        vis = []
        sel = []
    
        for dat in self.data:
            x = dat.x
            y = dat.y
            names = dat.names
            size = dat.size
            
            for i in xrange(len(dat.x)):
                if func(x[i], y[i], names[i]):
                    sel.append([x[i], y[i], names[i]])
                    vis.append(self.drawSelect(x[i], y[i], size, col=col))
        
        if len(sel) > 0:
            self.win.insert_group(self.selgroup, group(*vis))
        
        return sel
