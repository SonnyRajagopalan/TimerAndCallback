#ifndef TASK_ID_H
#define TASK_ID_H

#include "common/Types.h"

#define MAX_TASKS 3 // Increment as the enum Id {...} below is incremented

class TaskId
{
 public:

  enum Id
    {
      Test                 = 0, // Used for testing only
      ApplicationBaseTask     = 1,
      TimerTask            = 2,
      // Register new applications (tasks) below
      // E.g.:
      // SignalingApplication    = 3,
      // Also make sure that the #define MAX_TASKS above is
      // incremented by 1 for each new task
    };
  TaskId () {};
  TaskId (Id _id) : id (_id) {}
  inline const Id getId () const { return id; }
  inline void setId (const Id _id) { id = _id; }
  inline const CHAR* getPathName () const; // Return the POSIX message queue pathname
  inline static const CHAR* getPathNameFromId (Id _id);

 private:

  Id id;
};

const CHAR* TaskId::getPathName() const
{
  return getPathNameFromId (id);
}

const CHAR* TaskId::getPathNameFromId (TaskId::Id _id)
{
  switch (_id)
    {
    case TaskId::Test:
      return "/Test";
    case TaskId::ApplicationBaseTask:
      return "/ApplicationBaseTask";
    case TaskId::TimerTask:
      return "/TimerTask";
      // Add POSIX message queue path name for new applications below
      // E.g.:
      // case SignalingApplication:
      //   return "/SignalingApplication"
    default:
      return "Invalid";
    }
}

#endif
