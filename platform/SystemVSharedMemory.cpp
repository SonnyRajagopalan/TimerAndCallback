#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <cstring>
#include "platform/SystemVSharedMemory.h"
#include "common/Debug.h"

SystemVSharedMemory::SystemVSharedMemory (BasicString _pathname, CHAR _projectId, UINT32 _size, UINT32 _createFlag, UINT32 _attachFlag, UINT32 _shmctlFlags) :
  pathname (_pathname),
  projectId (_projectId),
  createFlag (_createFlag),
  attachFlag (_attachFlag),
  shmctlFlags (_shmctlFlags),
  size (_size)
{
  std::cout << "[" << getpid () << "] Creating shared memory for process." << std::endl;
  key = ftok (pathname.c_str(), projectId);
  shmId = shmget (key, size, createFlag);
  if (shmId < 0)
    {
      std::cout << "Error creating shared memory for pathname = " << pathname << std::endl;
    }
  sharedMem = shmat (shmId, (void*)0, attachFlag);

  if (sharedMem == (void *) -1)
    {
      std::cout << "Error attaching shared memory for pathname = " << pathname << std::endl;
    }
}

SystemVSharedMemory::~SystemVSharedMemory ()
{
  shmdt (sharedMem);
  shmctl (shmId, shmctlFlags, 0);
}
