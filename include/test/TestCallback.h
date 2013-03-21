#ifndef TEST_CALLBACK_H
#define TEST_CALLBACK_H

#include "platform/TimerCallback.h"
#include "platform/MemoryPool.h"
#include "platform/PosixSemaphore.h"

class TestCallback : public TimerCallback
{
 public:
  virtual void triggered ();
  TestCallback ();
  ~TestCallback ();
  void* operator new (size_t size);
  void operator delete (void* object, size_t size);
 private:
  static MemoryPool* testCallbackPool;
  static PosixSemaphore* pTestCallbackSemaphore;
};
#endif
