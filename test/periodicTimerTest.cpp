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
  Timer* pobj[20];
  TestCallback* pCb;
  UINT64 timeout;
  UINT32 status = 0;
  UINT32 numberOfRunnables = 0;
  UINT32 numberOfTimesToFire = 10;
  pthread_t messageQueueThread;
  SINT32 retVal = 0;

  retVal = pthread_create(&messageQueueThread, NULL, getMessages, NULL);
  tt.initialize (pArg);
  pid = tt.getTaskPID ();
  numberOfRunnables = tt.getNumberOfRunnables ();

  cout << "Parent " << getpid () << " will wait for child " << pid << "... " << endl;
  cout << "Parent: " << numberOfRunnables << " runnables currently...." << endl;

  cout << "Allocate 1 periodic Timer object..." << endl;

  for (int j = 0; j < 19; j++) 
    {
      cout << endl << "Allocating pobj[" << j << "]..." << endl;
      pCb = new TestCallback ();
      timeout.uint64 = 1000 + (j * 100);
      pobj[j] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
      pobj[j]->schedulePeriodicAtTimeout (pCb, timeout, numberOfTimesToFire);
    }

  //sleep (15); // Wait for the timers to all fire.
  
  SEPARATOR
  SEPARATOR
  SEPARATOR
  SEPARATOR
  SEPARATOR
  
  pCb = new TestCallback ();
  pobj[19] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
  timeout.uint64 = 1345;
  pobj[19]->schedulePeriodicAtTimeout (pCb, timeout, numberOfTimesToFire);

//   SEPARATOR
//   SEPARATOR
//   cout << endl << "Deleting objects pobj[2] and pobj[5]..." << endl;
//   delete pobj[2];
//   delete pobj[5];
//   SEPARATOR
//   SEPARATOR

//   cout << endl << "Allocate p-2, allocate p-10, p11 (not active)" << endl << endl << endl;

//   pobj[2] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
//   pCb = new TestCallback ();
//   timeout.uint64 = 15000;
//   pobj[2]->scheduleOneShotAtTimeout (pCb, timeout);

//   pobj[10] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
//   pCb = new TestCallback ();
//   timeout.uint64 = 16000;
//   pobj[10]->scheduleOneShotAtTimeout (pCb, timeout);

//   pobj[11] = new Timer (TaskId (TaskId::Test), TaskId (TaskId::Test));
//   pCb = new TestCallback ();
//   timeout.uint64 = 17000;
//   pobj[11]->scheduleOneShotAtTimeout (pCb, timeout);

//   cout << endl << endl;

  cout << "Main process will wait for child task to complete. Hopefully the timers will fire before then...." << endl;

  wait (&status);
  pthread_join(messageQueueThread, NULL);
  // Exit operations   
}
 
