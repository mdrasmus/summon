#!/usr/bin/python -i

from summon.core import *
from summon import shapes
import summon

from itertools import izip


NOVALUE = object()


win = summon.Window("object_graph")
win.set_bgcolor(1,1,1)

def makeObjHotspot(obj, x, y, width):
    def func():
        win.add_group(group(color(0,0,0),
                            shapes.boxStroke(x, y, x+width, y-1),
                            lines(x+width, y-.5, x+width+2, y-.5),
                            drawobj(obj, x+width+2, y)))
    return hotspot("click", x, y, x+width, y-1, func)



def drawobj(obj, x=0.0, y=0.0):
    if isinstance(obj, dict):
        keys = obj.keys()
        vals = obj.values()
    elif isinstance(obj, list) or isinstance(obj, tuple):
        keys = map(str, range(len(obj)))
        vals = obj
    elif hasattr(obj, "__dict__"):
        keys = obj.__dict__.keys()
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
            attr_vis.append(makeObjHotspot(val, x, y-1-i, width))
    
    return group(color(.5, .5, 1), shapes.box(x, y, x+width, y-1),
                 color(0, 0, 0),
                 text_clip(type_name, x, y, x+width, y-1, 4, 12, "left", "middle"),
                 color(.8, .8, 1), shapes.box(x, y-1, x+width, y-1-height),
                 *attr_vis)


x = {"a": 1, "b": win}
y = ['a', 'b', 'c', x]

win.add_group(drawobj(y))
win.home()
win.zoom(.9, .9)
