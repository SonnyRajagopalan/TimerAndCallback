#ifndef TIMER_MIN_HEAP
#define TIMER_MIN_HEAP

#include "common/Types.h"
#include "Timer.h"
#include "PosixSemaphore.h"
#include "PosixSharedMemory.h"
#include "TimerTaskShared.h"
#include "common/Debug.h"

class TimerMinHeap
{
 public:

  friend class TimerTask;

  static TimerMinHeap* getInstance ();

  inline Result addToHeap (Timer* pTimer) 
  {
    return upHeapify (pTimer); 
  }

  inline Result removeFromHeap (Timer* pTimer) 
  { 
    return removeSpecificFromIndex (pTimer, 0); 
  }

  inline UINT32 getNumberOfTimers () 
  { 
    return ((TimerTaskShared*) pTimerMinHeapPool->getSharedMem ())->numberOfTimersBeingManaged; 
  }

  inline void   setNumberOfTimers (UINT32 _numberOfTimersBeingManaged)
  { 
    ((TimerTaskShared*) pTimerMinHeapPool->getSharedMem ())->numberOfTimersBeingManaged = _numberOfTimersBeingManaged; 
  }

  inline void   incNumberOfTimers ()
  { 
    ((TimerTaskShared*) pTimerMinHeapPool->getSharedMem ())->numberOfTimersBeingManaged++; 
  }

  inline void   decNumberOfTimers ()
  { 
    ((TimerTaskShared*) pTimerMinHeapPool->getSharedMem ())->numberOfTimersBeingManaged--; 
  }

  inline Timer** getHeap ()
  {
    return ((TimerTaskShared*) pTimerMinHeapPool->getSharedMem ())->heap; 
  }

  inline void printHeap ()
  {
    Timer** minheap = getHeap();

    std::cout << "TimerMinHeap = ";
    for (unsigned int index = 0; index < getNumberOfTimers (); index++)
      {
	std::cout << std::hex << minheap[index] << std::dec << " ";
      }
    std::cout << std::endl;
  }

 private:

  TimerMinHeap ();
  static TimerMinHeap* instance; // Singleton access *per process*!
  static PosixSharedMemory* pTimerMinHeapPool; // Shared access across processes

  Timer* removeFromIndex (UINT32& index);
  Result removeSpecificFromIndex (Timer* pTimer, UINT32 index);
  void   downHeapifyFromIndex (UINT32 index);
  Result upHeapify (Timer* pTimer);
};

#endif
