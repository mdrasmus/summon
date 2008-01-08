#!/usr/bin/env python-i

import summon
from summon import colors, shapes
from summon.core import *

import re

try:
    import pydot
except:
    raise Exception("This script requires pydot to be installed")





def draw_node(node):
    # get node position
    pos = node.get_pos()
    
    if pos == None:
        # node is incomplete, skip
        return group()
    x, y = map(float, pos.split(","))            
    
    # get node size
    size = (node.get_width(), node.get_height())    
    if size == (None, None):
        size = (1.0, 1.0)
    else:
        size = map(float, size)

    # get node name
    name = node.get_name()    
    name = name[1:-1]
    
    return translate(
            x, y,
            zoom_clamp(
                colors.blue,
                scale(size[0], size[1], 
                      shapes.regularPolygon(0, 0, 12, 20)),
                zoom_clamp(
                    colors.white,                         
                    text_scale(name, -20*size[0], -20*size[-1], 
                              20*size[0], 20*size[1], 
                              "center", "middle"),
                    minx=.5, miny=.5, maxx=4, maxy=4, link=True, clip=True),
                minx=.1, maxx=4, miny=.1, maxy=4, link=True, clip=True))


def draw_edge(edge):
    pos = edge.get_pos()
    
    # skip e,
    pos = map(float, re.split(" |,", pos[2:]))
    
    return group(colors.white, shapes.arrow(*pos, **{"head_size": 5}))

def draw_graph(graph):
    vis = group()

    # draw edges
    for edge in graph.get_edge_list():
        vis.append(draw_edge(edge))
    
    # draw nodes
    for node in graph.get_node_list():
        vis.append(draw_node(node))
    
    return vis



infile = "graph2.dot"

graph = pydot.graph_from_dot_file(infile)


win = summon.Window("graph")
win.add_group(draw_graph(graph))
win.home()
