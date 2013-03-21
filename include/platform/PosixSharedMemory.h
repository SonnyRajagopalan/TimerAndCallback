#ifndef POSIX_SHARED_MEMORY_H
#define POSIX_SHARED_MEMORY_H

#include "common/Types.h"

#define MAX_POSIX_SHARED_MEMORY_PATHNAME 128

class PosixSharedMemory
{
 public:
  PosixSharedMemory (CHAR* _pathname, UINT32 _createFlag, UINT32 _mode, UINT32 _mapFlag, UINT32 size);
  ~PosixSharedMemory ();
  void* getSharedMem () { return sharedMem; }
  UINT32 getSharedMemSize () { return size; }
 private:
  CHAR pathname [MAX_POSIX_SHARED_MEMORY_PATHNAME];
  UINT32 createFlag;
  UINT32 mode;
  UINT32 mapFlag;
  UINT32 size;
  SINT32 shmFd;
  void* sharedMem;
};
#endif
