#ifndef TIMER_H
#define TIMER_H

#include "common/Types.h"
#include "MemoryPool.h"
#include "PosixSemaphore.h"
#include "TaskId.h"

class TimerCallback;

class Timer
{
 public:

  enum TimerType
    {
      OneShot,
      Periodic,
      Invalid // Used to check unscheduled Timer objects
    };

  enum TimerState
    {
      Idle,
      Active,
      Done,
    };

  Timer ();
  Timer (TaskId _creatingTId, TaskId _cbContextTId);

  ~Timer ();

  void scheduleOneShotAtAbsoluteTime       (TimerCallback* _pCb, UINT64& _absoluteTime);
  void scheduleOneShotAtTimeout            (TimerCallback* _pCb, UINT64& _timeout);
  void scheduleOneShotBetweenAbsoluteTimes (TimerCallback* _pCb, UINT64& _absoluteTime1, UINT64& _absoluteTime2);
  void scheduleOneShotBetweenTimeouts      (TimerCallback* _pCb, UINT64& _timeout1, UINT64& _timeout2);
  void schedulePeriodicAtAbsoluteTime      (TimerCallback* _pCb, UINT64& _absoluteTime, UINT64& _timeout, UINT32& _howManyTimesToFire);
  void schedulePeriodicAtTimeout           (TimerCallback* _pCb, UINT64& _timeout, UINT32& _howManyTimesToFire);

  // Removes the timer from heap, resets the timer to current time + timeout, places back on heap (no changes to CB)
  void restart ();
  // Stops the timer: removes from heap. Set the timer type to Timer::Idle. Stopped timers can be restart()-ed later.
  void stop ();
  // Removes the timer callback--this could be dangerous--what if the timer fires, and there is no callback?
  void cancel ();

  inline UINT32 getNumberOfTimesFired () { return numberOfTimesFired; }
  inline void incNumberOfTimesFired () { numberOfTimesFired++; }
  inline UINT32 getHowManyTimesToFire () { return howManyTimesToFire; }
  inline UINT64 getWhenToFire () { return whenToFire; };
  inline void setWhenToFire (UINT64& _whenToFire) { whenToFire = _whenToFire; }
  inline UINT64 getTimeout () { return timeout; }
  inline void setTimeout (UINT64& _timeout) { timeout = _timeout; }
  inline TimerCallback* getCallback() { return pCb; }
  inline TimerState getTimerState () { return state; }
  inline void setTimerState (TimerState _state) { state = _state; }
  inline TimerType getTimerType () { return type; }
  inline void setTimerType (TimerType _type) { type = _type; }
  inline TaskId getCreatingTaskId () { return creatingTId; }
  inline TaskId getCbContextTaskId () { return cbContexTId; }

  inline static UINT64 getIndex (Timer* _pTimer) 
  { 
    return pTimerPool->getIndexFromMemoryLocation (static_cast<void*> (_pTimer)); // Index of the Timer object in the mem pool
  }

  inline static Timer* getTimerAtIndex (UINT64 _index) 
  { 
    return static_cast<Timer*>(pTimerPool->getMemoryLocationFromIndex (_index)); 
  }

  static Result processACallback (Timer* _pTimer);
  void changeFiringTime (UINT64 _newTimeout);
/*   void makeIdle (); */
/*   void makeActive (); */
/*   void makePeriodic (); */
/*   void makeOneShot (); */
  void* operator new (size_t numbytes);
  void operator delete (void* object, size_t size);

 private:

  // Explicit copy contructor; illegal operation should cause compilation error
  Timer (const Timer& anotherTimer);
  // Explicit copy assignment operator; illegal operation should cause compilation error
  Timer& operator= (const Timer& anotherTimer);

  void schedule (TimerCallback* _pCb,
		 UINT64& _first, bool _firstIsAbsolute,
		 UINT64& _next,  bool _nextIsAbsolute,
		 TimerType _type,
		 UINT32& _howManyTimesToFire, bool _isFuzzyOneShot);

  TimerType type;
  TimerState state;
  TimerCallback* pCb;
  TaskId creatingTId;
  TaskId cbContexTId;
  UINT32 howManyTimesToFire;
  UINT32 numberOfTimesFired;   /** Wraps around after 4B fires (136 years for a 1 second timeout) */
  UINT64 first; /** When to fire first. */
  UINT64 timeout; /** Either the periodicity of a PERIODIC timer or the upper bound for "fuzzy timer" implementataion. */
  UINT64 whenToFire; /** In ms, absolute time since the epoch. */
  static MemoryPool* pTimerPool;
  static PosixSemaphore* pTimerSemaphore;
};

#endif
