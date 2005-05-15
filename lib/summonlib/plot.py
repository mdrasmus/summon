from summon import *
import os


def plotList(arr):
    pts = []
    for i in range(len(arr)):
        pts.append(i)
        pts.append(arr[i])
    return group(line_strip(apply(vertices, pts)))

def plotFunc(func, start, end, step):
    pts = []
    i = start
    while i < end:
        pts.append(i)
        pts.append(func(i))
        i += step
    return group(line_strip(apply(vertices, pts)))



