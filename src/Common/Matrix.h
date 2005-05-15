/***************************************************************************
* Vistools
* Matt Rasmussen
* Matrix.h
*
***************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>

namespace Vistools {

class Matrix
{
public:
    Matrix();
    virtual ~Matrix();
    
    void Create(int nrows, int ncols, int nnz);
    bool Read(FILE* stream);

    int nrows;
    int ncols;
    int nnz;
    
    int *rows;
    int *cols;
    float *vals;
};

}

#endif
