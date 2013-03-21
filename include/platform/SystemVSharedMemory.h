#ifndef SYSTEM_V_SHARED_MEMORY_H
#define SYSTEM_V_SHARED_MEMORY_H

#include "common/Types.h"

class SystemVSharedMemory
{
 public:
  SystemVSharedMemory (BasicString _pathname, CHAR _projectId, UINT32 _createFlag, UINT32 _attachFlag, UINT32 size, UINT32 _shmctlFlags);
  ~SystemVSharedMemory ();
  void* getSharedMem () { return sharedMem; }
  UINT32 getSharedMemSize () { return size; }
 private:
  BasicString pathname;
  CHAR projectId;
  UINT32 createFlag;
  UINT32 attachFlag;
  UINT32 shmctlFlags;
  UINT32 size;
  UINT32 shmId;
  void* sharedMem;
  key_t key; // Generated from pathname and projectid
};
#endif
