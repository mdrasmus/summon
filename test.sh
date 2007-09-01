#!/bin/sh


export PYTHONPATH=lib

cat > test.py <<EOF

import sys
from summon.core import *
import summon
from summon import svg



def func():
    win.remove_group(g)


def hello():
    print "hi"

l = lines(color(1,0,1), 0, 0, 100, 100)
g = group(l, 
        translate(100, 100, 
          scale(.5, .5, 
            flip(.5, .5,
            lines(color(1,1,1), 0, 100, 100, 0),
            hotspot("click", 0, 0, 100, 100, func),
            color(0, 1, 0, .5),
            text_clip("hello", 0, 0, 100, 100, 4, 20)))))

win = summon.Window()
win.add_group(g)

l2 = lines(100, 0, 0, 100)
g.append(l2)

g.remove(l2)

def walk(node, depth=0): 
    print "  " * depth, node, node.get_contents()
    for child in node:
        walk(child, depth+1)

#svg.writeSvg(win, sys.stdout, win.get_root())

EOF

python -i test.py
