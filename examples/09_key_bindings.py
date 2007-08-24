#!/usr/bin/env python-i
# SUMMON examples
# 09_key_bindings.py - example use of bindings
#
# Also see summon/lib/summon_config.py file for example use of set_binding() 
# and input_*(). 
#


from summon.core import *
from summon import shapes
import summon

win = summon.Window("09_key_bindings")

# color background boxes
#   save the group id of each box so we can manipulate it later
groupa = win.add_group(group(color(1,0,0), shapes.box(0,0,200,-50)))
groupb = win.add_group(group(color(0,1,0), shapes.box(0,-50,200,-100)))
groupc = win.add_group(group(color(0,0,1), shapes.box(0,-100,200,-150)))

# text
win.add_group(group(color(1,1,1), text_scale("press 'a'", 0, 0, 200, -50)))
win.add_group(group(color(1,1,1), text_scale("press 'b'", 0, -50, 200, -100)))
win.add_group(group(color(1,1,1), text_scale("press 'c'", 0, -100, 200, -150)))

win.add_group(shapes.boxStroke(0,0,200,-150))
win.home()


# variable to keep track of the visibility of colored boxes
show_groupa = True
show_groupb = True
show_groupc = True


# functions that can be bound to keys
def func_a():
    global show_groupa
    show_groupa = not show_groupa
    win.show_group(groupa, show_groupa)

def func_b():
    global show_groupb
    show_groupb = not show_groupb
    win.show_group(groupb, show_groupb)

def func_c():
    global show_groupc
    show_groupc = not show_groupc
    win.show_group(groupc, show_groupc)


# use summon to bind python functions to key presses
win.set_binding(input_key("a"), func_a)
win.set_binding(input_key("b"), func_b)
win.set_binding(input_key("c"), func_c)

