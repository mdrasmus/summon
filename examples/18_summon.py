#!/usr/bin/env python-i
# draws SUMMON logo
#

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

def draw_m(top, bottom, w, t):
    return group(
        translate(0, -2*w+t, 
            rotate(180, 
                draw_u(top, bottom, w, t))),
        translate(2*w-t, -2*w+t, 
            rotate(180, 
                draw_u(top, bottom, w, t))))

def draw_summon():    
    t = 150 # thickness
    w = 200 # width
    s = 50  # spacing
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
            draw_m(top, bottom, w, t)),
        
        # M
        translate(8*w-t+3*s, 0,
            draw_m(top, bottom, w, t)),
        
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



def blur(x, col):
    return group(
                 # color fade
                 quads(col, -2000, 0, 2000, 0,
                       color(0, 0, 0, 0), 2000, 300, -2000, 300),
                 
                 # white fades
                 quads(color(1, 1, 1, 1), -2000, 0, -2000, 600,
                       color(1, 1, 1, 0), -x, 600, -x, 0),
                 quads(color(1, 1, 1, 1), 2000, 0, 2000, 600,
                       color(1, 1, 1, 0), x, 600, x, 0))


def draw_summon_logo():
    return group(
        blur(1200, color(0, .2, .5, .8)),
        rotate(180, blur(0, color(0, 0, .5, .5))),

        color(0, 0, 0), 
        draw_summon(),
    
        color(0, 0, 0),
        text_clip("visualization prototyping and scripting", 
                  -1600, -450, 1600, -900, 0, 20, 
                  "top", "center"))

# draw logo
win = summon.Window("18_summon", size=(800,400))
win.set_bgcolor(1, 1, 1)
win.add_group(draw_summon_logo())

win.home()

