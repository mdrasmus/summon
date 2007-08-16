#!/bin/sh


export PYTHONPATH=lib

cat > test.py <<EOF

from summon.core import *
import summon




def func():
    win.remove_group(g)



l = lines(color(1,0,1), 0, 0, 100, 100)
g = group(l, 
        translate(100, 100, 
          scale(.5, .5, 
          lines(color(1,1,1), 0, 100, 100, 0),
          hotspot("click", 0, 0, 100, 100, func),
          color(0, 1, 0, .5),
          text_clip("hello", 0, 0, 100, 100, 4, 20))))

win = summon.Window()
win.add_group(g)
win.insert_group(g, group(lines(100, 0, 0, 200)))
win.add_group(group(lines(100, 0, 0, 300)))

q = win.replace_group(g, group(lines(100, 0, 0, 100)))
print q
q = win.replace_group(q, group(lines(100, 0, 0, 100)))
print q
q = win.replace_group(q, group(lines(100, 0, 0, 100)))
print q
q = win.replace_group(q, group(lines(100, 0, 0, 100)))
print q
q = win.replace_group(q, group(lines(100, 0, 0, 100)))
print q

win.set_binding(input_key("l"), func)

EOF

python -i test.py
