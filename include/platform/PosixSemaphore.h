#ifndef POSIX_SEMAPHORE_H
#define POSIX_SEMAPHORE_H

#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "common/Types.h"

#define MAX_POSIX_SEMAPHORE_NAME_LENGTH 64

class PosixSemaphore
{
 public:
  PosixSemaphore (CHAR* _name, UINT32 _flags, UINT32 _mode, UINT32 _value);
  ~PosixSemaphore ();
  void wait () { sem_wait (semaphore); /*std::cout << "[" << getpid () << "] Locked mutex " << name << std::endl*/; }
  void post () { sem_post (semaphore); /*std::cout << "[" << getpid () << "] Unlocked mutex " << name << std::endl*/; }
 private:
  sem_t* semaphore;
  CHAR name [MAX_POSIX_SEMAPHORE_NAME_LENGTH];
  UINT32 flags;
  UINT32 mode;
  UINT32 value;
};
#endif
