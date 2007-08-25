#!/usr/bin/python -i
#!/usr/bin/env python-i
# SUMMON examples
# 05_hotspots.py - example of hotspots

# The default binding for hotspot clicking is 'middle click'
# Middle click inside each box to activate the hotspots
# Notice that when two hotspots overlap that both are activated when clicked

# make summon commands available
from summon.core import *
import summon

win = summon.Window("05_hotspots.py")


def makeBox(string, x, y, width, boxColor):
    
    # create color quad separately
    # this will allow us to refer to it directly
    colorsquare = quads(boxColor, 
                        x, y ,
                        x + width, y,
                        x + width, y + width,
                        x, y + width)
    
    # get the groupid of the color quad so we can manipulate it in callback()
    # use myVars to keep track of the visibility of the color quad
    myVars = {"visible": True}
    
    def callback():
        print "HOTSPOT '%s' was clicked!" % string
        
        # toggle the visibility of a drawing element
        myVars["visible"] = not myVars["visible"]
        win.show_group(colorsquare, myVars["visible"])

    
    return  group(colorsquare,
                  line_strip(color(1, 1, 1), 
                      x, y ,
                      x + width, y,
                      x + width, y + width,
                      x, y + width,
                      x, y),
                  text_scale(string,
                      x + width*.1, y + width*.1,
                      x + width*.9, y + width*.9,
                      "center", "middle"),
                  color(1, 1, 1),
                  hotspot("click",
                      x, y,
                      x + width, y + width,
                      callback))

# define three colors
red = color(1, 0, 0, .4)
blue = color(0, 0, 1, .4)
green = color(0, 1, 0, .4)

# add some boxes with hotspots
win.add_group(makeBox("A", 0, 0, 10, red))
win.add_group(makeBox("B", 15, 0, 10, blue))
win.add_group(makeBox("C", 15, 15, 10, green))
win.add_group(makeBox("D", 30, 15, 10, green))
win.add_group(makeBox("E", 35, 20, 30, red))

# center the "camera" so that all shapes are in view
win.home()
