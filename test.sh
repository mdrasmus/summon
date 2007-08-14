#!/bin/sh


export PYTHONPATH=lib

cat > test.py <<EOF

import summon_core
from summon.core import color
import summon


GROUP = 1060
HOTSPOT = 1062

LINES = 1064

TEXT = 1072
TEXT_SCALE = 1073
TEXT_CLIP = 1074

TRANSLATE = 1078
ROTATE = 1079
SCALE = 1080
FLIP = 1081

class Construct:
    def __init__(self, constructid, code):
        try:
            self.ptr = summon_core.make_construct(constructid, code)
        except:
            self.ptr = None
            raise

    def __del__(self):
        if self.ptr != None:
            summon_core.delete_construct(self.ptr)

class group (Construct):
    def __init__(self, *code):
        Construct.__init__(self, GROUP, code)

class hotspot (Construct):
    def __init__(self, *code):
        Construct.__init__(self, HOTSPOT, code)


class lines (Construct):
    def __init__(self, *code):
        Construct.__init__(self, LINES, code)

class translate (Construct):
    def __init__(self, *code):
        Construct.__init__(self, TRANSLATE, code)

class scale (Construct):
    def __init__(self, *code):
        Construct.__init__(self, SCALE, code)

class text (Construct):
    def __init__(self, *code):
        Construct.__init__(self, TEXT, code)

class text_scale (Construct):
    def __init__(self, *code):
        Construct.__init__(self, TEXT_SCALE, code)

class text_clip (Construct):
    def __init__(self, *code):
        Construct.__init__(self, TEXT_CLIP, code)
        

def func():
    print "hello"



l = lines(color(1,0,1), 0, 0, 100, 100)
g = group(l, 
        translate(100, 100, 
          scale(.5, .5, 
          lines(color(1,1,1), 0, 100, 100, 0),
          hotspot("click", 0, 0, 100, 100, func),
          text_clip("hello", 0, 0, 100, 100, 4, 20))))

win = summon.Window()
print "group id", win.add_group(g)

EOF

python -i test.py
