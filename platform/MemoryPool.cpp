#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "platform/MemoryPool.h"
#include "common/Debug.h"

MemoryPool::MemoryPool (CHAR* _name, CHAR* _pathname, UINT32 _createFlag, UINT32 _mapFlag, UINT32 _mode,
			UINT32 _objectSize, UINT32 _maxNumberOfObjects) :
  createFlag (_createFlag),
  mapFlag (_mapFlag),
  mode (_mode),
  objectSize (_objectSize),
  maxNumberOfObjects (_maxNumberOfObjects)
{
  CHAR* byteifiedPShm;

  sizeOfSharedManagementArea = 
    (sizeof (UINT32)) + // Number of objects currently allocated
    (sizeof (UINT64)) + // First free location in memory pool
    (sizeof (UINT64) * _maxNumberOfObjects); // Total space reserved for the freelist management
  
  totalSize = sizeOfSharedManagementArea + (maxNumberOfObjects * objectSize);
  strcpy (name, _name);
  strcpy (pathname, _pathname);
  pShm = new PosixSharedMemory (pathname, createFlag, mode, mapFlag, totalSize);
      
  /**
     Set up the shared management area--critical for consistent cross-proces pool management!
  */
  
  byteifiedPShm = reinterpret_cast<char*> (pShm->getSharedMem ());
  pNumberAllocated = reinterpret_cast<UINT32*> (byteifiedPShm);
  pFirstFree       = reinterpret_cast<UINT64*> (byteifiedPShm + sizeof (UINT32));
  pFreeList        = reinterpret_cast<UINT64*> (byteifiedPShm + sizeof (UINT32) + sizeof (UINT64));
  
  if (createFlag & O_EXCL) // The following is done only for the creator of the memory pool
    {
      //cout << "Setting up shared management area inside shared memory for the process that created this shared memory...." << endl;
      *pNumberAllocated = 0;
      pFirstFree->uint64 = 0;
      for (UINT32 i = 0; i < maxNumberOfObjects; i++) // Assumes that you won't have more than 4 billion objects!
	{
	  pFreeList[i].uint64 = i + 1;	// Points to the next
	  //cout << "pFreeList[" << i << "].uint64 = " << pFreeList[i].uint64 << endl;
	}
      
      cout << "[" << getpid () << "] Memory pool " << name << " created." << endl;
    }
  else
    {
      cout << "[" << getpid () << "] Memory pool " << name << " was already created, this process has a handle to the shared pool." << endl;
    }
}

MemoryPool::~MemoryPool ()
{
  delete pShm;
}

void* MemoryPool::getFromPool ()
{
  void* thisObject;
  UINT64 index = *pFirstFree;
  UINT64 next = pFreeList [pFirstFree->uint64];

  if (*pNumberAllocated == maxNumberOfObjects)
    {
      cout << "Out of memory--cannot allocate any more pooled objects for " << pathname << "! Unrecoverable error!" << endl;
      return NULL;
    }  

  thisObject = getMemoryLocationFromIndex (*pFirstFree);
  *pFirstFree = next;
  (*pNumberAllocated) ++;

  UINT64 reIndex = getIndexFromMemoryLocation (static_cast<void*>(thisObject));
  cout << "[" << getpid () << "] +++ Allocated an object from memory pool " << name << " at index = " << index.uint64 << " (" << reIndex.uint64 << ")"
       << ", locn = " << hex << thisObject << dec << " first free = " << pFirstFree->uint64  << ", numberAllocated = " << *pNumberAllocated << endl;

  return thisObject;
}

void MemoryPool::removeFromPool (void* object)
{
  UINT64 index = getIndexFromMemoryLocation (object);
  if (*pNumberAllocated == 0)
    {
      cout << "Error--memory pool is empty!" << endl;
      return;
    }
  pFreeList [index.uint64] = *pFirstFree;
  *pFirstFree = index;
  
  cout << "[" << getpid () << "] --- Deallocated an object from memory pool " << name << " at index = " << index.uint64 << ", locn = " << 
    hex << object << dec << endl;

  (*pNumberAllocated) --;
}

void* MemoryPool::getMemoryLocationFromIndex (UINT64 index)
{
  /**
     Advance index locations from start of shared memory
  */
  void* location = NULL;
  char* byteifiedPShm = reinterpret_cast<char*> (pShm->getSharedMem ());

  //cout << "Shared memory is in location " << hex << pShm->getSharedMem () << dec << endl;
  location = reinterpret_cast<void*> (byteifiedPShm + sizeOfSharedManagementArea + (index.uint64 * objectSize));

  return location;

}

UINT64 MemoryPool::getIndexFromMemoryLocation(void* location)
{
  UINT64 index;
  char* byteifiedPShm = reinterpret_cast<char*> (pShm->getSharedMem ());
  char* byteifiedLocn = reinterpret_cast<char*> (location);

  index.uint64 = (byteifiedLocn - byteifiedPShm - sizeOfSharedManagementArea)/objectSize;

  if (index.uint64 < 0)
    {
      cout << "Trying to delete an object not in the pool. Error!" << endl;
      location = 0;
    }
  else
    {
      //cout << "Shared memory " << hex << location << dec << " is in location " << index.uint64 << " for byteifiedPShm in " << hex << pShm->getSharedMem() << dec << endl;
    }

  return index;
}
