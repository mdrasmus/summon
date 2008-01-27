#!/usr/bin/env python-i



import summon
from summon.core import *
from summon import shapes, colors

win = summon.Window()

win.add_group(
    group(colors.white,
          shapes.round_box(0, 0, 100, 100, 20, fill=False),
          shapes.box(0, 0, 100, 100, fill=False)))

win.add_group(translate(200, 0,
    colors.red,
    shapes.round_box(0, 0, 100, 100, [20, 20, 40, 40])))

win.add_group(translate(400, 0,
    colors.white,
    shapes.message_bubble(0, 0, 150, 100,
        group(colors.blue,
            text("hello world", 0, 0, 150, 100, "center", "middle")))))

win.add_group(translate(400, -400,
    colors.white,
    shapes.message_bubble(0, 0, 150, 20,
        group(colors.blue,
            text("hello world", 0, 0, 150, 20, "center", "middle")))))
     
    
win.home()
