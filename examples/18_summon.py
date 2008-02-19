#!/usr/bin/env python-i

import math

import summon
from summon.core import *
from summon import shapes, colors

def interleave(a, b):
    c = []
    for i in xrange(0, len(a), 2):
        c.extend(a[i:i+2] + b[i:i+2])
    return c

def curve(x, y, start, end, radius, width):
    p = shapes.arc_path(x, y, start, end, radius, 30)
    p2 = shapes.arc_path(x, y, start, end, radius-width, 30)
    return triangle_strip(*interleave(p, p2))

def draw_u(top, bottom, w, t):
    return group(shapes.box(-w,top, -w+t, bottom+w),
                 shapes.box(w,top, w-t, bottom+w),                     
                 curve(0, bottom+w, -math.pi, 0.0, w, t))

def draw_summon():    
    t = 150
    w = 200
    s = 50
    top = w
    bottom = -3*w+t
    
    return translate(-7*w+t-2.5*s, -(top + bottom) / 2.0, 
        # S
        curve(0, 0, 0, 1.5*math.pi, w, t),
        curve(0, -2*w+t, -math.pi, .5*math.pi, w, t),

        # U
        translate(2*w+s, 0,
            draw_u(top, bottom, w, t)),

        # M
        translate(4*w+2*s, 0,
            translate(0, -2*w+t, 
                rotate(180, 
                    draw_u(top, bottom, w, t))),
            translate(2*w-t, -2*w+t, 
                rotate(180, 
                    draw_u(top, bottom, w, t)))),
        
        # M
        translate(8*w-t+3*s, 0,
            translate(0, -2*w+t, 
                rotate(180, 
                    draw_u(top, bottom, w, t))),
            translate(2*w-t, -2*w+t, 
                rotate(180, 
                    draw_u(top, bottom, w, t)))),
        
        # 0
        translate(12*w-2*t+4*s, 0,
            curve(0, 0, 0.0, math.pi, w, t),
            shapes.box(-w,top-w, -w+t, bottom+w),
            shapes.box(w,top-w, w-t, bottom+w),                     
            curve(0, bottom+w, -math.pi, 0.0, w, t)),
        
        # N
        translate(14*w-2*t+5*s, 0,
            translate(0, -2*w+t, 
                rotate(180, 
                    draw_u(top, bottom, w, t))))
        )



win = summon.Window("18_summon", size=(800,400))
win.set_bgcolor(1, 1, 1)

n = 20
for i in xrange(n, -1, -1):
    c = 1-i/float(n)
    r = i / float(n)

    win.add_group(scale(1+r, 1+r,
                        color(c, c, 1, c*.5),
                            draw_summon()))
win.add_group(group(
    color(0, 0, .5), 
    draw_summon(),
    color(0, 0, 0),
    text_clip("Matt Rasmussen 2008", -1500, -600, 1500, -1000, 0, 20, 
              "middle", "center")))

win.home()
