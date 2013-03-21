#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <pthread.h>
#include "platform/PosixMessageQueueObject.h"
#include "platform/Timer.h"
#include "test/TestCallback.h"
#include "common/Types.h"
#include "platform/TimerTask.h"
#include "platform/TaskId.h"
#include "common/Debug.h"

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
  
	  cout << "[" << getpid() << "] Received message from TaskID = " << (pMQO.getTaskId()).getId ()
	       << " and shm index = " << (pMQO.getShmIndex ()).uint64
	       << " and timer = " << hex << pTimer << dec << endl;

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
  Timer* pobj[20];
  TestCallback* pCb;
  UINT64 timeout;
  UINT32 status = 0;
  UINT32 numberOfRunnables = 0;
  UINT32 numberOfTimesToFire = 10; // One million times
  pthread_t messageQueueThread;
  SINT32 retVal = 0;
  UINT32 timersMax = 1000;

  retVal = pthread_create(&messageQueueThread, NULL, getMessages, NULL);
  tt.initialize (pArg);
  pid = tt.getTaskPID ();
  numberOfRunnables = tt.getNumberOfRunnables ();

  cout << "Parent " << getpid () << " will wait for child " << pid << ". Number of runnables = "
  << numberOfRunnables << " runnables currently...." << endl;

  cout << "Allocate 1 periodic Timer object..." << endl;

  for (int j = 0; j < 100; j++) 
    {
      cout << endl << "Allocating pobj[" << j << "]..." << endl;
      pCb = new TestCallback ();
      timeout.uint64 = 1000 + (j * 100);
      pobj[j] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
      pobj[j]->schedulePeriodicAtTimeout (pCb, timeout, numberOfTimesToFire);
    }

  cout << "Main process will wait for child task to complete. Hopefully the timers will fire before then...." << endl;

  wait (&status);
  pthread_join(messageQueueThread, NULL);
  // Exit operations   
}
 
