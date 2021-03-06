"""

    Summon - SVG output

NOTE: zoom_clamps are not supported yet and text elements have basic support

"""

from summon.core import *
import summon
from summon import util, transform

import os
import math


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



class SvgWriter:
    def __init__(self):
        self.trans = [transform.makeIdentityMatrix()]
    
        
    def write(self, outfile, group, visible=(0.0, 0.0, 500.0, 500.0), 
                    size=(500,500), bgcolor=(0, 0, 0), win=None, 
                    scale=(1.0,1.0)):
        """print out svg code"""
         
        # get camera properties
        if win != None:
            visible = win.get_visible()
            size = win.get_size()
            bgcolor = win.get_bgcolor()         
        (x, y, x2, y2) = visible
        (width, height) = size
        
        # determine camera transform
        boxWidth  = x2 - x
        boxHeight = y2 - y
        self.scalex = width / boxWidth
        self.scaley = height / boxHeight
        self.s = scale   # prescale        
        
        self.out = outfile
        self.curcolor = [1, 1, 1, 1]

        print >>self.out, svgHeader
        print >>self.out, svgTag % (width, height)
        print >>self.out, "<g style='font-family: courier'>"
        print >>self.out, "<g transform='translate(0, %d)'>" % (height)
        print >>self.out, "<g transform='scale(1, -1)'>"
        print >>self.out, "<rect x='0' y='0' width='%d' height='%d' fill='%s'/>" % \
                (width, height, color2string(bgcolor))
        
        # transform camera
        print >>outfile, "<g transform='scale(%f, %f)'>" % \
            (self.scalex / self.s[0], self.scaley / self.s[1])
        print >>outfile, "<g transform='translate(%d, %d)'>" % \
            (self.s[0] * -x, self.s[1] * -y)
        print >>outfile, "<g stroke-width='%f'>" % \
            (1 / max(self.scalex * self.s[0], self.scaley * self.s[1]))
    
        mat = transform.makeTransMatrix((x, y))
        self.trans.append(transform.multMatrix(mat, self.trans[-1]))
        mat = transform.makeScaleMatrix((self.scalex, self.scaley))
        self.trans.append(transform.multMatrix(mat, self.trans[-1]))
        
        self.printElm(group)

        print >>self.out, "</g></g></g>"
        print >>self.out, "</g></g></g>"
        print >>self.out, svgEndTag

    def printLine(self, x1, y1, x2, y2, color):
        print >>self.out, "<line x1='%f' y1='%f' x2='%f' y2='%f' " % \
            (self.s[0] * x1, self.s[1] * y1, self.s[0] * x2, self.s[1] * y2)
        print >>self.out, "stroke-opacity='%f' " % (color[3])
        print >>self.out, "stroke='%s' />\n" % (color2string(color))


    def printPolygon(self, verts, color):
        print >>self.out, "<polygon fill='%s' stroke='%s' opacity='%f' points='" % \
                (color2string(color), color2string(color), color[3])
        for pt in verts:
            print >>self.out, "%f,%f " % (self.s[0] * pt[0], self.s[1] * pt[1])
        print >>self.out, "' />"

    
    def printText(self, elm):
        """write the SVG for a text element"""
        
        c = elm.get()
        kind = c[0]
        msg = c[1]
        x1, y1, x2, y2 = c[2:6]
        x1 = self.s[0] * x1
        x2 = self.s[0] * x2
        y1 = self.s[1] * y1
        y2 = self.s[1] * y2
        justify = set(c[6:])
        color = self.curcolor
        
        col = color2string(color)        
        
        boxheight = y2 - y1
        boxwidth = x2 - x1
        
        
        # TODO: add full justification support
        if "center" in justify:
            xjust = "center"
        elif "right" in justify:
            xjust = "right"
        else:
            xjust = "left"
        
        if "top" in justify:
            yjust = "top"
        elif "bottom" in justify:
            yjust = "bottom"
        else:
            yjust = "middle"

        if len(msg) == 0:
            return
        
        # TODO: add text_scale support
        if isinstance(elm, text_scale) or isinstance(elm, text_clip):
            textheight = 20.0
            textwidth = textheight * .75 * len(msg) # TODO: approx for now
            #mat = transform.multMatrixVec(self.trans[-1], (1.0, 1.0))
            
            scale = min(boxheight / textheight, boxwidth / textwidth)
            textwidth *= scale
            textheight *= scale

            if xjust == "left":
                tx = x1
            elif xjust == "right":
                tx = x2 - textwidth
            else:
                tx = (x1 + x2 - textwidth) / 2.0
            
            if yjust == "bottom":
                ty = y1
            elif yjust == "top":
                ty = y2 - textheight
            else:
                ty = (y1 + y2 - textheight) / 2.0

            # scale(%f,%f)'>  scale, -scale,
            
            print >>self.out, \
            """<g transform='translate(%f,%f) scale(1,-1)'>
                <text x='0' y='0' font-size='%s' fill='%s' fill-opacity='%f'>%s</text></g>
            """ % (tx, ty, textheight,  col, color[3], msg)
        elif isinstance(elm, text):
        
            scale = transform.getScaling(self.trans[-1])
            
            # TODO: approx for now        
            textheight = 13.0 * self.s[1] / scale[1]
            textwidth = textheight * .75 * len(msg) / scale[0] 
            
            if xjust == "left":
                tx = x1
            elif xjust == "right":
                tx = x2 - textwidth
            else:
                tx = (x1 + x2 - textwidth) / 2.0
            
            if yjust == "bottom":
                ty = y1
            elif yjust == "top":
                ty = y2 - textheight
            else:
                ty = (y1 + y2 - textheight) / 2.0
            
            print >>self.out, \
            """<g transform='translate(%f,%f) scale(1,-1)'>
                <text x='0' y='0' font-size='%s' fill='%s' fill-opacity='%f'>%s</text></g>
            """ % (tx, ty, textheight, col, color[3], msg)

    
    def printGraphic(self, elm):
        """write the SVG for a graphics element"""
        
        for verts, col in summon.iter_vertices(elm, self.curcolor):
            self.curcolor = col
            
            if isinstance(elm, lines) or \
               isinstance(elm, line_strip):
                self.printLine(verts[0][0], verts[0][1], 
                               verts[1][0], verts[1][1], col)
            elif isinstance(elm, triangles) or \
                 isinstance(elm, triangle_strip) or \
                 isinstance(elm, quads) or \
                 isinstance(elm, quad_strip) or \
                 isinstance(elm, polygon):
                self.printPolygon(verts, col)
    
    
    def printBeginTransform(self, elm):
        """write the opening tag for a transform"""
    
        c = elm.get()
        if isinstance(elm, translate):
            print >>self.out, "<g transform='translate(%f,%f)'>" % \
                (self.s[0] * c[1], self.s[1] * c[2])
            
        elif isinstance(elm, scale):
            print >>self.out, "<g transform='scale(%f,%f)'>" % \
                (c[1], c[2])
            
        elif isinstance(elm, rotate):
            print >>self.out, "<g transform='rotate(%f)'>" % c[1]
            
        elif isinstance(elm, flip):
            x, y = c[0], c[1]
            h = math.sqrt(x*x + y*y)
            angle = 180.0 / math.pi * math.acos(x / h)
            if y < 0:
                angle *= -1
            
            print >>self.out, "<g transform='rotate(%f) scale(-1, 1) rotate(%f)'>" % \
                (-angle, angle)
        

    def printEndTransform(self, elm):
        """write the closing tag for a transform"""
        
        if isinstance(elm, translate) or \
           isinstance(elm, scale) or \
           isinstance(elm, rotate) or \
           isinstance(elm, flip):
            print >>self.out, "</g>"
        
        
    def printElm(self, elm):
        """write the SVG an element and all of its children"""
        
        # print leaf elements
        if isinstance(elm, Graphic):
            self.printGraphic(elm)
        
        if isinstance(elm, text):
            self.printText(elm)
        
        # pre-process
        if isinstance(elm, Transform):
            self.printBeginTransform(elm)
            self.pushTransform(elm)
        
        # recurse
        for child in elm:
            self.printElm(child)

        # post-process
        if isinstance(elm, Transform):
            self.printEndTransform(elm)
            self.popTransform(elm)

    
    def pushTransform(self, elm):
    
        c = elm.get()[1:]
        if isinstance(elm, translate):
            mat = transform.makeTransMatrix(c)
            
        elif isinstance(elm, scale):
            mat = transform.makeScaleMatrix(c)
            
        elif isinstance(elm, rotate):
            mat = transform.makeRotateMatrix(c[0], [0, 0])
            
        elif isinstance(elm, flip):
            mat = transform.makeFlipMatrix(c)
                        
        else:
            mat = transform.makeIdentityMatrix()
        
        # should this be flipped?
        self.trans.append(transform.multMatrix(mat, self.trans[-1]))
    
    
    def popTransform(self, elm):
        self.trans.pop()



def writeSvg(outfile, group, visible=(0.0, 0.0, 500.0, 500.0), size=(500, 500), 
             bgcolor=(0, 0, 0), win=None, scale=(1.0, 1.0)):
    writer = SvgWriter()
    
    writer.write(outfile, group, visible, size, bgcolor, win=win, scale=scale)


    


def printScreen(win, filename = None, visgroup=None):
    if filename == None:
        filename = summon.tempfile(".svg")
    
    if visgroup == None:
        visgroup = win.get_root()
    
    print "dumping screen to '%s'..." % filename
    
    if filename.endswith(".svg"):
        outfile = file(filename, "w")    
        writeSvg(outfile, visgroup, win=win)
        outfile.close()
    else:
        svgfile = filename + ".svg"
        outfile = file(svgfile, "w")    
        writeSvg(outfile, visgroup, win=win)
        outfile.close()
        os.system("convert " +svgfile+ " " +filename)
        os.system("rm " + svgfile)
    
    print "wrote '%s'" % filename
    
    return filename


def printScreenPng(win, pngFilename = None):
    print "dumping screen to '%s'..." % pngFilename

    filename = printScreen(win)
    if pngFilename == None:
        pngFilename = filename.replace(".svg", ".png")
    os.system("convert " +filename+ " " +pngFilename)
    os.system("rm " + filename)
    
    print "wrote '%s'" % pngFilename
    
    return pngFilename


