#!/usr/bin/env python-i
# SUMMON examples
# 04_hotspots.py - example of hotspots

# The default binding for hotspot clicking is 'left click'
# Left click inside each box to activate the hotspots
# Notice that when two hotspots overlap that both are activated when clicked

# make summon commands available
from summon.core import *
import summon

# create a new window
win = summon.Window("04_hotspots")


def makeButton(string, x, y, width, boxColor):
    def callback():
        print "HOTSPOT '%s' was clicked!" % string

    return group(
        # color rectangle
        quads(boxColor, 
            x, y ,
            x + width, y,
            x + width, y + width,
            x, y + width),
        
        # white border of rectangle
        line_strip(color(1,1,1), 
            x, y ,
            x + width, y,
            x + width, y + width,
            x, y + width,
            x, y),
        
        # place text within the rectangle
        text_scale(string,
            x + width*.1, y + width*.1,
            x + width*.9, y + width*.9,
            "center", "middle"),
        
        # define an imaginary rectangle that when clicked will 
        # call the function 'callback'
        hotspot("click",
            x, y,
            x + width, y + width,
            callback))

# define three colors
red = color(1, 0, 0, .4)
blue = color(0, 0, 1, .4)
green = color(0, 1, 0, .4)

# add some boxes with hotspots
win.add_group(makeButton("A", 0, 0, 10, red))
win.add_group(makeButton("B", 15, 0, 10, blue))
win.add_group(makeButton("C", 15, 15, 10, green))
win.add_group(makeButton("D", 30, 15, 10, green))
win.add_group(makeButton("E", 35, 20, 30, red))


# center the "camera" so that all shapes are in view
win.home()
