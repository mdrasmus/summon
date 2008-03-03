#!/usr/bin/env python-i


from math import *
from random import randint
from random import random as rand

from summon.core import *
from summon import util, shapes, colors
import summon

import time

def frand(a, b):
    return (b - a) * rand() + a


class Actor (object):
    def __init__(self, game, pos, vel):
        self.game = game
        self.pos = list(pos)
        self.vel = list(vel)
        self.force = [0.0, 0.0]
        self.mass = 1.0

    def clear_force(self):
        self.force[0] = 0.0
        self.force[1] = 0.0

    def integrate_force(self):
        self.pos[0] += self.vel[0]
        self.pos[1] += self.vel[1]
        self.vel[0] += self.force[0] / self.mass
        self.vel[1] += self.force[1] / self.mass

    def compute_wall_force_circle(self, size, bounce=1.0):
        bounce += 1.0
    
        if self.pos[0] + size > self.game.game_size[0] \
           and self.vel[0] >= 0.0:
            self.pos[0] = self.game.game_size[0] - size - .5
            self.force[0] += -bounce * abs(self.vel[0]) * self.mass
            
        if self.pos[0] - size < 0 \
           and self.vel[0] <= 0.0:
            self.pos[0] = size + .5
            self.force[0] += bounce * abs(self.vel[0]) * self.mass          

        if self.pos[1] + size > self.game.game_size[1] \
           and self.vel[1] >= 0.0:
            self.pos[1] = self.game.game_size[1] - size - .5
            self.force[1] += -bounce * abs(self.vel[1]) * self.mass
            
        if self.pos[1] - size < 0 \
           and self.vel[1] <= 0.0:
            self.pos[1] = size + .5
            self.force[1] += bounce * abs(self.vel[1]) * self.mass        
    

class Ball (Actor):
    def __init__(self, game, pos, vel=[0.0,0.0], size=10.0):
        Actor.__init__(self, game, pos, vel)
        self.size = size

    
    def draw(self):
        orth = [self.vel[1], -self.vel[0]]
        orth = util.vmuls(orth, self.size / util.vmag(orth))
    
        return group(colors.red, 
                     shapes.regular_polygon(self.pos[0], self.pos[1],
                                            30, self.size),
                     quads(color(1, 0, 0, .5), 
                           self.pos[0] + orth[0], self.pos[1] + orth[1],
                           self.pos[0] - orth[0], self.pos[1] - orth[1],
                           
                           color(1, 0, 0, 0),
                           self.pos[0] - 10*self.vel[0] - orth[0],
                           self.pos[1] - 10*self.vel[1] - orth[1],
                          
                           self.pos[0] - 10*self.vel[0] + orth[0],
                           self.pos[1] - 10*self.vel[1] + orth[1]
                          ))

        
    def update(self):
        self.compute_wall_force_circle(self.size)
        self.integrate_force()
        
        


class Bubble (Actor):
    def __init__(self, game, pos, size=10):
        Actor.__init__(self, game, pos, [0.0, 0.0])
        self.size = size
        self.mass = pi * size**2
    
    def draw(self):
        return group(color(.3, .3, .9),
                     shapes.regular_polygon(self.pos[0], self.pos[1],
                                            30, self.size))
    
    def update(self):
        self.force[1] += -.1 * self.mass
        self.compute_wall_force_circle(self.size, .5)
        
        # air resistance
        if util.vmag(self.vel) > 1:
            self.force = util.vadd(self.force, util.vmuls(self.vel, -.2))
        
        self.integrate_force()
        
        self.vel[0] = util.clamp(self.vel[0], -20, 20)
        self.vel[1] = util.clamp(self.vel[1], -20, 20)
        
        
    
    def collide(self, obj):
        dist = sqrt((self.pos[0]-obj.pos[0])**2 + 
                    (self.pos[1]-obj.pos[1])**2)
        d = util.vsub(obj.pos, self.pos)
        par = util.vproj(self.vel, d)
        orth = util.vsub(self.vel, par)
        
        if util.vdot(self.vel, d) > 0:
            self.force = util.vadd(self.force, 
                                   util.vmuls(par, -1.5 * self.mass))
                                   
            overlap = max(((self.size + obj.size) - dist) / 20.0, .1)
            self.force = util.vadd(self.force,
                                    util.vmuls(d, -overlap / dist * self.mass))
        

class Filler (summon.VisObject):
    def __init__(self, game_size=[400, 400], nballs=3):
        self.game_size = game_size[:]
        self.win = summon.Window("Filler", size=self.game_size)
        self.start = time.time()
        self.frames = 0
        self.growing_bubble = None
        self.bubbles = []
        

        self.objects = []

        # make balls
        for i in xrange(nballs):        
            angle = frand(0, 2*pi)
            self.objects.append(Ball(self,
                                   pos=[frand(5, self.game_size[0]-5),
                                        frand(5, self.game_size[1]-5)],
                                   vel=[5*cos(angle), 5*sin(angle)],
                                   size=4.5))
        
    
        self.win.add_group(group(colors.white,        
            shapes.box(0, 0, self.game_size[0], self.game_size[1], fill=False)))
                        
        self.objects_vis = self.win.add_group(self.draw_objects())
        
        self.enable_updating(True, 1/100.)
        self.win.set_binding(input_click("left", "down"), self.start_bubble)
        self.win.set_binding(input_motion("left", "down"), self.move_bubble)
        self.win.set_binding(input_click("left", "up"), self.end_bubble)
        self.win.set_binding(input_click("left", "click"), self.end_bubble)
     
    def update(self):
        self.frames += 1
        
        if self.frames > 100:
            print "FPS:", self.frames / (time.time() - self.start)        
            self.frames = 0
            self.start = time.time()
    
        # update objects
        for obj in self.objects:            
            obj.update()
            obj.clear_force()
        
        # update growing bubble
        if self.growing_bubble:
            self.growing_bubble.size += 2
            self.growing_bubble.mass = pi * self.growing_bubble.size**2
        
        # test collisions
        for i in xrange(len(self.objects)):
            for j in xrange(i+1, len(self.objects)):
                a = self.objects[i]
                b = self.objects[j]
                
                if isinstance(a, Bubble) and isinstance(b, Bubble):
                    dist = sqrt((a.pos[0]-b.pos[0])**2 + 
                                (a.pos[1]-b.pos[1])**2)
                    if dist < a.size + b.size:
                        a.collide(b)
                        b.collide(a)
        
        self.objects_vis = self.win.replace_group(self.objects_vis, 
                                                  self.draw_objects())
    
    def draw_objects(self):
        
        # draw objects
        vis = group()
        for obj in self.objects:
            vis.append(obj.draw())
        
        # draw growing bubble
        if self.growing_bubble != None:
            vis.append(self.growing_bubble.draw())
        
        return vis
    
    def start_bubble(self):
        pos = self.win.get_mouse_pos("world")
        self.growing_bubble = Bubble(self, pos, size=10)

    def end_bubble(self):
        self.objects.append(self.growing_bubble)
        self.growing_bubble = None
        

    def move_bubble(self):
        self.growing_bubble.pos = list(self.win.get_mouse_pos("world"))
    

game = Filler(game_size=(800, 600), nballs=30)
