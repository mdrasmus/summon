#!/usr/bin/env python-i


from summon.core import *
from summon import shapes, colors
import summon


win = summon.Window("zoom_clamp")


win.add_group(group(zoom_clamp(colors.blue,
                               shapes.regularPoly(10, 10),
                               maxx=3, maxy=3, minx=1, miny=1, clip=True),
                    translate(0, 100, 
                              zoom_clamp(colors.red,
                                         shapes.regularPoly(10, 10),
                                         translate(0, 10,                                         
                                                    color(1, .5, .5),
                                                    shapes.regularPoly(10, 2)),
                                         translate(0, -10,
                                            zoom_clamp(    
                                                color(1, .5, .5),
                                                shapes.regularPoly(10, 2),
                                                maxx=5, maxy=5, minx=1, miny=1)),
                                         maxx=3, maxy=3, minx=1, miny=1,
                                         link=True)),
                    rotate(45, colors.white,
                               lines(0, 0, 100, 0),
                               translate(100, 0, 
                               zoom_clamp(colors.yellow,
                                          shapes.regularPoly(4, 20),
                                          colors.blue,
                                          lines(0, 0, -20, 0),
                                          maxx=1, maxy=1, minx=1, miny=1,
                                          axis=(-1, 0)))),
                    translate(100, 0, 
                              colors.green,
                              shapes.regularPoly(10, 10)),
                    colors.white,
                    shapes.box(0, 0, 100, 100, fill=False)))
win.home()
