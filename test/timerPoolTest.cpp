#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "platform/Timer.h"
#include "test/TestCallback.h"
#include "common/Types.h"
#include "platform/TimerTask.h"
#include "platform/TimerConstants.h"
#include "platform/TaskId.h"
#include "platform/PosixMessageQueue.h"
#include "platform/PosixMessageQueueObject.h"
#include "common/Debug.h"
//#include "platform/TimerExpiryMessage.h"
#include "platform/MemoryPool.h"
#include <pthread.h>
#include <string.h>
#include <vector>
#include "common/Util.h"
#include <sstream>
using namespace std;

void* getMessages (void* pArg)
{
  SINT32 retVal = 0;
  PosixMessageQueue* pMq = new PosixMessageQueue ((char *) "/Test", O_RDWR | O_EXCL | O_CREAT, 0, 0666, 10, 4096);
  char* message = reinterpret_cast<char*>(pArg);
  
  cout << "Thread for catching messages to pMq created..." << endl;

  while (1)
    {
      retVal = pMq->receive ();
      if (retVal != -1)
	{	  
	  PosixMessageQueueObject pMQO;
	  CHAR* buf = pMq->getMsg ();
	  Timer* pTimer;

	  pMQO.deserializeFromCharBuf (buf);
	  pTimer = Timer::getTimerAtIndex(pMQO.getShmIndex ());
  
	  cout << "[" << getpid() << "] Received message from TaskId = " << (pMQO.getTaskId()).getId () << " and shm index = " << (pMQO.getShmIndex ()).uint64 << 
	    " and timer = " << hex << pTimer << dec << endl;

	  Timer::processACallback (pTimer);
	} // Good message from MQ
      else
	{
	  cout << "Bad message received!" << endl;
	}
    }// End while
}

int main()
{
  TimerTask tt;
  UINT32 pid = 0;
  void* pArg;
  Timer* pobj[15];
  TestCallback* pCb;
  UINT64 timeout;
  UINT32 status = 0;
  UINT32 no = 0;
  pthread_t messageQueueThread;
  SINT32 retVal = 0;

  /**
    Compile time error for the following: (Good!)

    TimerTask tt1;
  
    tt1 = tt;
  */
//   cout << "Memory pool to be created on timerPool" << endl;
//   MemoryPool* pTimerPool = new MemoryPool ((char*) "timerObjectPool", 
// 					   (char*) "/timerObjectPool",
// 					   O_RDWR,
// 					   PROT_READ | PROT_WRITE, 
// 					   0666, 
// 					   sizeof (Timer), 
// 					   MAX_NUMBER_OF_TIMERS);

//   MemoryPool* testCallbackPool =  new MemoryPool ((char*) "testCallbackPool",
// 								(char*) "/testCallbackPool",
// 								O_RDWR, PROT_READ | PROT_WRITE, 0666, 
// 								sizeof (TestCallback), MAX_NUMBER_OF_TIMERS);

//   PosixSemaphore* pTestCallbackSemaphore = new PosixSemaphore ((char*) "/testCallbackSemaphore", 
// 									     O_CREAT, 
// 									     0666, 
// 									     1);
//   PosixSharedMemory* pTimerMinHeapPool = new PosixSharedMemory ((char*) "/timerMinheapPool", 
// 								O_RDWR, 
// 								0666, 
// 								PROT_READ | PROT_WRITE, 
// 								(sizeof (UINT32) + (sizeof (Timer*) * MAX_NUMBER_OF_TIMERS)));
  retVal = pthread_create(&messageQueueThread, NULL, getMessages, NULL);

  tt.initialize (pArg);
  pid = tt.getTaskPID ();
  no = tt.getNumberOfRunnables ();

  cout << "Parent " << getpid () << " will wait for child " << pid << "... " << endl;
  cout << "Parent: " << no << " runnables currently...." << endl;

  sleep (1);
  cout << "Allocate one timer object" << endl;
  pobj[14] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
  //cout << "Index = " << (Timer::getIndex (pobj[14])).uint64 << endl;
  cout << "Now, delete this timer object" << endl;

  delete pobj[14];
  cout << "What did you see?" << endl;
  cout << "Allocate 10 Timer objects..." << endl;

  for (int j = 0; j < 10; j++) 
    {
      UINT64 in;
      cout << endl << "Allocating pobj[" << j << "]..." << endl;
      pCb = new TestCallback ();
      timeout.uint64 = 1000 * (j + 1);
      pobj[j] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
      pobj[j]->scheduleOneShotAtTimeout (pCb, timeout);
      //in = Timer::getIndex (pobj[j]);
      //cout << "Index = " << in.uint64 << endl;
    }
  
  SEPARATOR
  SEPARATOR
  cout << endl << "Deleting objects pobj[2] and pobj[5]..." << endl;
  delete pobj[2];
  delete pobj[5];
  SEPARATOR
  SEPARATOR

  cout << endl << "Allocate p-2, allocate p-10, p11 (not active)" << endl << endl << endl;

  pobj[2] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
  pCb = new TestCallback ();
  timeout.uint64 = 15000;
  pobj[2]->scheduleOneShotAtTimeout (pCb, timeout);

  pobj[10] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
  pCb = new TestCallback ();
  timeout.uint64 = 16000;
  pobj[10]->scheduleOneShotAtTimeout (pCb, timeout);

  pobj[11] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
  pCb = new TestCallback ();
  timeout.uint64 = 17000;
  pobj[11]->scheduleOneShotAtTimeout (pCb, timeout);

  cout << endl << endl;

  cout << "Main process will wait for child task to complete. Hopefully the timers will fire before then...." << endl;

  pthread_join(messageQueueThread, NULL);
  wait (&status);

  // Exit operations   
}
