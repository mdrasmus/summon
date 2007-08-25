#!/usr/bin/env python-i
# 11_dragon.py

from math import *

import summon
from summon.core import *


class Pencil:
    def __init__(self, x=0, y=0, angle=0):
        self.x = x
        self.y = y
        self.angle = angle
        self.vx = cos(angle)
        self.vy = sin(angle)
        self.points = []
    
    def forward(self):
        self.x += self.vx
        self.y += self.vy
        self.points.extend([self.x, self.y])
    
    def right(self):
        self.angle = (self.angle + pi/2.0) % (2*pi)
        self.vx = cos(self.angle)
        self.vy = sin(self.angle)
    
    def left(self):
        self.angle = (self.angle - pi/2.0) % (2*pi)
        self.vx = cos(self.angle)
        self.vy = sin(self.angle)
    
    def clear(self):
        self.points = []
    
    def color(self, val):
        self.points.append(color(1, .7-val/.7, 0))

# dragon rules:
# fx
#   x = x+yf+
#   y = -fx-y

colorspan = 2**14

def dragon(p, depth):
    def x(d):
        if d < 0:
            return []
        
        p.color((len(p.points) % colorspan) / float(colorspan))
        x(d-1)
        p.right()
        y(d-1)
        p.forward()
        p.right()

    def y(d):
        if d < 0:
            return []
        
        p.color((len(p.points) % colorspan) / float(colorspan))
        p.left()
        p.forward()
        x(d-1)
        p.left()
        y(d-1)
    
    p.clear()
    p.forward()
    x(depth)
    return p.points




win = summon.Window("17_dragon")
pencil = Pencil()
win.add_group(line_strip(* dragon(pencil, 14)))
win.home()
