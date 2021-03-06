/***************************************************************************
* Summon
* Matt Rasmussen
* Graphic.cpp
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
#include "Graphic.h"

namespace Summon {

using namespace std;


Graphic::Graphic(int id) :
    Element(id),
    m_data(NULL),
    m_datasize(0)
{
}

Graphic::~Graphic()
{   
    if (m_data)
        delete [] m_data;
}


bool Graphic::Build(int header, const Scm &code2)
{
    SetId(header);
    Scm code = code2;
    
    //printf("graphic: %d\n", m_id);
    
    // special case for lonely color construct
    if (m_id == COLOR_CONSTRUCT) {
        code = ScmCons(ScmCons(Int2Scm(header), code), Scm_EOL);
    }
    
    // determine data size
    m_datasize = GetDataSize(code);
    if (m_datasize == -1) {
        return false;
    }
    
    
    // allocate data
    if (m_data)
        delete [] m_data;
    m_data = new char [m_datasize];
    
    
    // populate data array
    // loop through code
    int ptr = 0;
    for (Scm children=code; ScmConsp(children);) {
        // get code for child
        Scm child = ScmCar(children);
        
        // if number then it is an implied vertices primitive
        if (ScmFloatp(child)) {
            if (!BuildVertices(m_data, ptr, children))
                return false;
        }
        
        // otherwise it is a construct
        else if (ScmConsp(child)) {
            int tag = Scm2Int(ScmCar(child));
            
            if (tag == COLOR_CONSTRUCT) {
                // parse color construct
                m_data[ptr++] = PRIM_COLOR;
                int i=0;
                
                child = ScmCdr(child);
                for (; ScmConsp(child) && i<4; i++, child = ScmCdr(child)) {
                    int val = int(Scm2Float(ScmCar(child)) * 255);
                    if (val > 255) val = 255;
                    if (val < 0) val = 0;
                    m_data[ptr++] = val;
                }
                
                for (; i<4; i++) {
                    m_data[ptr++] = 255;
                }
            } else {
                // unknown construct
                return false;
            }
            
            children = ScmCdr(children);
        } else {
            // unknown object in data
            return false;
        }
    }
    
    return true;
}


bool Graphic::BuildVertices(char *data, int &ptr, Scm &code)
{
    data[ptr++] = PRIM_VERTICES; // write tag
    int len = 0;
    int lenptr = ptr;
    ptr += sizeof(int); // skip len field to fill in later

    // store successive floats in data
    do {
        Scm child = ScmCar(code);
        if (!ScmFloatp(child))
            break;
        *((float*) (data + ptr)) = Scm2Float(child);
        ptr += sizeof(float);
        len++;
        code = ScmCdr(code);
    } while (ScmConsp(code));
    
    // record number of vertices
    if (len % 2 == 0) {
        *((int*) (data + lenptr)) = len / 2;
    } else {
        Error("Odd number of coordinates given for vertices");
        return false;
    }
    
    return true;
}



int Graphic::GetDataSize(Scm code)
{
    int datasize = 0;

    // loop through code
    for (Scm children = code;
         ScmConsp(children);)
    {
        // get code for child
        Scm child = ScmCar(children);
        
        // if number then it is an implied vertices primitive
        if (ScmFloatp(child)) {
            datasize += 1 + sizeof(int);
            
            // determine size of vertices primitive
            do {
                datasize += sizeof(float);
                children = ScmCdr(children);
            } while (ScmConsp(children) && ScmFloatp(ScmCar(children)));
        }
        
        // otherwise it is a construct
        else if (ScmConsp(child)) {
            int tag = Scm2Int(ScmCar(child));
            
            if (tag == COLOR_CONSTRUCT) {
                datasize += 5; // (tag, r, g, b, a)
            } else {
                Error("Unknown primitive: %d", tag);
                return -1;
            }
            
            
            children = ScmCdr(children);
        } else {
            Error("Unknown primitive");
            return -1;
        }
    }

    return datasize;
}


Scm Graphic::GetContents()
{
    Scm children = Scm_EOL;

    // build primitives
    for (int ptr = 0; More(ptr); ptr = NextPrimitive(ptr))
    {
        Scm child = Scm_EOL;

        if (IsVertices(ptr)) {
            float *data = GetVertex(VerticesStart(ptr));

            for (int i = 2 * GetVerticesLen(ptr) - 1; i > 0; i-=2)
            {
                child = ScmCons(Float2Scm(data[i-1]), 
                                ScmCons(Float2Scm(data[i]), child));
            }
            child = ScmCons(Int2Scm(VERTICES_CONSTRUCT), child);

        } else if (IsColor(ptr)) {
            char *color = GetColor(ptr);
            for (int i = 3; i >= 0; i--) {
                child = ScmCons(Float2Scm(((unsigned char) color[i]) / 255.0), child);
            }
            child = ScmCons(Int2Scm(COLOR_CONSTRUCT), child);

        } else {
            // unknown primitive
            Error("unknown primitive");
            assert(0);
        }

        children = ScmAppend(children, ScmCons(child, Scm_EOL));
    }

    return children;
}


void Graphic::FindBounding(float *top, float *bottom, float *left, float *right,
                           const TransformMatrix *matrix,
                           const Camera &camera)
{    
   for (int ptr = 0; More(ptr); 
        ptr = NextPrimitive(ptr))
   {
       if (IsVertices(ptr)) {
           int len = 2 * GetVerticesLen(ptr);
           float *data = GetVertex(VerticesStart(ptr));

           for (int k=0; k<len-1; k+=2) {
               float x, y;
               matrix->VecMult(data[k], data[k+1], &x, &y);
               if (x < *left)   *left   = x;
               if (x > *right)  *right  = x;
               if (y < *bottom) *bottom = y;
               if (y > *top)    *top    = y;
           }
       }
   }
}


} // namespace Summon

