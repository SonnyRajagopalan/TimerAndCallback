#ifndef TIMER_UTIL_H
#define TIMER_UTIL_H

#include "common/Types.h"

UINT64 getCurrentTimeInMsec ();
UINT64 convertTimerTimeoutToAbsoluteTimeSinceEpoch (UINT64& timeoutMs);
UINT64 convertAbsoluteTimeSinceEpochToRelative (UINT64& absoluteTimeSinceEpoch);

#endif
