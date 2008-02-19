#!/usr/bin/env python-i


from summon.core import *
import summon
from summon import shapes, colors


win = summon.Window()

win.add_group(shapes.box(0, 0, 50, 100, fill=False))
win.add_group(text_clip("hello", 0, 0, 50, 100, 4, 20, "bottom"))
win.add_group(group(color(1, 0, 0), text_clip("hello", 0, 0, 50, 100, 4, 20, "bottom")))



win.add_group(translate(300, 300, rotate(20,
    color(1,1,1),
    shapes.box(0, 0, 5000, 100, fill=False),
    text_clip("hello", 0, 0, 5000, 100, 4, 20, "bottom"),
    group(color(1, 0, 0), text_clip("hello", 0, 0, 5000, 100, 4, 20, "bottom")))))


win.add_group(translate(0, 300, rotate(90,
    color(1,1,1),
    shapes.box(0, 0, 50, 100, fill=False),
    text_clip("hello", 0, 0, 50, 100, 4, 20, "bottom"),
    group(color(1, 0, 0), text_clip("hello", 0, 0, 50, 100, 4, 20, "bottom")))))


win.add_group(translate(-300, 300, rotate(0,
    color(1,1,1),
    shapes.box(0, 0, 50, 100, fill=False),
    text_clip("hello", 0, 0, 50, 100, 4, 20, "bottom"),
    group(color(1, 0, 0), text_clip("hello", 0, 0, 50, 100, 4, 20, "bottom")))))


win.add_group(translate(-600, 300, rotate(10,
    color(1,1,1),
    shapes.box(0, 0, 50, 100, fill=False),
    text_clip("hello", 0, 0, 50, 100, 4, 20, "bottom"),
    group(color(1, 0, 0), text_clip("hello", 0, 0, 50, 100, 4, 20, "bottom")))))
