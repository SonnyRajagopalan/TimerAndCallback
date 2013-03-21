#ifndef POSIX_THREAD_H
#define POSIX_THREAD_H

#include "common/Types.h"
#include "Runnable.h"
#include <pthread.h>
#include <iostream>

class PosixThread
{
 public:
  
  enum PosixThreadType
    {
      Joinable,
      Detached,
    };

  PosixThread (Runnable* _task, PosixThreadType _type);
  ~PosixThread ();
  Runnable* getTask () { return task; }
  void printDebug () { std::cout << "From " << thread << std::endl; }
  //protected:

  Result create ();
  Result join ();
  Result exit ();
  Runnable* task; // Need access from derived classes
  
 private:

  pthread_t thread;
  pthread_attr_t attr;
  PosixThreadType type;
  static void* internalPosixThreadEntryFunction (void* pArg);
};
#endif
