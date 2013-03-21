#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include "platform/Timer.h"
#include "platform/TimerTaskShared.h"
#include "platform/TimerUtils.h"
#include "platform/TimerTask.h"
#include "platform/TimerMinHeap.h"
#include "platform/PosixMessageQueueObject.h"
#include "test/TestCallback.h"
#include "common/Util.h"
#include "common/Debug.h"
#include <sstream>
#include <vector>
#include <cstring>
//#include <sys/prctl.h>

void TimerTask::initialize (void* pArg)
{
  UINT32 _pid = fork ();
  setTaskPID (_pid);
  assert (_pid >= 0);

  setTaskId (TaskId::TimerTask);

  switch (getTaskPID ())
    {
    case 0: /* Child process */
      {
	UINT32 mQIndex = 0;

	//prctl(PR_SET_NAME, (unsigned long) "TimerTask", 0, 0, 0);
	setThisTaskName ((const CHAR*) "TimerTask");
	cout << "My process name is " << getThisTaskName () << endl;

	for (mQIndex = 0; mQIndex < MAX_TASKS; mQIndex++)
	  {
	    /** 
		From man mq_open:

		If  O_CREAT  is  specified  in  oflag, then two additional arguments must be supplied.  The mode argument
		specifies the permissions to be placed on the new queue, as for open(2).  (Symbolic definitions  for  the
		permissions bits can be obtained by including <sys/stat.h>.)  The permissions settings are masked against
		the process umask.  The attr argument specifies attributes for the queue.  See mq_getattr(3) for details.
		If attr is NULL, then the queue is created with implementation-defined default attributes.

	    */

	    pMQToTasks [mQIndex] = 
	      new PosixMessageQueue (const_cast<CHAR*>(TaskId::getPathNameFromId (static_cast<TaskId::Id>(mQIndex))), 
				     O_RDWR, 
				     0, 0, 0, 0 // A number of zero-ed out parameters; these parameters, which
				                // can be accessed using mq_getattr (...) are set by the
				                // PMQ creator, not the PMQ accessor
				     );
	  }

	run (pArg);
	break;
      }
    case -1: /* Error */
      {
	assert (0);
      }
    default: /* Parent */
      {
	/*
	  Don't wait() here--if you did, then you will never get return
	  in the lexical scope where the TimerTask::initialize() is called.
	  The proper way would be to TimerTask::initialize () and then
	  call wait on the main lexical scope where the 
	  TimerTask::initialize () is called.
	*/
	cout << "Parent " << getpid () << " will wait for TaskId::TimerTask (PID = " << getTaskPID () << ")" << endl;
      }
    }
}

void TimerTask::run (void* pArg)
{
  TimerMinHeap* heap = TimerMinHeap::getInstance ();

  while (1)
    {
      if (heap->getNumberOfTimers () == 0)
	{
	  usleep (200);
	  continue;
	}

      Timer** minheap = heap->getHeap ();
      Timer* top = minheap [0];
      UINT64 topFiringTime = top->getWhenToFire ();
      Timer::TimerState topState = top->getTimerState ();
      //Timer::TimerType topType = top->getTimerType ();
      UINT64 topTimeout = top->getTimeout ();
      UINT64 current = getCurrentTimeInMsec ();

      //cout << "Loop: top firing time [" << topFiringTime.uint64 << "], current time [" << current.uint64 << "]" << endl;

      if (uint64LEQ (topFiringTime, current))
	{
// 	  COUT << "Some sweet timer action now for timer " << hex << top;
// 	  cout << dec << " with state = " << topState << endl;

	  switch (topState)
	    {
	    case Timer::Active:
	      {		
		UINT64 in = Timer::getIndex (top);
		PosixMessageQueueObject pMQO (TaskId (TaskId::TimerTask), 0, in);
		CHAR* buf = new CHAR [sizeof (PosixMessageQueueObject)];

		COUT << "At " << current.uint64
		     << " Timer " << hex << top << " at index " << dec << in.uint64
		     << " and callback " << hex << top->getCallback() << dec  
		     << " fired." << endl;

		top->incNumberOfTimesFired ();
		heap->removeFromHeap (top);
		top->setTimerState (Timer::Done); // Indicates to ~Timer() that no removal from heap is necessary

		pMQO.serializeToCharBuf (buf);
		pMQToTasks[static_cast<UINT32>(top->getCbContextTaskId ().getId ())]->send (buf, sizeof (PosixMessageQueueObject));

		//COUT << "After triggered: Timer " << hex << top << dec << " at index " << 
		//  in.uint64 << " fired. Calling triggered () on callback." << endl;

		delete[] buf;
		break;
	      }
	    case Timer::Idle:
	      {
		// This is the Timer state of Timer objects which have been cancel()-led or stop-ped! For stopped and cancelled
		// Timers however, they are removed from the heap, so this case may never execute unless the cancel-ling or
		// stopping task context gets switched out of CPU and the TimerTask executes this case.
		COUT << "Timer is Idle. Won't do anything except update the period..." << endl;
		//top->incNumberOfTimesFired (); // Should we count this as "fired?"
		heap->removeFromHeap (top);
		top->changeFiringTime (topTimeout);
		break;
	      }
	    case Timer::Done:
	      {
		// TBD: Is this state even necessary? Ans: 05/08/2012: Yes, for checking if the Timer
		//      object was already removed from heap
		// Do absolutely nothing and continue; this Timer is being handled by callback context task.
		break;
	      }
	    default:
	      {
		COUT << "Timer state is not clear..." << endl;
	      }
	    } // End of switch (topState)
	  
	} // If the top item needs to be managed
      else
	{
	  //cout << "No timer will not fire now... " << endl;
	}
    } // End while
}

TimerTask::~TimerTask ()
{
  // Unlink the PMQs
}
