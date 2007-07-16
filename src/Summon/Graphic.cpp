/***************************************************************************
* Summon
* Matt Rasmussen
* Graphic.cpp
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


bool Graphic::Build(Scm code)
{
    // determine data size
    m_datasize = GetDataSize(code);
    if (m_datasize == -1) {
        return false;
    }
    
    
    // allocate data
    if (m_data)
        delete [] m_data;
    m_data = new char [m_datasize];
    int ptr = 0;
    
    
    // populate data array
    // loop through code
    for (Scm children = code;
         ScmConsp(children);)
    {
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
            
            if (tag == VERTICES_CONSTRUCT) {
                Scm code = ScmCdr(child);
                if (!BuildVertices(m_data, ptr, code))
                    return false;
            }
            else if (tag == COLOR_CONSTRUCT) {
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
                return false;
            }
            
            children = ScmCdr(children);
        } else {
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
            
            if (tag == VERTICES_CONSTRUCT) {
                datasize += 1 + sizeof(int);  // tag (1 byte), len (4 bytes)
                
                // determine size of vertices primitive
                child = ScmCdr(child);                
                while (ScmConsp(child) && ScmFloatp(ScmCar(child))) {
                    datasize += sizeof(float);
                    child = ScmCdr(child);
                }
            }
            else if (tag == COLOR_CONSTRUCT) {
                datasize += 5; // (tag, r, g, b, a)
            } else {
                Error("Unknown primitive");
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


void Graphic::FindBounding(float *top, float *bottom, float *left, float *right,
                           TransformMatrix *matrix)
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

