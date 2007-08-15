#!/usr/bin/python -i
# 13_gameoflife.py
#
# Game of Life
#

import random
import time
import sys


from summon.core import *
import summon

#=============================================================================
# constants
#
# feel free to change


width = 400
height = 400
ncreatures = int(width*height*.1)


#=============================================================================
# data structures for Game of Life

class Game:
    def __init__(self, width, height, alloc=False):
        self.width = width
        self.height = height
        self.addset = set()
        self.killset = set()
        self.pause = False
        
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
        
        
        for x, y in list(self.addset):
            if 1 <= x <= self.width and 1 <= y <= self.height and \
               board[y][x] == 0:
                board[y][x] = 1
                self.recordChange(x, y, 1)
        self.addset.clear()

        for x, y in list(self.killset):
            if 1 <= x <= self.width and 1 <= y <= self.height and \
               board[y][x] == 1:
                board[y][x] = 0
                self.recordChange(x, y, 0)
        self.killset.clear()
        
        
        if self.pause:
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

#=============================================================================
# interactive functions
#

def create():
    x, y = [int(round(i)) for i in win.get_mouse_pos('world')]
    game.addset.add((x,y))


def createArea():
    x, y = [int(round(i)) for i in win.get_mouse_pos('world')]
    width = 20
    
    x = int(x // 3) * 3
    y = int(y // 3) * 3
    
    for i in range(x-width, x+width+1):
        for j in range(y-width, y+width+1):
            if i % 3 != 0 and j % 3 != 0:
                game.addset.add((i, j))
            else:
                game.killset.add((i, j))

def kill():
    x, y = [int(round(i)) for i in win.get_mouse_pos('world')]
    game.killset.add((x,y))

def killArea():
    x, y = [int(round(i)) for i in win.get_mouse_pos('world')]
    width = 20
    
    for i in range(x-width, x+width+1):
        for j in range(y-width, y+width+1):
            game.killset.add((i, j))

def killAll():
    for i in range(0, game.height):
        for j in range(0, game.width):
            game.killset.add((i, j))

def createWave():
    x, y = [int(round(i)) for i in win.get_mouse_pos('world')]
    width = 80
    
    for i in range(x-width, x+width+1):
        game.addset.add((i, y))

def createCross():
    x, y = [int(round(i)) for i in win.get_mouse_pos('world')]
    width = 22
    
    for i in range(x-width, x+width+1):
        game.addset.add((i, y)) 
    
    for i in range(y-width, y+width+1):
        game.addset.add((x, i))

def createX():
    x, y = [int(round(i)) for i in win.get_mouse_pos('world')]
    width = 40
    
    for i in range(-width, width+1):
        game.addset.add((x+i, y+i)) 
    
    for i in range(-width, width+1):
        game.addset.add((x+i, y-i))


def createBigX():
    x, y = [int(round(i)) for i in win.get_mouse_pos('world')]
    width = 1000
    
    for i in range(-width, width+1):
        game.addset.add((x+i, y+i)) 
    
    for i in range(-width, width+1):
        game.addset.add((x+i, y-i))


def createGlider():
    x, y = [int(round(i)) for i in win.get_mouse_pos('world')]
    
    mat = [[1, 1, 1],
           [0, 0, 1],
           [0, 1, 0]]
    
    for i in xrange(3):
        for j in xrange(3):
            if mat[i][j] == 1:
                game.addset.add((x+j, y+i))
    

def pauseGame():
    game.pause = not game.pause
    

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


#=============================================================================
# setup
#

print "Game of Life"
print 
print "press [spacebar] to pause evolution"
print "press 'c' to create a cell under the mouse pointer"
print "press 'k' to kill a cell under the mouse pointer"
print "press 'l' to kill a whole region under the mouse pointer"
print "press 'K' to kill all cells"
print "press 'w' to create a wave under the mouse pointer"
print "press 'g' to create a glider under the mouse pointer"
print "press 't' to create a cross under the mouse pointer"
print "press 'x' to create an 'X' under the mouse pointer"
print 
print "initializing..."

# create window
win = summon.Window("13_gameoflife")
win.add_group(group(hotspot("click", 0, 0, width, height, create)))
win.set_visible(0, 0, width, height)
win.set_antialias(False)

win.set_binding(input_key("c"), create)
win.set_binding(input_key("C", "shift"), createArea)
win.set_binding(input_key("k"), kill)
win.set_binding(input_key("l"), killArea)
win.set_binding(input_key("K", "shift"), killAll)
win.set_binding(input_key("g"), createGlider)
win.set_binding(input_key("w"), createWave)
win.set_binding(input_key("t"), createCross)
win.set_binding(input_key("x"), createX)
win.set_binding(input_key("X", "shift"), createBigX)
win.set_binding(input_key(" "), pauseGame)


# create game state
game = Game(width, height)
game.initBoard(ncreatures)


# parse arguments
if len(sys.argv) > 1:
    if "wave" in sys.argv:
        make_wave()

gid = win.add_group(drawBoard(game.board))


#=============================================================================
# animate

print "animating"
while True:
    game.evolve()
    
    g = drawBoard(game.board)
    
    # stop looping if window is closed
    if not win.is_open():
        break
        
    gid = win.replace_group(gid, g)
    time.sleep(.05)
