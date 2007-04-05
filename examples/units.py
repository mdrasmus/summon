#!/usr/bin/env summon

from summon.core import *


units = [149e9, "distance from sun",
         1.392e9, "diameter of sun",
         2*6372.797e3, "diameter of earth",
         1e3, "kilometer",
         1e2, "",
         1e1, "",
         1, "meter", 
         1e-1, "",
         1e-2, "", 
         1e-3, "millimeter", 
         1e-4, "", 
         1e-5, "",
         1e-6, "micrometer", 
         1e-7, "", 
         1e-8, "",
         1e-9, "nanometer", 
         1e-10, "", 
         1e-11, "",
         1e-12, "picometer"]


vis = []
for i in range(0, len(units), 2):
    size, name = units[i:i+2]
    
    vis.append(line_strip(0, 0,
                          size, 0,
                          size, size,
                          0, size,
                          0, 0))
    
    if name != "":
        vis.append(text_scale(name, 0, size/2.0, size, size, "bottom"))    
    vis.append(text_scale("%.2e" % size, 0, 0, size, size/2.0, "top"))
                        

shrink = 1
add_group(group(scale(shrink, shrink, *vis)))
set_visible(0, 0, shrink, shrink)
