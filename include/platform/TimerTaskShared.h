#ifndef TIMER_TASK_SHARED_H
#define TIMER_TASK_SHARED_H

#include "common/Types.h"
#include "Timer.h"
#include "TimerConstants.h"

/*
  Size of the following object is
  1000 * 4 + 4 + sizeof (Mutex)
*/
struct TimerTaskShared
{
 public:
  Timer* heap [MAX_NUMBER_OF_TIMERS];
  UINT32 numberOfTimersBeingManaged;
 private:
  // Nothing
};

#endif
