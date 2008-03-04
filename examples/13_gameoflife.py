#!/usr/bin/env python-i
# 13_gameoflife.py
#
# Conway's Game of Life
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
        """Record one change to the board"""
        
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
    
    
    def initBoard(self, ncreatures=0):
        """Initialize board"""
    
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
        """Compute one iteration of evolution"""
    
        board = self.board
        neighbors = self.neighbors
        changeset = self.changeset
        width = self.width
        height = self.height
        fate = {}
        
        # process add set
        for x, y in list(self.addset):
            if 1 <= x <= self.width and 1 <= y <= self.height and \
               board[y][x] == 0:
                board[y][x] = 1
                self.recordChange(x, y, 1)
        self.addset.clear()
        
        # process kill set
        for x, y in list(self.killset):
            if 1 <= x <= self.width and 1 <= y <= self.height and \
               board[y][x] == 1:
                board[y][x] = 0
                self.recordChange(x, y, 0)
        self.killset.clear()
        
        # do not evolve if paused
        if self.pause:
            return
        
        # apply evolution rules to every cell that has changed
        for (j, i) in changeset:
            if 1 <= j <= width and 1 <= i <= height:
            
                # evolve rules
                if board[i][j] == 1:
                   if neighbors[i][j] < 2 or \
                      neighbors[i][j] > 3:
                        fate[(j, i)] = 0
                else:
                    if neighbors[i][j] == 3:
                        fate[(j, i)] = 1
        changeset.clear()
        
        # recalculate neighbor counts and propogate changes
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
    """Creates a matrix with width and height (plus size-1 borders)"""
    
    # make board
    board = []
    for i in xrange(height+2):
        board.append([0] * (width+2))
    
    return board
   


def drawBoard(board):
    """Returns a SUMMON group representing the board"""
    
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


def createPattern():
    x, y = [int(round(i)) for i in win.get_mouse_pos('world')]
    addPattern(patterns[pattern_index].pattern, x, y)
    
def nextPattern():
    global pattern_index
    pattern_index = (pattern_index + 1) % len(patterns)
    print "current pattern:", patterns[pattern_index].name

def prevPattern():
    global pattern_index
    pattern_index = (pattern_index - 1) % len(patterns)
    print "current pattern:", patterns[pattern_index].name

def addPattern(pattern, x, y):
    w = len(pattern[0])
    h = len(pattern)

    for i in xrange(h):
        for j in xrange(w):
            if pattern[i][j] != " ":
                game.addset.add((x+j, y-i))
    

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
# patterns
# from http://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

class Pattern:
    def __init__(self, name, pattern):
        self.name = name
        self.pattern = pattern

pattern_index = 0

patterns = [
    Pattern("glider",
            ["###",
             "#  ",
             " # "]),

    Pattern("lwss",
            ["#### ",
             "#   #",
             "#    ",
             " #  #"]),

    Pattern("mwss",
            ["##### ",
             "#    #",
             "#     ",
             " #   #",
             "   #  "]),
    
    Pattern("hwss",
            ["###### ",
             "#     #",
             "#      ",
             " #    #",
             "   ##  "]),
    
    # http://home.interserv.com/~mniemiec/sship.htm
    Pattern("puffer",
            ["###   ###",
             "#  # #  #",
             "#       #",
             "#       #",
             " # # # # ",
             "         ",
             "    #    ",
             "   ###   ",
             "   ###   "]),
    
    Pattern("puffer2",
            ["###              ",
             "#  #             ",
             "#      ###       ",
             "#     #  #     # ",
             " # #  ## #    ###",
             "             ## #",
             "             ### ",
             "             ### ",
             "              ## "]),
    
    Pattern("turle",
            ["    ##    ",
             "## #  # ##",
             "##      ##",
             "# #    # #",
             "  ##  ##  ",
             " ## ## ## ",
             "   #  #   ",
             " #      # ",
             " #      # ",
             "          ",
             " ######## ",
             "##      ##"]),

    Pattern("s_spaceship",
            ["    #     #    ",
             "   ###   ###   ",
             "  #  #   #  #  ",
             " ###       ### ",
             "  # #     # #  ",
             "    ##   ##    ",
             "#    #   #    #",
             "     #   #     ",
             "##   #   #   ##",
             "  #  #   #  #  ",
             "    #     #    "]),
    
    Pattern("pulsar",
            ["  ##     ##  ",
             "   ##   ##   ",
             "#  # # # #  #",
             "### ## ## ###",
             " # # # # # # ",
             "  ###   ###  ",
             "             ",
             "  ###   ###  ",
             " # # # # # # ",
             "### ## ## ###",
             "#  # # # #  #",
             "   ##   ##   ",
             "  ##     ##  "]),
    
    Pattern("gosper_glider_gun",
            ["                        #           ",
             "                      # #           ",
             "            ##      ##            ##",
             "           #   #    ##            ##",
             "##        #     #   ##              ",
             "##        #   # ##    # #           ",
             "          #     #       #           ",
             "           #   #                    ",
             "            ##                      "]),
    
    Pattern("infinite_smallest",
            ["      # ",
             "    # ##",
             "    # # ",
             "    #   ",
             "  #     ",
             "# #     "]),
    
    Pattern("infinite_compact",
            ["### #",
             "#    ",
             "   ##",
             " ## #",
             "# # #"]),

    Pattern("infinite_row",
            ["######## #####   ###      ####### #####"]),
 
    Pattern("eight_bit_metheuselah",
            ["##  #",
             "#   #",
             "#  ##"])
    
]


#=============================================================================
# setup
#

print "Game of Life"
print 
print "press [spacebar] to pause evolution"
print "press 'c' to create a cell under the mouse pointer"
print "press 'd' to delete a cell under the mouse pointer"
print "press 'k' to kill a whole region under the mouse pointer"
print "press 'D' to delete all cells"
print "press 'w' to create a wave under the mouse pointer"
print "press 't' to create a cross under the mouse pointer"
print "press 'x' to create an 'X' under the mouse pointer"
print 
print "press '.' to move to next pattern"
print "press ',' to move to prev pattern"
print "press 'p' to create pattern"
print 
print "initializing..."

# create window
win = summon.Window("13_gameoflife")
win.add_group(hotspot("click", 0, 0, width, height, create))
win.set_visible(0, 0, width, height)
win.set_antialias(False)

win.set_binding(input_key(" "), pauseGame)
win.set_binding(input_key("c"), create)
win.set_binding(input_key("C", "shift"), createArea)
win.set_binding(input_key("d"), kill)
win.set_binding(input_key("k"), killArea)
win.set_binding(input_key("D", "shift"), killAll)
win.set_binding(input_key("w"), createWave)
win.set_binding(input_key("t"), createCross)
win.set_binding(input_key("x"), createX)
win.set_binding(input_key("X", "shift"), createBigX)

win.set_binding(input_key(","), prevPattern)
win.set_binding(input_key("."), nextPattern)
win.set_binding(input_key("p"), createPattern)



# create game state
game = Game(width, height)

# parse arguments
if len(sys.argv) > 1:
    if "wave" in sys.argv:
        make_wave()
    
    if "empty" in sys.argv:
        game.initBoard(0)
else:
    game.initBoard(ncreatures)


board = win.add_group(drawBoard(game.board))


#=============================================================================
# animate

print "animating"

def animate():
    global timer, board
    
    if not win.is_open():
        timer.stop()
    else:
        game.evolve()
        board = board.replace_self(drawBoard(game.board))

timer = summon.add_timer(animate, .2)
