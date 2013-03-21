#ifndef TASK_H
#define TASK_H

#include <string.h>
#include "Runnable.h"
#include "TaskId.h"

#define MAX_TASK_PROCESS_NAME_LEN 15

class Task : public Runnable
{
 public:

  UINT32 getTaskPID () const { return pid; }
  void   setTaskPID (UINT32 _pid) { pid = _pid; }
  void   setTaskId (TaskId::Id _id) { tId.setId (_id); }
  inline void setThisTaskName (const CHAR* _taskName) { strncpy (taskName, _taskName, MAX_TASK_PROCESS_NAME_LEN); }
  inline const CHAR* getThisTaskName () const { return taskName; }

 private:

  TaskId tId;
  UINT32 pid;
  CHAR taskName [MAX_TASK_PROCESS_NAME_LEN];
  
};

#endif
