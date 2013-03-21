#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "platform/TimerConstants.h"
#include "test/TestCallback.h"
#include "common/Debug.h"

MemoryPool* TestCallback::testCallbackPool =  new MemoryPool ((char*) "testCallbackPool",
							      (char*) "/testCallbackPool",
							      O_RDWR | O_CREAT | O_EXCL, PROT_READ | PROT_WRITE, 0666, 
							      sizeof (TestCallback), MAX_NUMBER_OF_TIMERS);

PosixSemaphore* TestCallback::pTestCallbackSemaphore = new PosixSemaphore ((char*) "/testCallbackSemaphore", 
									   O_CREAT | O_RDWR | O_EXCL, 
									   0666, 
									   1);
TestCallback::TestCallback ()
{
  // Empty ctor that does nothing
  //cout << "[" << getpid () << "] TestCallback ctor called..." << endl;
}

void TestCallback::triggered ()
{
  cout << "[" << getpid () << "] Hello from TestCallback at location " << hex << this << dec << endl;
  return;
}

TestCallback::~TestCallback ()
{
  //cout << "[" << getpid () << "] TestCallback dtor called!" << endl;
  // Will now call TestCallback::delete()
}

void* TestCallback::operator new (size_t size)
{
  void* location = NULL;

  pTestCallbackSemaphore->wait ();

  location = testCallbackPool->getFromPool ();

  pTestCallbackSemaphore->post ();

  cout << "[" << getpid () << "] TestCallback::operator new: Returning location = " << hex << location << dec << endl;
  return location;
}

void TestCallback::operator delete (void* object, size_t size)
{
  cout << "[" << getpid () << "] TestCallback::operator delete (void* object) member function called..." << endl;

  pTestCallbackSemaphore->wait ();

  testCallbackPool->removeFromPool (object);

  pTestCallbackSemaphore->post ();
}
