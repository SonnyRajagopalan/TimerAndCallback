#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "platform/Timer.h"
#include "platform/TimerCallback.h"
#include "platform/TimerMinHeap.h"
#include "platform/TimerUtils.h"
#include "common/Debug.h"

MemoryPool* Timer::pTimerPool = new MemoryPool ((char*) "TimerObjectPool",
						(char*) "/TimerObjectPool",
						O_RDWR | O_CREAT | O_EXCL, // The exclusive creator of a RD-WR pool
						PROT_READ | PROT_WRITE, 0666, 
						sizeof (Timer), 
						MAX_NUMBER_OF_TIMERS // Total number of timers the pool will hold
						);

PosixSemaphore* Timer::pTimerSemaphore = new PosixSemaphore ((char*) "/TimerSemaphore", 
							     O_CREAT | O_RDWR | O_EXCL, // The exclusive creastor of a RD-WR semaphore
							     0666, 
							     1 // Count
							     );
Timer::Timer () :
  type (Timer::Invalid), // Explicitly spelt out Timer:: scope for clarity
  state (Timer::Idle),
  pCb (NULL)
{
  first.uint64 = 0;
  timeout.uint64 = 0;
  // Uninitialized type and other variables.
}

Timer::Timer (TaskId _creatingTId, TaskId _cbContextTId) :
  creatingTId (_creatingTId), cbContexTId (_cbContextTId) 
{
}

void Timer::scheduleOneShotAtAbsoluteTime (TimerCallback* _pCb, UINT64& _absoluteTime)
{
  UINT64 dummy;
  UINT32 zeroTimes = 0;

  schedule (_pCb, 
	    _absoluteTime, true, 
	    dummy, false,
	    Timer::OneShot,
	    zeroTimes, false);
}

void Timer::scheduleOneShotAtTimeout (TimerCallback* _pCb, UINT64& _timeout)
{
  UINT64 dummy;
  UINT32 zeroTimes = 0;

  schedule (_pCb,
	    _timeout, false,
	    dummy, false,
	    Timer::OneShot,
	    zeroTimes, false);
}

void Timer::scheduleOneShotBetweenAbsoluteTimes (TimerCallback* _pCb, UINT64& _absoluteTime1, UINT64& _absoluteTime2)
{
  UINT32 zeroTimes = 0;

  schedule (_pCb,
	    _absoluteTime1, true,
	    _absoluteTime2, true,
	    Timer::OneShot,
	    zeroTimes, true);
}

void Timer::scheduleOneShotBetweenTimeouts (TimerCallback* _pCb, UINT64& _timeout1, UINT64& _timeout2)
{
  UINT32 zeroTimes = 0;

  schedule (_pCb,
	    _timeout1, false,
	    _timeout2, false,
	    Timer::OneShot,
	    zeroTimes, true);
}

void Timer::schedulePeriodicAtAbsoluteTime (TimerCallback* _pCb, UINT64& _absoluteTime, UINT64& _timeout, UINT32& _howManyTimesToFire)
{
  schedule (_pCb,
	    _absoluteTime, true,
	    _timeout, false,
	    Timer::Periodic,
	    _howManyTimesToFire, false);
}

void Timer::schedulePeriodicAtTimeout (TimerCallback* _pCb, UINT64& _timeout, UINT32& _howManyTimesToFire)
{
  schedule (_pCb,
	    _timeout, false,
	    _timeout, false,
	    Timer::Periodic,
	    _howManyTimesToFire, false);
}

void Timer::schedule (TimerCallback* _pCb,
		      UINT64& _first, bool _firstIsAbsolute,
		      UINT64& _timeout,  bool _timeoutIsAbsolute,
		      TimerType _type,
		      UINT32& _howManyTimesToFire, bool _isFuzzyOneShot)
{
  TimerMinHeap* heap = TimerMinHeap::getInstance ();

  pTimerSemaphore->wait ();

  if (!pCb)
    {
      pCb = _pCb;
    }
  else
    {
      cout << "There is already a callback associated with this timer. Remove the callback first using cancel ()!" << endl;
    }

  state = Timer::Active;
  type = _type;
  howManyTimesToFire = _howManyTimesToFire;

  /*
    Work out the fuzzy one shot timer here.
  */

  if (_isFuzzyOneShot)
    {      
      UINT64 x1, x2, randomTimeout;

      if (type != Timer::OneShot)
	{
	  cout << "Incorrect parameters. ERROR!" << endl;
	}

      if (_firstIsAbsolute)
	  x1 = convertAbsoluteTimeSinceEpochToRelative (_first);
      else
	x1 = _first;

      if (_timeoutIsAbsolute)
	x2 = convertAbsoluteTimeSinceEpochToRelative (_timeout);
      else
	x2 = _timeout;

      if (x2.uint64 < x1.uint64)
	cout << "Incorrect parameters. ERROR!" << endl;
      else
	{
	  randomTimeout.uint64 = x1.uint64 + 1 + (rand () % (x2.uint64 - x1.uint64));
	  whenToFire = convertTimerTimeoutToAbsoluteTimeSinceEpoch (randomTimeout);
	}
    }
  else if (_firstIsAbsolute)
    {
      whenToFire = first;
    }
  else
    {      
      first.uint64 = _first.uint64;
      whenToFire = convertTimerTimeoutToAbsoluteTimeSinceEpoch (first);
    }

  if (_timeoutIsAbsolute)
    {
      timeout = convertAbsoluteTimeSinceEpochToRelative (_timeout);
    }
  else
    {
      timeout.uint64 = _timeout.uint64;      
    }

  numberOfTimesFired = 0;

  if ((type == OneShot) && (howManyTimesToFire != 0))
    {
      cout << "Error--OneShot can only fire once!" << endl;
    }

  if (heap != NULL)
    {
      heap->addToHeap (this);
    }
  else
    {
      cout << "heap is NULL" << endl;
    }

  pTimerSemaphore->post ();
}


void Timer::cancel ()
{
  cout << "[" << getpid () << "] Timer cancel () called" << endl;

  pTimerSemaphore->wait ();

  if (pCb)
    {
      delete pCb;

      pCb = NULL;
    }
  else
    {
      cout << YELLOW_BOLD << "In cancel, the callback was already destroyed, looks like..." << END_COLOR << endl;
      // return ERROR;
    }
  setTimerState (Timer::Idle);

  pTimerSemaphore->post ();
}

void Timer::restart ()
{
  TimerMinHeap* heap = TimerMinHeap::getInstance ();
  UINT64 nextFiringTime;

  cout << "[" << getpid () << "] Timer reset () called" << endl;

  pTimerSemaphore->wait ();
  
  if (state == Timer::Active)
    {
      setTimerState (Timer::Idle);
      heap->removeFromHeap (this);
    }
  else
    {
      if (pCb != NULL) // Cancelled timers cannot be restarted!
	{
	  // Restarting a previously stopped timer	  
	}
      else
	{
	  // Log an error here! An attempt to restart a cancelled timer!
	}
    }
  nextFiringTime = convertTimerTimeoutToAbsoluteTimeSinceEpoch (timeout);
  setWhenToFire (nextFiringTime);
  setTimerState (Timer::Active); // Note: otherwise, the timer won't fire again!
  heap->addToHeap (this);
  
  pTimerSemaphore->post ();
}

void Timer::stop ()
{
  TimerMinHeap* heap = TimerMinHeap::getInstance ();

  cout << "[" << getpid () << "] Timer reset () called" << endl;

  pTimerSemaphore->wait ();

  setTimerState (Timer::Idle);
  heap->removeFromHeap (this);

  pTimerSemaphore->post ();  
}

void Timer::changeFiringTime (UINT64 _newTimeout)
{
  TimerMinHeap* heap = TimerMinHeap::getInstance ();
  UINT64 nextFiringTime;

  //cout << "[" << getpid () << "] Timer changeFiringTime () called" << endl;

  pTimerSemaphore->wait ();

  //heap->removeFromHeap (this); Bug correction made on 05/10/2012: Do not remove from heap here. Call flow here comes from TimerTask
  nextFiringTime = convertTimerTimeoutToAbsoluteTimeSinceEpoch (_newTimeout);
  setTimeout (_newTimeout);
  setWhenToFire (nextFiringTime);
  setTimerState (Timer::Active); // Note: otherwise, the timer won't fire again!
  heap->addToHeap (this);

  pTimerSemaphore->post ();
  //cout << "[" << getpid () << "] Timer firing time changed to " << nextFiringTime.uint64 << endl;
}

Timer::~Timer ()
{
  cout << "[" << getpid () << "] Timer destructor called for timer object = " << hex << this << dec << endl;
  TimerMinHeap* heap = TimerMinHeap::getInstance ();

  pTimerSemaphore->wait ();

  // INVALID timers are timers not added into the heap.
  if ((type == Timer::Invalid) || (state == Timer::Done))
    {
      // Don't remove from heap:
      // type == INVALID type timers were not scheduled in the first place,
      // and state == Done timers were removed from the heap in the TimerTask.
    }
  else
    {
      cout << RED_BOLD << "[" << getpid () << "] About to remove timer (~INVALID && ~Done) " << hex << this << " from heap..." << dec << END_COLOR << endl;
      heap->removeFromHeap (this);
    }

  if (pCb) // If this Timer object may have been cancelled before, pCb will be == NULL
    {
      cout << GREEN_BOLD << "[" << getpid () << "] The Timer callback about to be destroyed..." << END_COLOR << endl;      

      delete pCb;
    }
  else
    {
      cout << YELLOW_BOLD << "[" << getpid () << "] The Timer callback was already destroyed..." << END_COLOR << endl;
    }

  pCb = 0;

  pTimerSemaphore->post ();

  cout << "[" << getpid () << "] Timer object " << hex << this << dec << " removed from heap, will now remove from memory pool..." << endl;
}

void* Timer::operator new (size_t size)
{
  void* newObject;

  pTimerSemaphore->wait ();

  newObject = pTimerPool->getFromPool ();

  pTimerSemaphore->post ();

  return newObject;
}

void Timer::operator delete (void* object, size_t size)
{
  cout << YELLOW_BOLD << "[" << getpid () << "] Calling overloaded Timer delete..." << END_COLOR << endl;

  pTimerSemaphore->wait ();

  pTimerPool->removeFromPool (object);

  pTimerSemaphore->post ();
}

Result Timer::processACallback (Timer* pTimer)
{
  Timer::TimerType timerType = pTimer->getTimerType ();
  TimerCallback* pCb = pTimer->getCallback ();
  
  switch (timerType)
    {
    case Timer::OneShot:
      {
	// Execute the callback

	if (pCb)
	  {
	    pCb->triggered ();
	  }
	else
	  {
	    // The timer was either cancel()-ed or the callback removed another way.
	    return ERROR;
	  }
	// Delete the timer
	delete pTimer;
	pTimer = 0;

	break;
      }
    case Timer::Periodic:
      {
	UINT64 timerTimeout = pTimer->getTimeout ();
	UINT32 howManyFirings = pTimer->getHowManyTimesToFire ();
	UINT32 numberOfFirings = pTimer->getNumberOfTimesFired ();

	if (pCb)
	  {	    
	    pCb->triggered ();
	  }
	else
	  {
	    // The timer was either cancel()-ed or the callback removed another way.
	    return ERROR;
	  }

	//if ((pTimer->getNumberOfTimesFired () < pTimer->getHowManyTimesToFire ()) ||
	if (
	    (numberOfFirings <= howManyFirings) ||
	    (howManyFirings == 0)
	    )
	  {
	    pTimer->changeFiringTime (timerTimeout);
	  }
	else
	  {
	    // Remove the timer object
	    cout << "[" << getpid() << "] Timer reached the maximum number of times to fire (" 
	      //<< pTimer->getHowManyTimesToFire () << "). Deleting object." << endl;
		 << howManyFirings << "). Deleting object." << endl;
	    delete pTimer;
	  }
	
	break;
      }
    default:
      {
	/* Error */
	return ERROR;
      }
    } // End of switch
  return OK;
}
