/***************************************************************************
* Vistools
* Matt Rasmussen
* Matrix.cpp
*
***************************************************************************/

#include "first.h"
#include "common.h"
#include "Matrix.h"

namespace Vistools {


Matrix::Matrix() :
    nrows(0), ncols(0), nnz(0),
    rows(NULL), cols(NULL), vals(NULL)
{}


Matrix::~Matrix()
{
    if (rows) delete [] rows;
    if (cols) delete [] cols;
    if (vals) delete [] vals;
}


void Matrix::Create(int _nrows, int _ncols, int _nnz)
{
    nrows = _nrows;
    ncols = _ncols;
    nnz = _nnz;
    
    rows = new int [nnz];
    cols = new int [nnz];
    vals = new float [nnz];
}


bool Matrix::Read(FILE* stream)
{   
    // read header
    if (fscanf(stream, "%d %d %d", &nrows, &ncols, &nnz) != 3) {
        Error("Error on first line of file");
        return false;
    }
    
    // allocate space
    Create(nrows, ncols, nnz);
    
    // read non zeros
    for (int i=0; i<nnz; i++) {
        if (fscanf(stream, "%d %d %f", &rows[i], &cols[i], &vals[i]) != 3) {
            Error("Error on line %d", i + 2);
            return false;
        }
    }
    
    return true;
}

}
