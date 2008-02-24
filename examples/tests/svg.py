#!/usr/bin/env python-i

import sys, os

import summon
from summon.core import *
from summon import svg, shapes, colors



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

svg.writeSvg(open("out.svg", "w"), 
             vis, visible=(0.0, 0.0, 500.0, 500.0), size=(500, 500), 
             bgcolor=(0, 0, 0), win=None)

if "win" in sys.argv:
    win = summon.Window("svg", size=(500, 500))
    win.add_group(vis)
else:
    os.system("display out.svg")
    sys.exit()
