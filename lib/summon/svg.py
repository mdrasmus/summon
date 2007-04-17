from summon_core import *
import summon

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
        print >>outfile, "<polygon fill='%s' stroke='%s' opacity='%f' points='" % \
                (color2string(color), color2string(color), color[3])
        for pt in verts:
            print >>outfile, "%f,%f " % (pt[0], pt[1])
        print >>outfile, "' />"
    
    def printText(elm, verts, color):
        msg = text_contents(elm)[0]
        pos = text_contents(elm)[5:]
        (x1, y1), (x2, y2) = verts
        
        col = color2string(color)
        
        # TODO: remove hack (why is color[3] so near 0)
        color[3] = 1
        
        
        boxheight = y2 - y1
        boxwidth = x2 - x1
        
        if summon.is_text_scale(elm) or summon.is_text_clip(elm):
            textheight = 20.0
            textwidth = textheight * .75 * float(len(msg)) # TODO: approx for now
            
            scale = min(boxheight / textheight, boxwidth / textwidth)
            
            print >>outfile, \
            """<g transform='translate(%f,%f) scale(%f,%f)'>
                <text x='0' y='0' font-size='%s' fill='%s' fill-opacity='%f'>%s</text></g>
            """ % (x1, y1, scale, -scale, textheight,  col, color[3], msg)
        elif summon.is_text(elm):
            print >>outfile, \
            """<g transform='translate(%f,%f) scale(1,-1)'>
                <text x='0' y='0' font-size='%s' fill='%s' fill-opacity='%f'>%s</text></g>
            """ % (x1, y1, 10, col, color[3], msg)
    
    
    def printText2(elm, verts, color):
        msg = text_contents(elm)[0]
        pos = text_contents(elm)[5:]
        (x1, y1), (x2, y2) = verts
        
        col = color2string(color)
        
        # TODO: remove hack (why is color[3] so near 0)
        color[3] = 1
        
        
        boxheight = y2 - y1
        boxwidth = x2 - x1
        
        if summon.is_text_scale(elm) or summon.is_text_clip(elm):
            textheight = 20.0
            textwidth = textheight * .75 * float(len(msg)) # TODO: approx for now
            
            scale = min(boxheight / textheight, boxwidth / textwidth)
            
            print >>outfile, \
            """<g transform='translate(%f,%f) scale(%f,%f)'>
                <text x='0' y='0' font-size='%s' stroke-width='%f' stroke='%s' 
                    fill='%s' stroke-opacity='%f' fill-opacity='%f'>%s</text></g>
            """ % (x1, y1, scale, -scale, textheight, 1/scale, col, col, 
                    0, color[3], msg)
        elif summon.is_text(elm):
            print >>outfile, \
            """<g transform='translate(%f,%f) scale(1,-1)'>
                <text x='0' y='0' font-size='%s' stroke-width='%f' stroke='%s' 
                    fill='%s' stroke-opacity='%f' fill-opacity='%f'>%s</text></g>
            """ % (x1, y1, 10, 1, col, col, 
                    0, color[3], msg)        
        

    def printGraphic(elm, verts, color):
        verts2 = []
        for vert in verts:
            verts2.append([(vert[0] + transx) * scalex,
                           (vert[1] + transy) * scaley])
        verts = verts2
    
        if summon.is_lines(elm) or summon.is_line_strip(elm):
            printLine(verts[0][0], verts[0][1], verts[1][0], verts[1][1], color)
        elif summon.is_triangles(elm) or \
             summon.is_triangle_strip(elm) or \
             summon.is_quads(elm) or \
             summon.is_quad_strip(elm) or \
             summon.is_polygon(elm):
            printPolygon(verts, color)
        elif summon.is_text_elm(elm):
            printText(elm, verts2, color)
    
    
    def printElm(elm):
        summon.visitGraphics(elm, printGraphic)

    # print out svg code
    (x, y, x2, y2) = get_visible()  
    boxWidth  = x2 - x
    boxHeight = y2 - y
    (width, height) = get_window_size()    
    transx = -x
    transy = -y
    scalex = width / boxWidth
    scaley = height / boxHeight
    bgcolor = get_bgcolor()
    
    print >>outfile, svgHeader
    print >>outfile, svgTag % (width, height)
    print >>outfile, "<g transform='translate(0, %d)'>" % height
    print >>outfile, "<g transform='scale(1, -1)'>"
    print >>outfile, "<rect x='0' y='0' width='%d' height='%d' fill='%s'/>" % \
            (width, height, color2string(bgcolor))
    #print >>outfile, "<g transform='scale(%f, %f)'>"  % (scalex, scaley)
    #print >>outfile, "<g transform='translate(%d, %d)'>" % (-x, -y)
    #print >>outfile, "<g stroke-width='%f'>" % (1 / max(scalex, scaley))

    printElm(group)

    print >>outfile, "</g></g>"  #</g></g></g>"
    print >>outfile, svgEndTag


def printScreen(filename = None, visgroup=None):
    if filename == None:
        import warnings
        warnings.filterwarnings("ignore", ".*", RuntimeWarning)    
        filename = os.tempnam(".", "_") + ".svg"
        warnings.filterwarnings("default", ".*", RuntimeWarning)        
        filename = filename.replace("./", "./summon")
    
    if visgroup == None:
        visgroup = get_group(get_root_id())
    
    print "dumping screen to '%s'..." % filename
    
    if filename.endswith(".svg"):
        outfile = file(filename, "w")    
        writeSvg(outfile, visgroup)
        outfile.close()
    else:
        svgfile = filename + ".svg"
        outfile = file(svgfile, "w")    
        writeSvg(outfile, visgroup)
        outfile.close()
        os.system("convert " +svgfile+ " " +filename)
        os.system("rm " + svgfile)
    
    print "wrote '%s'" % filename
    
    return filename

def printScreenPng(pngFilename = None):
    print "dumping screen to '%s'..." % pngFilename

    filename = printScreen()
    if pngFilename == None:
        pngFilename = filename.replace(".svg", ".png")
    os.system("convert " +filename+ " " +pngFilename)
    os.system("rm " + filename)
    
    print "wrote '%s'" % pngFilename
    
    return pngFilename


