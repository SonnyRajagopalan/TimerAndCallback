#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "platform/PosixSharedMemory.h"
#include "common/Debug.h"

PosixSharedMemory::PosixSharedMemory (CHAR* _pathname, UINT32 _createFlag, UINT32 _mode, UINT32 _mapFlag, UINT32 _size) :
  createFlag (_createFlag),
  mode (_mode),
  mapFlag (_mapFlag),
  size (_size)
{
  strcpy (pathname, _pathname);
  shmFd = shm_open (pathname, createFlag, mode);

  if (shmFd == -1)
    {
      std::cout << "[" << getpid () << "] Failure opening POSIX shared memory location (" << pathname << ")" 
		<< " with createFlag = " << std::hex << createFlag << std::oct << ", and mode = " << mode << std::dec << std::endl;
    }
  else
    {
      std::cout << "[" << getpid () << "] Opening POSIX shared memory (" << pathname 
		<< ") location with createFlag = " << std::hex << createFlag << std::oct << ", and mode = " << mode << std::dec << std::endl;

      ftruncate (shmFd, size);
      sharedMem = mmap (NULL, size, mapFlag, MAP_SHARED, shmFd, 0);
    }
}

PosixSharedMemory::~PosixSharedMemory ()
{
  munmap (sharedMem, size);

  std::cout << "Unlinking the shared memory...." << std::endl;
  // It would be nice to control this for only the static object 
  // that was created for the class attribute
  // but we will allow any object to delete the mapped memory
  // for now. 03/15/2012.
  //   if (createFlag & O_EXCL) // Only for the "creator" of this shared memory
  shm_unlink (pathname);
}
