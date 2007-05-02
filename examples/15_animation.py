#!/usr/bin/python -i
# SUMMON examples
# 15_animation.py - example of animation

import math, random

# make summon commands available
from summon.core import *
from summon import shapes
import summon


win = summon.Window("15_animation")


# class for bouncing Ball
class Ball:
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

# parameters (feel free to change these here and WHILE the animation is running!)
rate = 0
winsize = 300
maxballsize = 20
balls = []
nballs = 30
gravity = [0, -.1]
bounce = -.9


# create balls
x = winsize / 2
y = winsize / 2

for i in range(nballs):
    angle = random.random()
    vx = 5 * math.cos(angle)
    vy = 5 * math.sin(angle)
    size = maxballsize * i/ float(nballs)
    x = size + (winsize - 2 * size) * random.random()
    y = size + (winsize - 2 * size) * random.random()
    
    balls.append(Ball(x, y, size, vx, vy))



# draw bounding box
win.add_group(group(color(1,1,1), shapes.boxStroke(0, 0, winsize, winsize)))


def drawBall(ball):
    def func():
        ball.vy *= 3
        ball.vx *= 3
        ball.activate = 1
        
    val = (ball.vy + 3) / 6
    g = group(color(val, ball.activate, 1),
            translate(ball.x, ball.y,
            shapes.regularPoly(20, ball.size),
            hotspot("click", - 2 * ball.size, - 2*ball.size,
                             2*ball.size, 2*ball.size,
                             func)))
    ball.group = get_group_id(g)
    win.add_group(g)


def drawFrame():
    for ball in balls:
        if ball.group != None:
            win.remove_group(ball.group)

        # update position and velocity
        ball.x += ball.vx
        ball.y += ball.vy
        ball.vx += ball.ax
        ball.vy += ball.ay
        ball.ax = gravity[0]
        ball.ay = gravity[1]
        ball.activate *= .99
        

        # draw new ball
        drawBall(ball)

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
    
    # setup timer to call this function again
    summon.timer_call(rate, drawFrame)


# begin animation
win.home()
drawFrame()
