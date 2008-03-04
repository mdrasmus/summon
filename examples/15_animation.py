#!/usr/bin/env python-i
# SUMMON examples
# 15_animation.py - example of animation

import math, random

# make summon commands available
from summon.core import *
from summon import shapes
import summon


# parameters (feel free to change these here and WHILE the animation is running!)
rate = 1/60.0
winsize = 300
maxballsize = 20
nballs = 30
gravity = [0, -.1]
bounce = -.9





class Ball:
    """Class for bouncing Ball"""
    def __init__(self, x, y, size, vx, vy):
        self.group = None
        self.x = x
        self.y = y
        self.vx = vx
        self.vy = vy
        self.ax = 0
        self.ay = 0
        self.size = size
        self.activate = 0

    def move(self):
        # update position and velocity
        self.x += self.vx
        self.y += self.vy
        self.vx += self.ax
        self.vy += self.ay
        self.ax = gravity[0]
        self.ay = gravity[1]
        self.activate *= .99        

    def draw(self):
        """Method for draw a ball"""
        
        colorval = (self.vy + 3) / 6.0
        
        if self.group == None:
            
            self.color = group(color(colorval, self.activate, 1))

            self.group = win.add_group(
                translate(self.x, self.y,
                          self.color,
                          shapes.regular_polygon(0, 0, 20, self.size),
                          hotspot("click", - 2 * self.size, - 2*self.size,
                                  2*self.size, 2*self.size,
                                  self.on_click)))

        else:
            self.group.set(self.x, self.y)
            self.color = self.color.replace_self(group(color(colorval, self.activate, 1)))


    def on_click(self):
        """Callback function for when a ball is clicked"""
        self.vy *= 3
        self.vx *= 3
        self.activate = 1
        

def draw_frame():
    """Draw one frame of the animation"""

    global last, frames
    
    frames += 1
    
    if frames > 100:
        frames = 0
        now = summon.get_time()
        print "FPS: %f.1" % (100.0/ (now - last))
        last = now

    for ball in balls:
        # move ball
        ball.move()

        # update ball's drawing
        ball.draw()
        
        # calculate bouncing
        if ball.x < ball.size:
            ball.vx *= bounce
            ball.x = ball.size
        if ball.x > winsize - ball.size:
            ball.vx *= bounce
            ball.x = winsize - ball.size
            
        if ball.y < ball.size:
            ball.vy *= bounce
            ball.y = ball.size
        if ball.y > winsize - ball.size:
            ball.vy *= bounce
            ball.y = winsize - ball.size


# frame rate information
last = summon.get_time()
frames = 0

# create balls
x = winsize / 2
y = winsize / 2

balls = []
for i in range(nballs):
    angle = random.random()
    vx = 5 * math.cos(angle)
    vy = 5 * math.sin(angle)
    size = maxballsize * i/ float(nballs)
    x = size + (winsize - 2 * size) * random.random()
    y = size + (winsize - 2 * size) * random.random()
    
    balls.append(Ball(x, y, size, vx, vy))



# draw bounding box
win = summon.Window("15_animation")
win.add_group(group(color(1,1,1), shapes.box(0, 0, winsize, winsize, fill=False)))
win.home()


# setup timer to call the draw frame function
timer = summon.add_timer(draw_frame, interval=rate, window=win)
