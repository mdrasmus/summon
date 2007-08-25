#!/usr/bin/env python-i

from summon.core import *
from summon import shapes
import summon

from itertools import izip


NOVALUE = object()

class VarDisplay (object):
    def __init__(self, obj, x=None, y=None, width=None, height=None):
        self.obj = obj
        self.x = x
        self.y = y
        self.width = width
        self.height = height

class ObjectGraph (object):
    def __init__(self):
        self.win = None
        self.vars = {}
    

    def show(self):
        self.win = summon.Window("object_graph")
        self.win.set_bgcolor(1,1,1)


    def makeObjHotspot(self, obj, x, y, width):
        def func():
            self.win.add_group(group(color(0,0,0),
                                     shapes.box(x, y, x+width, y-1, fill=False),
                                     lines(x+width, y-.5, x+width+2, y-.5),
                                     self.drawobj(obj, x+width+2, y)))
        return hotspot("click", x, y, x+width, y-1, func)



    def drawobj(self, obj, x=0.0, y=0.0):
        if id(obj) in self.vars:
            return group()
        else:
            self.vars[id(obj)] = VarDisplay(obj, x=x, y=y)
        
        if isinstance(obj, dict):
            keys = map(str, obj.keys())
            vals = obj.values()
        elif isinstance(obj, list) or isinstance(obj, tuple):
            keys = map(str, range(len(obj)))
            vals = obj
        elif hasattr(obj, "__dict__"):
            keys = map(str, obj.__dict__.keys())
            vals = obj.__dict__.values()
        else:
            keys = [str(obj)]
            vals = [NOVALUE]
        
        
        type_name = type(obj).__name__
        width = len(type_name)
        if len(keys) > 0:
            width = max(width, max(map(len, keys)))

        height = len(keys)

        attr_vis = []
        for i, (key, val) in enumerate(izip(keys, vals)):
            if val == NOVALUE:
                attr_vis.append(group(color(1, .8, .8),
                                      shapes.box(x, y-1-i, x+width, y-2-i)))

            attr_vis.append(color(0, 0, 0))
            attr_vis.append(text_clip(key, 
                            x, y-1-i, x+width, y-2-i, 
                            4, 12, "left", "middle"))
            if val != NOVALUE:
                attr_vis.append(self.makeObjHotspot(val, x, y-1-i, width))

        return group(color(.5, .5, 1), shapes.box(x, y, x+width, y-1),
                     color(0, 0, 0),
                     text_clip(type_name, x, y, x+width, y-1, 4, 12, "left", "middle"),
                     color(.8, .8, 1), shapes.box(x, y-1, x+width, y-1-height),
                     *attr_vis)
    
    def showObj(self, obj, x=0, y=0):
        self.win.add_group(self.drawobj(obj))




view = ObjectGraph()
view.show()

x = {"a": 1, "b": view}
y = ['a', 'b', 'c', x]

view.showObj(y)
view.win.home()
view.win.zoom(.9, .9)


