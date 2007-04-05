#!/usr/bin/env summon
# SUMMON examples
# bindings.py - example use of bindings
#
# Also see summon/lib/summon_config.py file for example use of set_binding() 
# and input_*(). 
#


from summon import *
from summonlib import shapes

# color background boxes
#   save the group id of each box so we can manipulate it later
groupa = add_group(group(color(1,0,0), shapes.box(0,0,200,-50)))
groupb = add_group(group(color(0,1,0), shapes.box(0,-50,200,-100)))
groupc = add_group(group(color(0,0,1), shapes.box(0,-100,200,-150)))

# text
add_group(group(color(1,1,1), text_scale("press 'a'", 0, 0, 200, -50)))
add_group(group(color(1,1,1), text_scale("press 'b'", 0, -50, 200, -100)))
add_group(group(color(1,1,1), text_scale("press 'c'", 0, -100, 200, -150)))

add_group(shapes.boxStroke(0,0,200,-150))
home()


# variable to keep track of the visibility of colored boxes
show_groupa = True
show_groupb = True
show_groupc = True


# functions that can be bound to keys
def func_a():
    global show_groupa
    show_groupa = not show_groupa
    show_group(groupa, show_groupa)

def func_b():
    global show_groupb
    show_groupb = not show_groupb
    show_group(groupb, show_groupb)

def func_c():
    global show_groupc
    show_groupc = not show_groupc
    show_group(groupc, show_groupc)


# use summon to bind python functions to key presses
set_binding(input_key("a"), func_a)
set_binding(input_key("b"), func_b)
set_binding(input_key("c"), func_c)

