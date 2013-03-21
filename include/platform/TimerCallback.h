#ifndef TIMER_CALLBACK_H
#define TIMER_CALLBACK_H

#include "TaskId.h"

class TimerCallback
{
 public:
  
  TimerCallback () {};
  virtual ~TimerCallback ()
  {
    delete pIdOfCallbackContextTask;
  }
  virtual void triggered() = 0; // Will always be the callback function
  const CHAR* getPathNameOfCallbackContextTask () const { return pIdOfCallbackContextTask->getPathName (); }
 private:
  TaskId* pIdOfCallbackContextTask;
  TimerCallback& operator= (const TimerCallback& _anotherTimerCallbackObject);
};

#endif
