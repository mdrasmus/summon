#!/usr/bin/env summon


import random
import time
import sys

from rasmus import util


width = 400
height = 400
ncreatures = int(width*height*.1)


class Game:
    def __init__(self, width, height, alloc=False):
        self.width = width
        self.height = height
        self.addset = set()
        self.pause = 0
        
        if alloc:
            self.board = makeBoard(width, height)
            self.neighbors = makeBoard(width, height)
        self.changeset = set()
    
    
    def recordChange(self, x, y, alive):
        neighbors = self.neighbors
        changeset = self.changeset    
        
        if alive:
            step = 1
        else:
            step = -1

        changeset.add((x-1, y-1))
        changeset.add((x  , y-1))
        changeset.add((x+1, y-1))
        changeset.add((x-1, y))
        changeset.add((x  , y))
        changeset.add((x+1, y))
        changeset.add((x-1, y+1))
        changeset.add((x  , y+1))
        changeset.add((x+1, y+1))

        neighbors[y-1][x-1] += step
        neighbors[y  ][x-1] += step
        neighbors[y+1][x-1] += step
        neighbors[y-1][x  ] += step
        neighbors[y+1][x  ] += step
        neighbors[y-1][x+1] += step
        neighbors[y  ][x+1] += step
        neighbors[y+1][x+1] += step
    
    
    def initBoard(self, ncreatures):
        self.board = makeBoard(width, height)
        self.neighbors = makeBoard(width, height)
        self.changeset.clear()        
        
        # randomly populate board
        for i in xrange(ncreatures):
            x = random.randint(1, self.width)
            y = random.randint(1, self.height)
            
            if self.board[y][x] == 0:
                self.board[y][x] = 1
                self.recordChange(x, y, 1)

    
    def evolve(self):
        board = self.board
        neighbors = self.neighbors
        changeset = self.changeset
        width = self.width
        height = self.height
        fate = {}
        
        
        if len(self.addset) > 0:
            for x, y in list(game.addset):
                if board[y][x] == 0:
                    board[y][x] = 1
                    self.recordChange(x, y, 1)
            self.addset.clear()
            return
        
        if self.pause > 0:
            self.pause -= 1
            return
        

        for (j, i) in changeset:
            if 1 <= j <= width and 1 <= i <= height:
                if board[i][j] == 1:
                   if neighbors[i][j] < 2 or \
                      neighbors[i][j] > 3:
                        fate[(j, i)] = 0
                else:
                    if neighbors[i][j] == 3:
                        fate[(j, i)] = 1
        
        changeset.clear()
        
        for (j, i), alive in fate.iteritems():
            board[i][j] = alive
            
            if alive:
                step = 1
            else:
                step = -1
            
            changeset.add((j-1, i-1))
            changeset.add((j  , i-1))
            changeset.add((j+1, i-1))
            changeset.add((j-1, i))
            changeset.add((j  , i))
            changeset.add((j+1, i))
            changeset.add((j-1, i+1))
            changeset.add((j  , i+1))
            changeset.add((j+1, i+1))
            
            neighbors[i-1][j-1] += step
            neighbors[i  ][j-1] += step
            neighbors[i+1][j-1] += step
            neighbors[i-1][j  ] += step
            neighbors[i+1][j  ] += step
            neighbors[i-1][j+1] += step
            neighbors[i  ][j+1] += step
            neighbors[i+1][j+1] += step


def makeBoard(width, height):
    # make board
    board = []
    for i in xrange(height+2):
        board.append([0] * (width+2))
    
    return board
   


def drawBoard(board):
    vis = []
    width = len(board[0]) - 2
    height = len(board) - 2
    
    for i in xrange(1, height+1):
        for j in xrange(1, width+1):
            if board[i][j] == 1:
                vis.extend([j, i])
    return group(color(0, 0, 1),
                 points(* vis),
                 color(1, 1, 1),
                 line_strip(-.5, -.5, 
                            width+.5, -.5,
                            width+.5, height+.5,
                            -.5, height+.5,
                            -.5, -.5))

def create():
    global gid
    x, y = [int(round(i)) for i in get_mouse_pos('world')]
    game.pause = 2
    
    if 1 <= x <= game.width and 1 <= y <= game.height:
        game.addset.add((x,y))


def make_wave():
    width = len(game.board[0]) - 2
    height = len(game.board) - 2

    for i in xrange(1, height+1):
        for j in xrange(1, width+1):
            game.board[i][j] = 0
            game.neighbors[i][j] = 0
    
    for j in xrange(10, width+1-10):
        game.board[height // 2][j] = 1
        game.recordChange(j, height // 2, 1)


def drawFrame():
    global gid
    

print "press 'a' to place a cell under the mouse pointer"
print "initializing..."
add_group(group(hotspot("click", 0, 0, width, height, create)))
set_binding(input_key("a"), create) 
set_visible(0, 0, width, height)
set_antialias(False)


game = Game(width, height)
game.initBoard(ncreatures)

"""
for row in game.board:
    print row
print
for row in game.neighbors:
    print row
"""

if len(sys.argv) > 2:
    if "wave" in sys.argv:
        make_wave()

pause = False
gid = add_group(drawBoard(game.board))


print "animating"
while True:
    game.evolve()
    
    g = drawBoard(game.board)
    gid = replace_group(gid, g)
    time.sleep(.1)
