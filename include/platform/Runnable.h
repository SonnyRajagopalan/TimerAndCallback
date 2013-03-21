#ifndef RUNNABLE_H
#define RUNNABLE_H

#include "common/Types.h"

class Runnable
{
 public:

  Runnable ();
  ~Runnable () { numberOfRunnables--; }

  const UINT32 getNumberOfRunnables () const { return numberOfRunnables; }

  virtual void initialize (void* pArg) {};
  virtual void run (void* pArg) = 0;
  virtual void exit (void* pArg) {};

 private:

  static UINT32 numberOfRunnables;
  
  // Empty explicit copy constructor; causes a compilation error (good!)
  Runnable (const Runnable& anotherRunnable);

  // Empty explicit copy assignment operator; causes a compilation error (good!)
  Runnable& operator= (const Runnable& anotherRunnable);

};

#endif
