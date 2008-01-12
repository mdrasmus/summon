#!/usr/bin/env python-i

import summon
from summon import colors, shapes, util
from summon.core import *

import re

try:
    import pydot
except:
    raise Exception("This script requires pydot to be installed")



def lerp(x0, x1, t):
    return x0 * (1-t) + x1 * t

def bezier(x0, x1, x2, x3, t):
    x00 = lerp(x0, x1, t)
    x01 = lerp(x1, x2, t)
    x02 = lerp(x2, x3, t)
    x10 = lerp(x00, x01, t)
    x11 = lerp(x01, x02, t)
    x20 = lerp(x10, x11, t)
    return x20


def bezier_curve(control_pts, ndivs=20):
    pts = []
    c = control_pts
    
    for i in xrange(0, len(c)-6, 6):
        for t in util.frange(0.0, 1.0 + 1.0/ndivs, 1.0/ndivs):
            pts.append(bezier(c[i],   c[i+2], c[i+4], c[i+6], t))
            pts.append(bezier(c[i+1], c[i+3], c[i+5], c[i+7], t))
    return pts


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
    node_radius = 37
    label_radius = 30

    # get node name
    name = node.get_name()    
    name = name[1:-1]
    
    return translate(
            x, y,
            zoom_clamp(
                colors.blue,
                scale(size[0], size[1], 
                      shapes.regularPolygon(0, 0, 20, node_radius)),
                zoom_clamp(
                    colors.white,                         
                    text_scale(name, -label_radius*size[0], -label_radius*size[-1], 
                               label_radius*size[0], label_radius*size[1], 
                               "center", "middle"),
                    minx=.5, miny=.5, maxx=1, maxy=1, 
                    link=True, clip=True, link_type="smaller"),
                minx=.1, maxx=1, miny=.1, maxy=1, 
                link=True, clip=True, link_type="smaller"))


def draw_edge(graph, edge):
    pos = edge.get_pos()
    
    # split into vertices
    vertices = re.split(" ", pos)
    control_pts = []
    arrow_head = (None, None)
    
    for v in vertices:
        if v[0] == "e":
            arrow_head = ("end", map(float, v[2:].split(",")))
        elif v[0] == "s":
            arrow_head = ("start", map(float, v[2:].split(",")))
        else:
            control_pts.extend(map(float, v.split(",")))
    
    curve = bezier_curve(control_pts, ndivs=20)
    
    target = graph.get_node('"' + edge.get_destination() + '"')
    target_pos = map(float, target.get_pos().split(","))
    
    source = graph.get_node('"' + edge.get_source() + '"')
    source_pos = map(float, source.get_pos().split(","))
    
    if arrow_head[0] == "start":
        return group(colors.black,
                     lines(source_pos[0], source_pos[1], 
                           control_pts[0], control_pts[1]),
                     line_strip(*curve),
                     lines(target_pos[0], target_pos[1], 
                           control_pts[-2], control_pts[-1]),
                     shapes.arrow(arrow_head[1][0], arrow_head[1][1],
                                  control_pts[0], control_pts[1],
                                  head_size=7))
    elif arrow_head[0] == "end":
        return group(colors.black, 
                     lines(source_pos[0], source_pos[1], 
                           control_pts[0], control_pts[1]),
                     line_strip(*curve),
                     lines(target_pos[0], target_pos[1], 
                           control_pts[-2], control_pts[-1]),
                     colors.red,
                     zoom_clamp(
                         shapes.arrow_head(arrow_head[1][0], arrow_head[1][1], 
                                           control_pts[-2], control_pts[-1],
                                           size=7),
                         origin=target_pos,
                         axis=arrow_head[1],
                         maxx=1, maxy=1, minx=.1, miny=.1, 
                         clip=True, link=True, link_type="smaller"))
    else:
        return group(colors.black, 
                     lines(source_pos[0], source_pos[1], 
                           control_pts[0], control_pts[1]),
                     line_strip(*curve),
                     lines(target_pos[0], target_pos[1], 
                           control_pts[-2], control_pts[-1]))


def draw_graph(graph):
    vis = group()

    # draw edges
    for edge in graph.get_edge_list():
        vis.append(draw_edge(graph, edge))
    
    # draw nodes
    for node in graph.get_node_list():
        vis.append(draw_node(node))
    
    return vis



infile = "graph2.dot"
#infile = "undirected_layout.dot"

graph = pydot.graph_from_dot_file(infile)


win = summon.Window("graph")
win.set_bgcolor(1,1,1)
win.add_group(draw_graph(graph))
win.home()
