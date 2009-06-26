#!/usr/bin/env python-i

from sumgraph import *


infile = "graph.dot"
graph = pydot.graph_from_dot_file(infile)


win = summon.Window("graph")
win.set_bgcolor(1,1,1)
win.add_group(draw_graph(graph))
win.home()
