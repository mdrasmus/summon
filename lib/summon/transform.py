
from math import *


def makeIdentityMatrix():
    return [[1.0, 0.0, 0.0, 0.0],
            [0.0, 1.0, 0.0, 0.0],
            [0.0, 0.0, 1.0, 0.0],
            [0.0, 0.0, 0.0, 1.0]]


def makeZeroMatrix():
    return [[0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0]]


def multMatrix(mat1, mat2):
    mat3 = makeZeroMatrix()
    
    for i in (0, 1, 2, 3):
        for j in (0, 1, 2, 3):
            for k in (0, 1, 2, 3):
                mat3[i][j] += mat1[i][k] * mat2[k][j]
    return mat3

def makeTransMatrix(vec):
    return [[1.0, 0.0, 0.0, vec[0]],
            [0.0, 1.0, 0.0, vec[1]],
            [0.0, 0.0, 1.0, 0.0],
            [0.0, 0.0, 0.0, 1.0]]


def makeTransMatrix3(vec):
    return [[1.0, 0.0, 0.0, vec[0]],
            [0.0, 1.0, 0.0, vec[1]],
            [0.0, 0.0, 1.0, vec[2]],
            [0.0, 0.0, 0.0, 1.0]]

def makeRotateMatrix(angle, vec):
    s = sin(angle * (3.1415926/180.0))
    c = cos(angle * (3.1415926/180.0))
    t = 1 - c
    x, y = vec
    z = 1.0
    n = sqrt(x*x + y*y + z*z)
    
    # normalize
    x = x / n
    y = y / n
    z = z / n
    
    return [[t*x*x + c  , t*x*y - s*z, t*x*z + s*y, 0.0],
            [t*y*x + s*z, t*y*y + c  , t*y*z - s*x, 0.0],
            [t*z*x - s*y, t*z*y + s*x, t*z*z + c  , 0.0],
            [0.0        , 0.0        , 0.0        , 1.0]]


def makeRotateMatrix3(angle, vec):
    s = sin(angle * (3.1415926/180.0))
    c = cos(angle * (3.1415926/180.0))
    t = 1 - c
    x, y, z = vec
    n = sqrt(x*x + y*y + z*z)
    
    # normalize
    x = x / n
    y = y / n
    z = z / n
    
    return [[t*x*x + c  , t*x*y - s*z, t*x*z + s*y, 0.0],
            [t*y*x + s*z, t*y*y + c  , t*y*z - s*x, 0.0],
            [t*z*x - s*y, t*z*y + s*x, t*z*z + c  , 0.0],
            [0.0        , 0.0        , 0.0        , 1.0]]


def makeScaleMatrix(vec):
    return [[vec[0], 0.0   , 0.0, 0.0],
            [0.0   , vec[1], 0.0, 0.0],
            [0.0   , 0.0   , 0.0, 0.0],
            [0.0   , 0.0   , 0.0, 1.0]]


def makeScaleMatrix3(vec):
    return [[vec[0], 0.0   , 0.0   , 0.0],
            [0.0   , vec[1], 0.0   , 0.0],
            [0.0   , 0.0   , vec[2], 0.0],
            [0.0   , 0.0   , 0.0   , 1.0]]


def makeFlipMatrix(vec):
    x, y = vec
    h = sqrt(x*x + y*y)
    angle = 180 / pi * acos(x / h)
    if y < 0:
        angle *= -1
    
    rot = makeRotateMatrix(angle, (0.0, 0.0))
    rot2 = makeRotateMatrix(-angle, (0.0, 0.0))
    s = makeScaleMatrix((-1, 1))
    
    return multMatrix(rot2, multMatrix(s, rot))
