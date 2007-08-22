#!/usr/bin/env summon
# SUMMON examples
# 03_basics.py - more complex building before add_group()



# a function for drawing a commonly used shape
def arrow():
    white = color(1,1,1)
    red  = color(1,0,0)
    blue = color(0,0,1)
    
    return group(
        quads(white, 0, 0,        # right fin
              red,   40, -40,
              red,   40, -80,
              white, 0, -40,
              
              white, 0,0,         # left fin
              blue,  -40, -40,
              blue,  -40, -80,
              white, 0, -40),
              
              lines(white, 0,0, 0, 100)) # shaft


def drawArrows():
    # start a list of drawing elements that we will add to
    vis = []
    
    # add several arrows to our list, each translated and rotated differently
    for x in range(0, 10):
        vis.append(
            translate(x*100, 0, 
                rotate(x*-10, arrow())))

    # now convert our list to a single group!
    g = group(*vis)
    
    # return the group to who ever wants it
    return g


# change the background color to dark green
set_bgcolor(0, .5, 0)


# add a row of arrows
add_group(drawArrows())

# draw row of arrows flipped!
add_group(group(
    translate(0, -200, 
        flip(100, 0, drawArrows()))))


# center the "camera" so that all shapes are in view
home()
