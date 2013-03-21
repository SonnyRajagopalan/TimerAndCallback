#include <math.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <semaphore.h>
#include "platform/TimerMinHeap.h"
#include "platform/TimerTaskShared.h"
#include "platform/PosixSemaphore.h"
#include "platform/PosixSharedMemory.h"
#include "common/Util.h"
#include "common/Debug.h"

PosixSharedMemory* TimerMinHeap::pTimerMinHeapPool = new PosixSharedMemory ((char*) "/TimerMinheapPool", 
									    O_RDWR | O_CREAT | O_EXCL, // The exclusive creator of a RD-WR pool
									    0666, 
									    PROT_READ | PROT_WRITE, 
									    (sizeof (UINT32) + 
									     (sizeof (Timer*) * MAX_NUMBER_OF_TIMERS)) // Enough space for both mgmt and Timer 
									                                               // pointers
									    );
TimerMinHeap* TimerMinHeap::instance = 0;

TimerMinHeap* TimerMinHeap::getInstance() // Yes, this will give different instance pointers in different tasks!
{
  if (!instance)
    {
      instance = new TimerMinHeap();
      COUT << "TimerMinHeap getInstance () created: " << hex << instance << dec
	   << " :: " << sizeof(TimerMinHeap) << " bytes. " << endl;
    }

  //COUT << "TimerMinHeap getInstance () returned " << hex << instance << dec << endl;
  return instance;
}

TimerMinHeap::TimerMinHeap()
{
}

/**
   Called by: addToHeap (Timer*)
*/
Result TimerMinHeap::upHeapify (Timer* pTimer)
{
  // Insert into minheap, minheapify
  // At index i; children at (2i+1) and (2i+2)
  // If minheap property is satisfied, then we are done!
  UINT32 heaped = 0;
  Timer** minheap = getHeap ();
  UINT32 index = getNumberOfTimers ();
  minheap [index] = pTimer;

  if (index == 0) // The first timer
    {
    }
  else
    {
      while (!heaped)
	{
	  UINT32 parent = floor((index - 1)/2);
	  UINT64 parentFiringTime = minheap [parent]->getWhenToFire ();
	  UINT64 indexFiringTime = minheap [index]->getWhenToFire ();
	  if (uint64LT (parentFiringTime, indexFiringTime))
	    {
	      heaped = 1;
	    }
	  else
	    {
	      Timer* tmp = minheap [index];
	      minheap [index] = minheap [parent];
	      minheap [parent] = tmp;
	      
	      index = parent;
	    }

	  if (parent == 0)
	    heaped = 1;	  
	}
    }
  incNumberOfTimers ();
  //COUT << "TimerMinHeap::upHeapify (): Current number of timers = " << getNumberOfTimers () << endl;
  COUT << "Added timer " << hex << pTimer 
       << " to timer minheap [" << this << dec << "]" << " (number of timers in heap = "
       << getNumberOfTimers () << ")." << endl;
  printHeap ();
  return OK;
}

Timer* TimerMinHeap::removeFromIndex (UINT32& index)
{
  // first, flip the top of the minheap with the bottom of the minheap, found in minheap [numberOfTimersBeingManaged]
  Timer** minheap = getHeap ();
  UINT32 parent = index;
  Timer* pTimer = minheap [parent];

  minheap [parent] = minheap [getNumberOfTimers () - 1]; // Disrupt the minheap property
  decNumberOfTimers ();
  downHeapifyFromIndex (index);

  COUT << "TimerMinHeap:: Removed item at index = " << index << endl;

  return pTimer;
}

Result TimerMinHeap::removeSpecificFromIndex (Timer* pTimer, UINT32 index)
{
  UINT32 parent = index;
  UINT32 left = parent * 2 + 1;
  UINT32 right = left + 1;
  Timer** minheap = getHeap ();
  UINT32 numberOfTimers = getNumberOfTimers ();

  //COUT << "TimerMinHeap::removeSpecificFromIndex (): Left = " << left << ", right = " << right << " and pTimer = " << hex << pTimer 
  //   << " and minheap [" << parent << "] = " << minheap [parent] << dec << " and number of timers = " << numberOfTimers << endl;

  if (minheap [parent] == pTimer)
    {
      minheap [parent] = minheap [getNumberOfTimers () - 1];
      decNumberOfTimers ();

      downHeapifyFromIndex (parent);
    }
  else if ((left < (numberOfTimers - 1)) && (minheap [left] == pTimer))
    {
      minheap [left] = minheap [getNumberOfTimers () - 1];
      decNumberOfTimers ();

      downHeapifyFromIndex (left);
    }
  else if ((right < (numberOfTimers - 1)) && (minheap [right] == pTimer))
    {
      minheap [right] = minheap [getNumberOfTimers () - 1];
      decNumberOfTimers ();
      
      downHeapifyFromIndex (right);
    }
  //else if ((left > (numberOfTimers - 1)) || (right > (numberOfTimers - 1))): 1. Corrections made 05/08/2012:
  //else if ((left > (numberOfTimers - 1)) || (right > (numberOfTimers))): 2. Correctiosn made 05/08/2012:
  else if (left > numberOfTimers)
    {
      COUT << "Returning ERROR: Left = " << left << ", Right = " << right << ", numberOfTimers = " << numberOfTimers << endl;
      return ERROR;
    }
  else
    {
      COUT << "Calling removeSpecificFromIndex with (pTimer, " << (parent + 1) << ")" << endl;
      return removeSpecificFromIndex (pTimer, parent + 1);
    }  
  return OK;
}

void TimerMinHeap::downHeapifyFromIndex (UINT32 index)
{
  UINT32 heaped = 0;
  UINT32 parent = index;

  while (!heaped)
    {
      Timer** minheap = getHeap ();
      UINT32 left = 2 * parent + 1;
      UINT32 right = left + 1;
      UINT64 parentFiringTime = minheap [parent]->getWhenToFire ();
      UINT64 leftFiringTime, rightFiringTime;

      //cout << "Left = " << left << ", right = " << right << ", n = " << getNumberOfTimers () << endl;

      if (left < getNumberOfTimers ())
	{
	  //cout << "Left firing time..." << endl;
	  leftFiringTime = minheap [left]->getWhenToFire ();
	}
      else
	{
	  heaped = 1;
	  //return;
	}
	    
      if (right <= getNumberOfTimers ())
	rightFiringTime = minheap [right]->getWhenToFire ();
      else
	{
	  heaped = 1;
	}
      if ((left >= (getNumberOfTimers ())) || (right >= getNumberOfTimers ())) 
	{
	  heaped = 1;
	  //cout << "TimerMinHeap:: Returning from downHeapifyFromIndex (exit #1)" << endl;
	  return;
	}

      if (uint64LT (leftFiringTime, parentFiringTime) ||
	  uint64LT (rightFiringTime, parentFiringTime))
	{
	  if (uint64LT (leftFiringTime, rightFiringTime))
	    {
	      // the one on the left is lower
	      Timer* tmp = minheap [left];
	      minheap [left] = minheap [parent];
	      minheap [parent] = tmp;	      

	      parent = left;
	    }
	  else if (uint64GEQ (leftFiringTime, rightFiringTime))
	    {
	      // the one on the right is lower
	      Timer* tmp = minheap [right];
	      minheap [right] = minheap [parent];
	      minheap [parent] = tmp;	      
	      
	      parent = right;
	    }
	}
      else
	{
	  heaped = 1;
	}
    } // End While
  //cout << "Returning from downHeapifyFromIndex (exit #2)" << endl;
}
