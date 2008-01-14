#!/usr/bin/python -i
#!/usr/bin/env python-i
# SUMMON examples
# 05_hotspots.py - example of hotspots

# The default binding for hotspot clicking is 'left click'
# Left click inside each box to activate the hotspots
# Notice that when two hotspots overlap that both are activated when clicked

# make summon commands available
from summon.core import *
import summon
from summon import shapes, colors

win = summon.Window("05_hotspots.py")


def makeButton(string, x, y, width, boxColor, kind):
    
    # use my_vars to keep track of the visibility of the color quad
    my_vars = {"visible": True}
    
    def callback():
        print "HOTSPOT '%s' was clicked!" % string
        
        # toggle the visibility of a drawing element
        my_vars["visible"] = not my_vars["visible"]
        win.show_group(shape_color, my_vars["visible"])
    
    
    if kind == "square":
        # create color quad separately
        # this will allow us to refer to later to toggle its visibility
        shape_color = group(boxColor, 
                            shapes.box(x, y, x + width, y + width))
    
        return  group(shape_color,
                      colors.white,
                      shapes.box(x, y, x + width, y + width, fill=False),
                      text_scale(string,
                          x + width*.1, y + width*.1,
                          x + width*.9, y + width*.9,
                          "center", "middle"),
                      color(1, 1, 1),
                      hotspot("click",
                          x, y,
                          x + width, y + width,
                          callback))
    
    elif kind == "circle":
        # create color shape separately
        # this will allow us to refer to later to toggle its visibility
        shape_color = group(boxColor, 
                            shapes.regular_polygon(x, y, 50, width))
    
        return  group(shape_color,
                      colors.white,
                      shapes.regular_polygon(x, y, 50, width, fill=False),
                      text_scale(string,
                          x - width*.7, y - width*.7,
                          x + width*.7, y + width*.7,
                          "center", "middle"),
                      color(1, 1, 1),
                      hotspot_circle("click",
                          x, y, width,
                          callback))
                          
    elif kind == "polygon":
        # create color shape separately
        # this will allow us to refer to later to toggle its visibility
        pts = [0, 0, 30, 100, 50, 100, 80, 0]
        shape_color = group(boxColor, polygon(*pts))
    
        return  translate(x, y,
                    scale(.1, .1,
                      shape_color,
                      colors.white,
                      line_strip(80, 0, *pts),
                      text_scale(string,
                          0, 20, 80, 80,
                          "center", "middle"),
                      color(1, 1, 1),
                      hotspot_polygon("click",
                          pts,
                          callback)))

# define three colors
red = color(1, 0, 0, .4)
blue = color(0, 0, 1, .4)
green = color(0, 1, 0, .4)
yellow = color(1, 1, 0, .4)

# add some boxes with hotspots
win.add_group(makeButton("A", 0, 0, 10, red, "circle"))
win.add_group(makeButton("B", 15, 0, 10, blue, "square"))
win.add_group(makeButton("C", 15, 15, 10, yellow, "polygon"))
win.add_group(makeButton("D", 30, 15, 10, green, "square"))
win.add_group(makeButton("E", 35, 20, 30, red, "square"))

# center the "camera" so that all shapes are in view
win.home()
