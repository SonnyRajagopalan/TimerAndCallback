#include "platform/TimerCallback.h"
#include "common/Debug.h"

TimerCallback& TimerCallback::operator= (const TimerCallback& _anotherTimerCallbackObject)
{
  cout << "TimerCallback base class destructor called" << endl;
  pIdOfCallbackContextTask = _anotherTimerCallbackObject.pIdOfCallbackContextTask;
  return *this;
}
