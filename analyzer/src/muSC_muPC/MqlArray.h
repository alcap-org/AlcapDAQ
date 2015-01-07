//
// Tables are kept in arrays of structures.  Each time an element is added, 
// the array size increases according to size_new = N + C*size_old, where N 
// would typically be of order 10 and C would typically be of order 2.  It is
// necessary to copy all values when changing the array size, so it should
// be done fairly infrequently.
//

#ifndef MQLARRAY
#define MQLARRAY

#include <stdio.h>
#include <stdlib.h>

static const int REALLOC_N = 2;
static const int REALLOC_C = 8;

template<typename T> class MqlArray
{
  public:

  MqlArray() {
    fArray = NULL;
    fSize = 0;
    fLastSize = 0;
    fAllocation = 0;
    fImmutable = false;
  }

  ~MqlArray() {
    if(!fImmutable && fArray != NULL) {
      delete[] fArray;
    }
  }

  inline T& operator[] (int i) {
    return fArray[i];
  }

  inline int next() {
    int retval = fSize;
    setSize(fSize + 1);
    return retval;
  }

  inline int size() {
    return fSize;
  }

  inline void setSize(int size) {
    if(size > fAllocation) {
      realloc(size);
    }
    fSize = size;
  }

  inline void reset() {
#if 0
    if(!fImmutable) {
      int nextSize = (int) (fLastSize * 0.9);
      fSize = 0;
      realloc(nextSize);
    }
#endif
  }

  inline void finish() {
#if 0
    if(!fImmutable) {
      fAllocation = 0;
      fLastSize = fSize;
      fSize = 0;

      if(fArray != NULL) {
        delete[] fArray;
        fArray = NULL;
      }
    }
#else
    if(!fImmutable) {
      fSize = 0;
    }
#endif
  }

  inline void sort(int (*compar)(const void *, const void *)) {
    qsort(fArray, fSize, sizeof(T), compar);
  }

  inline bool checkSort(int (*compar)(const void *, const void *)) {
    for(int i = 1; i < fSize; i++) {
//      if(compar(&fArray[i-1], &fArray[i]) > 0) {
      if(compar(&(*this)[i-1], &(*this)[i]) > 0) {
printf("checkSort: problem at element %d\n", i);
        return false;
      }
    }

    return true;
  }

  void realloc(int minSize) {

    int newSize = REALLOC_N*fAllocation + REALLOC_C;

    if(minSize > newSize) {
      newSize = minSize;
    }

    // make a new array and put it into place
    T *oldArray = fArray;
    T *newArray = new T[newSize];
    fArray = newArray;
    fAllocation = newSize;

    // copy the contents and delete the old array
    if(oldArray != NULL) {
      memcpy(newArray, oldArray, fSize*sizeof(T)); 
      memset(newArray+fSize, 0, (newSize-fSize)*sizeof(T));
      delete[] oldArray;  
    } else {
      memset(newArray, 0, (newSize)*sizeof(T));
    }
  }

  void fromBank(void *pevent, char *bankName) {
    fSize = bk_locate(pevent, bankName, (DWORD*) &fArray)
             * sizeof(DWORD) / sizeof(T);
    if(fArray == NULL)
    {
      printf("Warning: could not find bank %s\n", bankName);
    }
    fImmutable = true;
  }

  void toBank(void *pevent, char *bankName) {
    T *outputArray;
    bk_create(pevent, bankName, TID_DWORD, (DWORD*) &outputArray);
    memcpy(outputArray, fArray, fSize * sizeof(T));
    bk_close(pevent, outputArray + fSize);
  }

  private:

  T *fArray;
  int fAllocation;
  int fSize;
  int fLastSize;
  bool fImmutable;
};

#endif
