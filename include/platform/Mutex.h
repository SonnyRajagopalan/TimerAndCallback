#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

class Mutex
{
 public:

  enum MutexType
    {
      Normal,
      Errorcheck,
      Recursive,
      Default
    };

  enum ProcessSharedAttrType
    {
      Shared, // Across processes
      Private // Shared only within threads of the sameprocess
    };

  Mutex ();
  Mutex (MutexType _mtype, ProcessSharedAttrType _psatype);
  ~Mutex ();
  void lock ();
  void unlock ();
  void destroy ();
  void yield ();
 private:
  pthread_mutex_t mutex;
  pthread_mutexattr_t mutexAttr;
};
#endif
