from summon import *
import random


vis = []
for i in range(100000):
    for j in range(4):
        x = random.random()
        vis.append(quads(color(x, 1-x, 0), i, j, i+1, j, i+1, j+1, i, j+1))
add_group(group(scale(1,100, list2group(vis))))
