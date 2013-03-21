#ifndef POOLED_OBJECT
#define POOLED_OBJECT

/**
An arbitrary class used to test MemoryPool object operations
*/
#include "platform/MemoryPool.h"
#include <new>

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
  static MemoryPool* pool;
};
#endif
