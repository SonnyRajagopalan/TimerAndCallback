#include <pthread.h>
#include "common/Debug.h"
#include "platform/Mutex.h"
#include "common/Types.h"

Mutex::Mutex ()
{
  UINT32 retVal = 0;

  retVal = pthread_mutex_init (&mutex, NULL);
  if (retVal < 0)
    {
      std::cout << "Could not create default mutex! " << std::endl;
    }
}

Mutex::Mutex (MutexType _type, ProcessSharedAttrType _psatype)
{
  UINT32 retVal = 0;
  switch (_type)
    {
    case Mutex::Normal:
      std::cout << "Creating a normal mutex " << std::hex << this << std::dec;  
      retVal = pthread_mutexattr_settype (&mutexAttr, PTHREAD_MUTEX_NORMAL);
      break;
    case Mutex::Errorcheck:
      std::cout << "Creating a errorcheck mutex " << std::hex << this << std::dec;  
      retVal = pthread_mutexattr_settype (&mutexAttr, PTHREAD_MUTEX_ERRORCHECK);
      break;
    case Mutex::Recursive:
      std::cout << "Creating a recursive mutex " << std::hex << this << std::dec;  
      retVal = pthread_mutexattr_settype (&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
      break;
    case Mutex::Default:
      std::cout << "Creating a default mutex " << std::hex << this << std::dec;  
      retVal = pthread_mutexattr_settype (&mutexAttr, PTHREAD_MUTEX_DEFAULT);
      break;
    default:
      std::cout << "Mutex creation Error" << std::endl;
    }

  if (retVal < 0)
    std::cout << "Error setting the mutex type..." << std::endl;

  switch (_psatype)
    {
    case Mutex::Shared:
      std::cout << "Creating a process shared mutex " << std::hex << this << std::dec << std::endl;  
      retVal = pthread_mutexattr_setpshared (&mutexAttr, PTHREAD_PROCESS_SHARED);
      break;
    case Mutex::Private:
      std::cout << "Creating a private mutex " << std::hex << this << std::dec << std::endl;  
      retVal = pthread_mutexattr_setpshared (&mutexAttr, PTHREAD_PROCESS_PRIVATE);
      break;
    default:
      std::cout << "Mutex creation Error" << std::endl;
    }

  if (retVal < 0)
    std::cout << "Error setting the mutex process shared type..." << std::endl;

  retVal = pthread_mutex_init (&mutex, &mutexAttr);

  if (retVal < 0)
    {
      std::cout << "Could not create the mutex!" << std::endl;
    }
}

Mutex::~Mutex ()
{
  pthread_mutex_destroy (&mutex);
}

void Mutex::lock ()
{
  //std::cout << "Locking the mutex " << std::hex << this << std::dec << std::endl;
  UINT32 retVal = 0;
  retVal = pthread_mutex_lock (&mutex);
  if (retVal < 0)
    {
      std::cout << "Locking the mutex " << std::hex << this << std::dec << " failed!" << std::endl;
    }
}

void Mutex::unlock ()
{
  UINT32 retVal = 0;
  //std::cout << "Unlocking the mutex " << std::hex << this << std::dec << std::endl;
  pthread_mutex_unlock (&mutex);
  if (retVal < 0)
    {
      std::cout << "Unlocking the mutex " << std::hex << this << std::dec << " failed!" << std::endl;
    }
}

void Mutex::yield ()
{
  UINT32 retVal = 0;

  retVal = pthread_yield ();
  if (retVal < 0)
    {
      std::cout << "Alas! Couldn't yield, and I am a glutton for resources" << std::endl;
    }
}

void Mutex::destroy ()
{
  pthread_mutex_destroy (&mutex);
}
