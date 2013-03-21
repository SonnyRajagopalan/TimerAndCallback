#ifndef TIMER_TASK_H
#define TIMER_TASK_H

#include "Runnable.h"
#include "PosixMessageQueue.h"
#include "Task.h"

class TimerTask : public Task
{
 public:

  TimerTask () {}; // No known use for constructor yet
  ~TimerTask ();   // Unlink the PMQs

  void initialize (void* pArg);
  void run (void* pArg);  

 private:

  PosixMessageQueue* pMQToTasks [MAX_TASKS];

  // Empty explicit copy contructor here; illegal operation
  TimerTask (const TimerTask& anotherTimerTask);

  // Emply explicit copy assignment operator here; illegal operation
  TimerTask& operator= (const TimerTask& anotherTimerTask);

};

#endif
