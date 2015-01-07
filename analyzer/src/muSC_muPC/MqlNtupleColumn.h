#ifndef MQLNTUPLECOLUMN
#define MQLNTUPLECOLUMN

#include <stdlib.h>
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"

static const int MQLNTUPLECOLUMN_REALLOC_N = 2;
static const int MQLNTUPLECOLUMN_REALLOC_C = 8;

template<typename T> class MqlNtupleColumn
{
  public:

  MqlNtupleColumn() {
    fArray = NULL;
    fAllocation = 0;
  }

  ~MqlNtupleColumn() {
    if(fArray != NULL) {
      delete[] fArray;
    }
  }

  inline T& operator[] (int i) {
    return fArray[i];
  }

  void realloc(int newSize) {

    // make a new array and put it into place
    T *oldArray = fArray;
    T *newArray = new T[newSize];
    fArray = newArray;
    int oldSize = fAllocation;
    fAllocation = newSize;

    // copy the contents and delete the old array
    if(oldArray != NULL) {
      memcpy(newArray, oldArray, oldSize*sizeof(T)); 
      delete[] oldArray;  
    }

  }
  private:

  T *fArray;
  int fAllocation;
};

#endif
