#!/usr/bin/env python-i

from summon.core import *
from summon import shapes, util
from summon.vector import *
import summon
from math import *

from random import random, randint, uniform


class Actor:
    def __init__(self, game, pos=[0, 0]):
        self.game = game
        self.pos = pos[:]
        self.vel = [0, 0]
        self.accel = [0, 0]
        self.vis = None

    def set_drawing(self, vis):
        self.vis = vis

    def update(self):
        pass

    def update_physics(self):
        self.pos[0] += self.vel[0]
        self.pos[1] += self.vel[1]
        
        self.vel[0] += self.accel[0]
        self.vel[1] += self.accel[1]


class Player (Actor):
    def __init__(self, game, pos, health=100):
        Actor.__init__(self, game, pos)
        self.health = health


def find_closest(pos, actors, actor_filter=lambda x: True,
                 max_search=util.INF, min_dist=0):
    closest_dist = util.INF
    closest = None
    num = 0

    for other in actors:
        num += 1
        if closest != None and num > max_search and closest_dist < min_dist:
            break
        
        if actor_filter(other):
            dist = vdist(other.pos, pos)
            if dist < closest_dist:
                closest_dist = dist
                closest = other
    return closest


class Ship (Player):
    def __init__(self, game, pos, size=10, 
                 color=(1, 0, 0), team=0, 
                 **options):
        Player.__init__(self, game, pos, **options)
        self.color = color
        self.size = size
        self.team = team
        
        self.vis_ship = None
        
        self.turn = 1
        self.closest = None
        
    
    def plan(self):
        # change turn
        if random() < .01:
            self.turn *= -1
    
        # find closest actor
        if random() < .02:
            self.closest = find_closest(self.pos, self.game.actors, 
                                        lambda other: 
                                        other != self and 
                                        isinstance(other, Ship) and 
                                        other.team != self.team)
        
        # move towards closest actor
        if self.closest != None:
            closest_dist = vdist(self.closest.pos, self.pos)
        
            if closest_dist > 200 + uniform(-20, 20):
                self.vel = vdivs(vsub(self.closest.pos, self.pos), closest_dist/10)
            elif closest_dist < 50 + uniform(-10, 10):
                self.vel = vdivs(vsub(self.closest.pos, self.pos), -closest_dist/10)
            else:
                direct = vdivs(vsub(self.closest.pos, self.pos), closest_dist/1)
                self.vel = [-self.turn * direct[1], self.turn * direct[0]]
                    
            
    
    def update(self):
        if self.vis == None:
            return
        
        # plan actions
        self.plan()
        
        # update physics
        self.update_physics()
        
        if self.closest:
            ptr = group(color(self.color[0],
                              self.color[1],
                              self.color[2],
                              .9),
                              lines(self.pos[0], self.pos[1], 
                                    self.closest.pos[0], self.closest.pos[1]))
        else:
            ptr = group()
        
        # update graphics
        vis = group(translate(self.pos[0], self.pos[1], 
                              color(*self.color), 
                              shapes.regular_polygon(8, self.size)),
                    ptr)
        
        
        if self.vis_ship:
            #self.vis_ship = game.win.replace_group(self.vis_ship, vis) 
            self.vis_ship = self.vis.replace(self.vis_ship, vis)
        else:
            self.vis_ship = self.vis.append(vis)


class Ship2 (Ship):
    def __init__(self, game, pos, **options):
        Ship.__init__(self, game, pos, **options)
        self.seen = set()
        
    
    def plan(self):
        # change turn
        if random() < .01:
            self.turn *= -1
    
        # find closest actor
        if random() < .01:
            self.closest = find_closest(self.pos, self.game.actors, 
                                        lambda other: 
                                        other != self and 
                                        isinstance(other, Ship) and 
                                        other.team != self.team and 
                                        other not in self.seen)
                
        
        # move towards closest actor
        if self.closest != None:
            closest_dist = vdist(self.closest.pos, self.pos)
        
            if closest_dist > 200 + uniform(-20, 20):
                self.vel = vdivs(vsub(self.closest.pos, self.pos), closest_dist/20)
            elif closest_dist < 50 + uniform(-10, 10):            
                self.vel = vdivs(vsub(self.closest.pos, self.pos), -closest_dist/20)
            else:                
                direct = vdivs(vsub(self.closest.pos, self.pos), closest_dist/5)
                self.vel = [-self.turn * direct[1], self.turn * direct[0]]        
                
                self.seen.add(self.closest)
                if len(self.seen) == 10:
                    self.closest = None
                    
                if len(self.seen) > 30:
                    self.seen.clear()


class Ship3 (Ship):
    def __init__(self, game, pos, **options):
        Ship.__init__(self, game, pos, **options)
        self.follower = None
        self.seen = set()
        
    
    def plan(self):
        # find closest actor
        if self.closest == None and random() < .05:
            self.closest = find_closest(self.pos, self.game.actors, 
                                        lambda other: 
                                        other != self and 
                                        isinstance(other, Ship) and 
                                        other.closest != self and
                                        other.follower == None and
                                        other not in self.seen and
                                        other.team != self.team,
                                        max_search=10,
                                        min_dist=500)
            if self.closest != None:
                self.seen.add(self.closest)
                self.closest.follower = self
                
                if len(self.seen) > 1:
                    self.seen.pop()
                
        elif self.closest != None and random() < .0001:
            self.closest.follower = None
            self.closest = None

        
        # move towards closest actor
        if self.closest != None:
            closest_dist = vdist(self.closest.pos, self.pos)
        
            if closest_dist > 100:
                self.vel = vdivs(vsub(self.closest.pos, self.pos), log(closest_dist)*2)
            elif closest_dist < 90:
                self.vel = vdivs(vsub(self.closest.pos, self.pos), -closest_dist/2)
                #pass
            else:
                direct = vdivs(vsub(self.closest.pos, self.pos), closest_dist/20)
                self.vel = [-self.turn * direct[1], self.turn * direct[0]]        



class Game (summon.VisObject):
    def __init__(self):
        self.actors = []
        self.actors_vis = group()
        self.win = None
        

    def show(self):
        self.win = summon.Window("Battle")
        self.win.add_group(self.actors_vis)
        
        self.update()
        self.win.home()
        self.enable_updating(interval=.000)
        
        
    def add_actor(self, actor):
        actor.set_drawing(self.actors_vis.append(group()))
        self.actors.append(actor)

    def update(self): 
        for actor in self.actors:
            actor.update()


game = Game()

team_colors = [(1, 0, 0), (0, 0, 1), (0, 1, 0), (1, 1, 0)]

for i in xrange(500):
    team = randint(0, len(team_colors)-1)
    ship = Ship3(game, 
                [uniform(0, 100000), uniform(0, 100000)], 
                team=team,
                color=team_colors[team])
    ship.vel = [uniform(-5, 5), uniform(-5, 5)]
    game.add_actor(ship)


game.show()

