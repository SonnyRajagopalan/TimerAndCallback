#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include "common/Types.h"
#include "platform/TimerUtils.h"

UINT64 getCurrentTimeInMsec ()
{
  struct timeval current;
  UINT64 cTime;

  gettimeofday(&current, NULL);
  
  cTime.uint64 = ((current.tv_sec) * 1000 + current.tv_usec/1000.0);

  return cTime;
}

UINT64 convertTimerTimeoutToAbsoluteTimeSinceEpoch (UINT64& timeoutMs)
{
  UINT64 currentMs;
  UINT64 timeSinceEpochMs;
  
  currentMs = getCurrentTimeInMsec ();
  timeSinceEpochMs.uint64 = currentMs.uint64 + timeoutMs.uint64;

  return timeSinceEpochMs;
}

UINT64 convertAbsoluteTimeSinceEpochToRelative (UINT64& absoluteTimeSinceEpoch)
{
  UINT64 currentMs = getCurrentTimeInMsec ();
  UINT64 relativeTimeMs;

  relativeTimeMs.uint64 = 0;

  if ((absoluteTimeSinceEpoch.uint64 - currentMs.uint64) >= 0)
    {      
      relativeTimeMs.uint64 = absoluteTimeSinceEpoch.uint64 - currentMs.uint64;
    }
  else
    {
      // ERROR
    }
  return relativeTimeMs;
}
