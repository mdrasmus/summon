from summon import *
from __init__ import *
import os


svgHeader = """<?xml version='1.0' encoding='UTF-8'?> 
<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' 
    'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>
"""

svgTag = """
<svg width='%d' height='%d' xmlns='http://www.w3.org/2000/svg' version='1.1'>
"""

svgEndTag = "</svg>"


def color2string(color):
  return "rgb(%d,%d,%d)" % (int(255 * color[0]),
                            int(255 * color[1]),
                            int(255 * color[2]))


def writeSvg(outfile, group):
    def printLine(x1, y1, x2, y2, color):
        print >>outfile, "<line x1='%f' y1='%f' x2='%f' y2='%f' " % (x1, y1, x2, y2)
        print >>outfile, "stroke-opacity='%f' " % (color[3])
        print >>outfile, "stroke='%s' />\n" % (color2string(color))


    def printPolygon(verts, color):
        print >>outfile, "<polygon fill='%s' opacity='%f' points='" % \
                (color2string(color), color[3])
        for pt in verts:
            print >>outfile, "%f,%f " % (pt[0], pt[1])
        print >>outfile, "' />"

    def printGraphic(elm, verts, color):
        if is_lines(elm) or is_line_strip(elm):
            printLine(verts[0][0], verts[0][1], verts[1][0], verts[1][1], color)
        elif is_triangles(elm) or \
             is_triangle_strip(elm) or \
             is_quads(elm) or \
             is_quad_strip(elm) or \
             is_polygon(elm):
            printPolygon(verts, color)

    def printElm(elm):
        visitGraphics(elm, printGraphic)

    # print out svg code
    (x, y, x2, y2) = get_visible()  
    boxWidth  = x2 - x
    boxHeight = y2 - y
    (width, height) = get_window_size()
    scale = width / boxWidth
    bgcolor = get_bgcolor()
    
    print >>outfile, svgHeader
    print >>outfile, svgTag % (width, height)
    print >>outfile, "<g transform='translate(0, %d)'>" % height
    print >>outfile, "<g transform='scale(1, -1)'>"
    print >>outfile, "<rect x='0' y='0' width='%d' height='%d' fill='%s'/>" % \
            (width, height, color2string(bgcolor))
    print >>outfile, "<g transform='scale(%f, %f)'>"  % (scale, scale)
    print >>outfile, "<g transform='translate(%d, %d)'>" % (-x, -y)
    print >>outfile, "<g stroke-width='%f'>" % (1 / scale)

    printElm(group)

    print >>outfile, "</g></g></g></g></g>"
    print >>outfile, svgEndTag


def printScreen(filename = None):
    if filename == None:
        filename = os.tempnam(".", "_") + ".svg"
        filename = filename.replace("./", "./visdraw")
    outfile = file(filename, "w")
    
    writeSvg(outfile, get_group(get_root_id()))
    outfile.close()
    print "wrote '%s'" % filename
    
    return filename

def printScreenPng(pngFilename = None):
    filename = printScreen()
    if pngFilename == None:
        pngFilename = filename.replace(".svg", ".png")
    os.system("convert " +filename+ " " +pngFilename)
    os.system("rm " + filename)
    
    print "wrote '%s'" % pngFilename
    
    return pngFilename

set_binding(input_key("p"), printScreen)
set_binding(input_key("p", "ctrl"), printScreenPng)



