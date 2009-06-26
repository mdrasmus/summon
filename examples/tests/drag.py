#!/usr/bin/env python-i


from summon.core import *
import summon
from summon import shapes



win = summon.Window()

win.add_group(shapes.box(0, 0, 100, 100))

win.add_group(shapes.draggable(
    # region that detects drags
    summon.Region(0, 0, 100, 100),
    # draw group
    group(color(1, 0, 0),
          shapes.round_box(0, 0, 100, 100, 10))))

win.add_group(translate(200, 200, rotate(30, 
    shapes.draggable(
        # region that detects drags
        summon.Region(0, 0, 100, 100),

        # draw group
        group(
            color(0, 0, 1),
            shapes.round_box(0, 0, 100, 100, 10),
            shapes.message_bubble(50, 50, 150, 40, 
                text("hello", 0, 0, 150, 40,
                     "middle", "center"),
                close_button=True))))))

                              
win.add_binding(input_click("left", "up", "shift"), "hotspot_drag_stop")
win.add_binding(input_click("left", "down", "shift"), "hotspot_drag_start")
win.add_binding(input_motion("left", "down", "shift"), "hotspot_drag")

