//#include <pthread.h>
#include "platform/PosixThread.h"
#include "platform/Runnable.h"
#include "common/Debug.h"

PosixThread::PosixThread (Runnable* _task, PosixThreadType _type) : 
  task (_task),
  type (_type)
{
  // Create the thread in a detached mode so the parent can terminate.
  // See https://computing.llnl.gov/tutorials/pthreads/#Joining
  if (type == PosixThread::Joinable)
    {
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    }
  else
    {
      pthread_attr_init (&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    }    
}

PosixThread::~PosixThread ()
{
  std::cout << "Killing the thread..." << std::endl;
  if (type == PosixThread::Joinable)
    {
      join ();
      exit ();
    }
  else
    {
      exit ();
    }
}

Result PosixThread::create ()
{
  UINT32 retVal = 0;

  retVal = pthread_create (&thread, &attr, &internalPosixThreadEntryFunction, ((void *)this));

  if (retVal < 0)
    {
      std::cout << "problems Starting the thread..." << std::endl;
      return ERROR;
    }
  else
    {
      std::cout << "Starting the thread (OK)..." << std::endl;
      return OK;
    }

  pthread_attr_destroy (&attr);
  
  if (type == PosixThread::Detached)
    {
      retVal = pthread_detach (thread);
      
      if (retVal < 0)
	{
	  std::cout << "Detachment error!" << std::endl;
	}
    }
}

// We'll have an explicit join method because while
// detaching is automatic and immediate, we want to be able to have
// the flexibility of calling join independently.
Result PosixThread::join ()
{
  UINT32 retVal = 0;

  if (type == PosixThread::Detached)
    {
      std::cout << "Cannot join with a detached thread!" << std::endl;
      return ERROR;
    }

  retVal = pthread_join(thread, NULL);
  
  if (retVal < 0)
    {
      return ERROR;
    }
  else
    return OK;
}

Result PosixThread::exit ()
{
  pthread_exit (NULL);
  return OK;
}

void* PosixThread::internalPosixThreadEntryFunction (void* pArg)
{
  PosixThread* pThr = (PosixThread*) pArg;
  std::cout << "Trying to run task's run()" << std::endl;
  ((Runnable*) (pThr->getTask()))->run (pArg);

  return NULL;
}
