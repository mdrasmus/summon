/***************************************************************************
* SUMMON
* Matt Rasmussen
* screenshot.cpp
*
* This file creates main python interface to the SUMMON module.  The SUMMON 
* module itself, it represented by a singleton class SummonModule.
*
*
* This file is part of SUMMON.
* 
* SUMMON is free software: you can redistribute it and/or modify
* it under the terms of the Lesser GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
* 
* SUMMON is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
* 
* You should have received a copy of the Lesser GNU General Public License
* along with SUMMON.  If not, see <http://www.gnu.org/licenses/>.
* 
***************************************************************************/

#include "first.h"

// c/c++ headers
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <vector>

// opengl headers
#include <GL/glut.h>
#include <GL/gl.h>

// SDL headers
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>
#include <SDL/SDL_timer.h>

// summon headers
#include "SummonWindow.h"


namespace Summon {

    
bool ScreenshotWindow(SummonWindow *window, string filename)
{
    Vertex2i winsize = window->GetView()->GetWindowSize();
    
    // allocate pixel data
    int align; // bytes per pixel
    static const int srcpixelsize = 3;
    glGetIntegerv(GL_PACK_ALIGNMENT, &align);
    //glPixelStorei
    int rowsize = int(ceil(float(winsize.x * srcpixelsize) / align)*align);
        
        
        // read gl screen
        GLubyte *pixels = new GLubyte[rowsize * winsize.y];
        window->GetView()->CopyPixels(pixels);

        // create SDL surface
        Uint32 rmask, gmask, bmask, amask;
#       if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
#       else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
#       endif
        
        static const int bpp = 32;
        SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
            winsize.x, winsize.y, 
            bpp, rmask, gmask, bmask, amask);
        
        if (!surface) {
            Error("Internal error: cannot allocate SDL surface: %s",
                  SDL_GetError());
            delete [] pixels;
            return false;
        }
        
        
        SDL_LockSurface(surface);
        char *dest = (char *) surface->pixels;        
        int bytespp = surface->format->BytesPerPixel;
        
        // copy into SDL surface
        for (int y=0; y<surface->h; y++) {
            for (int x=0; x<surface->w; x++) {
                int a = y * rowsize + x * srcpixelsize;
                int b = (surface->h - 1 - y) * surface->pitch + x * bytespp;
                *((Uint32*) (dest + b)) = 
                    SDL_MapRGBA(surface->format, pixels[a + 0], pixels[a + 1], 
                                pixels[a + 2], 255);
            }
        }

        SDL_UnlockSurface(surface);
        

        // save the surface
        if (SDL_SaveBMP(surface, filename.c_str()) != 0) {
            Error("Error saving screenshot '%s'", filename.c_str());
            SDL_FreeSurface(surface);
            delete [] pixels;
            return false;
        }

        // cleanup
        SDL_FreeSurface(surface);
        delete [] pixels;
        
        return true;
    }
       

}; // namespace
