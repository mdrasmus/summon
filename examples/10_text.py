#!/usr/bin/python -i
# SUMMON examples
# 10_text.py - example of text
#
# Try zoomin in and out so see affects on text.  Use CTRL-right drag and 
# SHIFT-right drag to zoom and x and y axis separately.
#


# make summon commands available
from summon.core import *
import summon


win = summon.Window("10_text")

width = 100
a = 0
b = width * (1/3.0)
c = width * (2/3.0)
d = width



##################
# Bitmap Text
#
# Always the same on-screen size.  Clips when text cannot fit in bounding box
#

x = 0
y = 0

win.add_group(group(line_strip(color(1,1,1), 
    x, y,
    x + width, y,
    x + width, y + width,
    x, y + width,
    x, y)))

win.add_group(group(
    translate(x, y,
        text("NW", a, c, b, d, "top", "left"),
        text("N",  b, c, c, d, "top", "center"),
        text("NE", c, c, d, d, "top", "right"),
        text("W",  a, b, b, c, "middle", "left"),
        text("X",  b, b, c, c, "middle", "center"),
        text("E",  c, b, d, c, "middle", "right"),
        text("SW", a, a, b, b, "bottom", "left"),
        text("S",  b, a, c, b, "bottom", "center"),
        text("SE", c, a, d, b, "bottom", "right"))))


##################
# Scale Text
#
# Text is drawn with lines.  Always visible, scales to fit as large as 
# possible inside bounding box
#

x = 110
y = 0

win.add_group(group(line_strip(color(1,1,1), 
    x, y,
    x + width, y,
    x + width, y + width,
    x, y + width,
    x, y)))

win.add_group(group(
    translate(x, y,
        text_scale("NW", a, c, b, d, "top", "left"),
        text_scale("N",  b, c, c, d, "top", "center"),
        text_scale("NE", c, c, d, d, "top", "right"),
        text_scale("W",  a, b, b, c, "middle", "left"),
        text_scale("X",  b, b, c, c, "middle", "center"),
        text_scale("E",  c, b, d, c, "middle", "right"),
        text_scale("SW", a, a, b, b, "bottom", "left"),
        text_scale("S",  b, a, c, b, "bottom", "center"),
        text_scale("SE", c, a, d, b, "bottom", "right"))))

##################
# Clip Text
#
# Similar to Scale Text except it has a minimum and maximum height.  Also
# text will not distort when x and y axis are zoomed independently.
#

x = 220
y = 0
minsize = 10
maxsize = 33

win.add_group(group(line_strip(color(1,1,1), 
    x, y,
    x + width, y,
    x + width, y + width,
    x, y + width,
    x, y)))

win.add_group(group(
    translate(x, y,
        text_clip("NW", a, c, b, d, minsize, maxsize, "top", "left"),
        text_clip("N",  b, c, c, d, minsize, maxsize, "top", "center"),
        text_clip("NE", c, c, d, d, minsize, maxsize, "top", "right"),
        text_clip("W",  a, b, b, c, minsize, maxsize, "middle", "left"),
        text_clip("X",  b, b, c, c, minsize, maxsize, "middle", "center"),
        text_clip("E",  c, b, d, c, minsize, maxsize, "middle", "right"),
        text_clip("SW", a, a, b, b, minsize, maxsize, "bottom", "left"),
        text_clip("S",  b, a, c, b, minsize, maxsize, "bottom", "center"),
        text_clip("SE", c, a, d, b, minsize, maxsize, "bottom", "right"))))



# center the "camera" so that all shapes are in view
win.home()
