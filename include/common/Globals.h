#ifndef GLOBALS_H
#define GLOBALS_H

#include "platform/MemoryPool.h"
#include <iostream>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
/*
  Include all the header files for all objects that
  are being stored in shared memory here.
*/
#include "test/PooledObject.h"
#define MAX_NUMBER_OF_POOLS 100

/**
   Create all the memory pools
*/


/*
  A NOTE ON MEMORY POOLS BEING POINTED TO HERE.
  =============================================
  Clearly, the pools are being created per object. These objects all have 
  a static member in their private region which points to the attribute name
  that is used by the class to create the pool. For example, for 
  the test class PooledObject, the following class defintion declares
  attribute name "pool" to be a static of type MemoryPool:

  class PooledObject
  {
  public:
    PooledObject ();
    ~PooledObject ();
    void* operator new (size_t numbytes);
    void operator delete (void* object);
  private:
    int eenie;
    int meenie;
    int mynie;
    int moe;
    static MemoryPool* pool; // <----- (1)
  };

  Now, the *Globals.h file is really NOT used to declare these static private attributes
  of classes whose memory allocations come from MemoryPools.

  What is the *Globals.h file really used for? It is used to declare and define 
  physical-memory-wide variables. I.e., variables that point to memory regions
  that don't change from one process to another.

  That is, if you have a task that needs the access to the region pointed to by
  (1) above, then you will need other handles. *Globals.h defines these _other_
  handles. This is why the name of the following variable is 
  "pooledObjectHandle" and not "pool."

  Another note: Clearly these are probably created AFTER the class definitions
  are loaded into the run-time system. So, they should not have "EXCL" handles.
*/
/*
   Example:

   MemoryPool* pooledObjectHandle = new MemoryPool ((char*) "pooledObject", 
						 (char*) "/pooledObject", 
						 // O_RDWR | O_CREAT | O_EXCL, // Incorrect handles see note above.
						                               // In short: this is not the creator of
						                               // the shared memory.
						 O_RDWR | O_CREAT, 
						 PROT_READ | PROT_WRITE, 
						 0666, 
						 sizeof (PooledObject), 
						 MAX_NUMBER_OF_POOLS);
*/

/**
   Posix shared memory
*/

/**
   Posix semaphores
*/


#endif
