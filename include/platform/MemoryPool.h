#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include "common/Types.h"
#include "PosixSharedMemory.h"

/**
   Memory pool model
   LOCN offset                                  ATTRIBUTE
   =======================================================
   0                                            name
   64                                           pathname
   128                                          createFlag
   132                                          mapFlag
   136                                          mode
   140                                          totalSize
   144                                          objectSize
   148                                          maxNumberOfObjects
   152                                          numberUsed
   156                                          free list
   156 + (8 * maxNumberOfObjects)               first free
   156 + (8 * maxNumberOfObjects) + 8           pShm
*/
#define MAX_MEMORY_POOL_NAME_LEN 64
#define MAX_MEMORY_POOL_PATHNAME_LEN 64

class MemoryPool
{
 public:

  MemoryPool (char* _name, char* _pathname, UINT32 _createFlag, UINT32 _mapFlag, UINT32 _mode,
	      UINT32 _objectSize, UINT32 _maxNumberOfObjects);
  ~MemoryPool ();
  void* getFromPool ();
  void removeFromPool (void* object);
  UINT32 getNumberUsed () { return *pNumberAllocated; }
  UINT32 getObjectSize () { return objectSize; }
  UINT32 getMaxNumberOfObjects () { return maxNumberOfObjects; }

  void* getMemoryLocationFromIndex (UINT64 index);
/*   UINT64 getIndexOfLastUsed (); */
  UINT64 getIndexFromMemoryLocation (void* location);

 private:  

  CHAR    name [MAX_MEMORY_POOL_NAME_LEN];
  CHAR    pathname [MAX_MEMORY_POOL_PATHNAME_LEN]; /*! used as the shared memory pathname. E.g.: "/someObjectPool" */
  UINT32  createFlag;
  UINT32  mapFlag;
  UINT32  mode;
  UINT32  totalSize;
  UINT32  sizeOfSharedManagementArea;
  UINT32  objectSize;
  UINT32  maxNumberOfObjects;
  UINT32* pNumberAllocated;
  UINT64* pFirstFree;
  UINT64* pFreeList; // Used to store pointers--should be 8 bytes on x86_64.
  PosixSharedMemory* pShm;

};

#endif
