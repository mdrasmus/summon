# SUMMON examples
# text.py - example of text

# make summon commands available
from summon import *


# clear the screen of all drawing
clear_groups()

x = 110
y = 0
width = 100
a = 0 
b = width * (1/3.0)
c = width * (2/3.0)
d = width

add_group(group(line_strip(color(1,1,1), 
    x, y,
    x + width, y,
    x + width, y + width,
    x, y + width,
    x, y)))


add_group(group(
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


x = 0
y = 0
width = 100
a = x 
b = x + width * (1/3.0)
c = x + width * (2/3.0)
d = x + width

add_group(group(line_strip(color(1,1,1), 
    x, y,
    x + width, y,
    x + width, y + width,
    x, y + width,
    x, y)))

add_group(group(
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



# center the "camera" so that all shapes are in view
home()
