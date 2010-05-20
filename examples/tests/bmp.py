#!/usr/bin/env python-i

import sys, os, time

import summon
from summon.core import *
from summon import shapes, colors



vis = group(colors.red, 
            shapes.box(0, 0, 100, 100),
            colors.white,
            text("hello", 0, 0, 100, 100),

            translate(0, 200,
                      colors.orange, 
                      shapes.box(0, 0, 100, 100),
                      colors.white,
                      text_scale("hello", 0, 0, 100, 100),
            ),

            translate(300, 300,
                      colors.purple, 
                      shapes.box(0, 0, 200, 200),
                      colors.white,
                      text_clip("hello", 0, 0, 200, 200),
            ),
            
            translate(200, 200,
                zoom_clamp(
                    colors.green, 
                    shapes.box(0, 0, 100, 100),
                    colors.white,
                    text("hello", 0, 0, 100, 100),
                    maxx=.5, maxy=.5
                )
            )
        )



win = summon.Window("bmp", size=(403, 400))
win.add_group(vis)
time.sleep(1)


win.screenshot("out.bmp")
os.system("display out.bmp")

