# SUMMON examples
# hotspots.py - example of hotspots

# The default binding for hotspot clicking is 'control + left click'
# Left click on each box with the control key pressed to activate the hotspots
# Notice that when two hotspots overlap that both are activated when clicked

# make summon commands available
from summon import *


# clear the screen of all drawing
clear_groups()


def makeBox(string, x, y, width, boxColor):
    def callback():
        print "HOTSPOT '%s' was clicked!" % string

    return group(
        quads(boxColor, vertices(
            x, y ,
            x + width, y,
            x + width, y + width,
            x, y + width)),
        line_strip(color(1,1,1), vertices(
            x, y ,
            x + width, y,
            x + width, y + width,
            x, y + width,
            x, y)),
        text_scale(string,
            x + width*.1, y + width*.1,
            x + width*.9, y + width*.9,
            "center", "middle"),
        color(1,1,1),
        hotspot("click",
            x, y,
            x + width, y + width,
            callback))

red = color(1,0,0, .4)
blue = color(0,0,1, .4)
green = color(0,1,0, .4)

# add some boxes with hotspots
add_group(makeBox("A", 0, 0, 10, red))
add_group(makeBox("B", 15, 0, 10, blue))
add_group(makeBox("C", 15, 15, 10, green))
add_group(makeBox("D", 30, 15, 10, green))
add_group(makeBox("E", 35, 20, 30, red))


# center the "camera" so that all shapes are in view
home()
